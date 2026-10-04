/*
 * server.c - агент сбора информации о компьютере.
 *
 * Архитектура: порт завершения ввода-вывода (IOCP) + пул рабочих потоков.
 * У каждого подключения в любой момент времени есть РОВНО ОДНА незавершённая
 * операция (приём или отправка), поэтому при получении уведомления о её
 * завершении сокет и контекст можно безопасно закрыть/освободить.
 *
 * Жизненный цикл подключения (один запрос на подключение):
 *   ST_WAIT_PUBKEY    <- клиент прислал публичный RSA-ключ
 *   ST_SEND_KEY       -> сервер отправил сеансовый AES-ключ, зашифрованный этим ключом
 *   ST_WAIT_REQUEST   <- зашифрованный запрос
 *   ST_SEND_RESPONSE  -> зашифрованный ответ, затем соединение закрывается
 */
#include "common.h"
#include <mswsock.h>
#include <process.h>

#define ACCEPT_POSTED    8        /* сколько AcceptEx одновременно ждут клиентов */
#define MAX_CONNS        2000
#define IDLE_TIMEOUT_MS  30000    /* бездействующее подключение закрывается */
#define KEY_LISTEN       ((ULONG_PTR)1)
#define KEY_STOP         ((ULONG_PTR)2)

typedef enum { OP_ACCEPT = 1, OP_RECV, OP_SEND } op_type;

typedef struct {
    OVERLAPPED ov;      /* должен быть первым */
    op_type    type;
} io_op;

typedef struct accept_ctx {
    io_op  op;
    SOCKET sock;
    DWORD  bytes;
    char   addrs[2 * (sizeof(struct sockaddr_in) + 16)];
} accept_ctx;

typedef enum { ST_WAIT_PUBKEY, ST_SEND_KEY, ST_WAIT_REQUEST, ST_SEND_RESPONSE } conn_state;

typedef struct conn {
    io_op       rx_op, tx_op;
    SOCKET      sock;
    conn_state  state;
    unsigned    id;
    char        peer[40];
    DWORD       rx_flags;
    DWORD       rx_len;
    BYTE        rx[4 + SERVER_RX_MAX];
    BYTE       *tx;
    DWORD       tx_len, tx_sent;
    HCRYPTKEY   sess;
    volatile DWORD last_tick;
    struct conn *prev, *next;
} conn;

static HANDLE           g_port;
static SOCKET           g_listen = INVALID_SOCKET;
static volatile LONG    g_stop;
static volatile LONG    g_next_id;
static conn            *g_head;
static int              g_nconns;
static CRITICAL_SECTION g_list_cs;

/* ------------------------------------------------------------------ */
static void log_line(const char *fmt, ...)
{
    SYSTEMTIME t;
    char head[32];
    va_list ap, ap2;
    GetLocalTime(&t);
    sprintf(head, "[%02u:%02u:%02u] ", t.wHour, t.wMinute, t.wSecond);

    va_start(ap, fmt);
    va_copy(ap2, ap);
    int n = _vscprintf(fmt, ap);
    char *body = (char *)malloc((size_t)n + 1);
    if (body) {
        _vsnprintf(body, (size_t)n + 1, fmt, ap2);
        body[n] = 0;
        /* одним вызовом, чтобы строки разных потоков не перемешивались */
        char *line = (char *)malloc(strlen(head) + (size_t)n + 2);
        if (line) {
            sprintf(line, "%s%s\n", head, body);
            con_write(line);
            free(line);
        }
        free(body);
    }
    va_end(ap2);
    va_end(ap);
}

/* ------------------------------------------------------------------ */
/* Подключения                                                         */
/* ------------------------------------------------------------------ */
static conn *conn_new(SOCKET s, const struct sockaddr_in *peer)
{
    conn *c;
    EnterCriticalSection(&g_list_cs);
    if (g_nconns >= MAX_CONNS) {
        LeaveCriticalSection(&g_list_cs);
        return NULL;
    }
    c = (conn *)calloc(1, sizeof(conn));
    if (!c) {
        LeaveCriticalSection(&g_list_cs);
        return NULL;
    }
    c->sock = s;
    c->id = (unsigned)InterlockedIncrement(&g_next_id);
    c->last_tick = GetTickCount();
    if (peer) {
        unsigned ip = ntohl(peer->sin_addr.s_addr);
        sprintf(c->peer, "%u.%u.%u.%u:%u", (ip >> 24) & 255, (ip >> 16) & 255, (ip >> 8) & 255, ip & 255,
                ntohs(peer->sin_port));
    } else {
        strcpy(c->peer, "?");
    }
    c->next = g_head;
    if (g_head) g_head->prev = c;
    g_head = c;
    g_nconns++;
    LeaveCriticalSection(&g_list_cs);
    return c;
}

/* Закрыть подключение. Вызывать только когда у сокета нет незавершённых операций. */
static void conn_close(conn *c)
{
    EnterCriticalSection(&g_list_cs);
    if (c->prev) c->prev->next = c->next; else g_head = c->next;
    if (c->next) c->next->prev = c->prev;
    g_nconns--;
    LeaveCriticalSection(&g_list_cs);

    closesocket(c->sock);
    crypto_free_key(c->sess);
    free(c->tx);
    log_line("#%u %s: клиент отключился", c->id, c->peer);
    free(c);
}

/* Запуск асинхронных операций. Возвращают FALSE, если подключение уже закрыто. */
static BOOL post_recv(conn *c)
{
    WSABUF b;
    b.buf = (char *)c->rx + c->rx_len;
    b.len = (ULONG)(sizeof(c->rx) - c->rx_len);
    memset(&c->rx_op.ov, 0, sizeof(OVERLAPPED));
    c->rx_op.type = OP_RECV;
    c->rx_flags = 0;
    if (WSARecv(c->sock, &b, 1, NULL, &c->rx_flags, &c->rx_op.ov, NULL) == SOCKET_ERROR &&
        WSAGetLastError() != WSA_IO_PENDING) {
        conn_close(c);
        return FALSE;
    }
    return TRUE;
}

static BOOL post_send(conn *c)
{
    WSABUF b;
    b.buf = (char *)c->tx + c->tx_sent;
    b.len = c->tx_len - c->tx_sent;
    memset(&c->tx_op.ov, 0, sizeof(OVERLAPPED));
    c->tx_op.type = OP_SEND;
    if (WSASend(c->sock, &b, 1, NULL, 0, &c->tx_op.ov, NULL) == SOCKET_ERROR &&
        WSAGetLastError() != WSA_IO_PENDING) {
        conn_close(c);
        return FALSE;
    }
    return TRUE;
}

/* Поставить кадр на отправку (забирает владение buf, buf - тело без заголовка) */
static BOOL start_send_frame(conn *c, BYTE *body, DWORD len, conn_state next)
{
    DWORD flen;
    BYTE *f = frame_wrap(body, len, &flen);
    free(body);
    if (!f) {
        conn_close(c);
        return FALSE;
    }
    free(c->tx);
    c->tx = f;
    c->tx_len = flen;
    c->tx_sent = 0;
    c->state = next;
    c->rx_len = 0;
    return post_send(c);
}

static void on_recv(conn *c, DWORD n)
{
    if (n == 0) { /* клиент закрыл соединение */
        conn_close(c);
        return;
    }
    c->rx_len += n;
    if (c->rx_len < 4) {
        post_recv(c);
        return;
    }
    DWORD flen = frame_be32(c->rx);
    if (flen == 0 || flen > SERVER_RX_MAX) {
        log_line("#%u %s: некорректная длина кадра (%lu), соединение закрыто", c->id, c->peer,
                 (unsigned long)flen);
        conn_close(c);
        return;
    }
    if (c->rx_len < 4 + flen) { /* кадр пришёл не целиком */
        post_recv(c);
        return;
    }

    const BYTE *body = c->rx + 4;

    if (c->state == ST_WAIT_PUBKEY) {
        BYTE *blob = NULL;
        DWORD blen = 0;
        if (!crypto_create_session(body, flen, &c->sess, &blob, &blen)) {
            log_line("#%u %s: ошибка обмена ключами, соединение закрыто", c->id, c->peer);
            conn_close(c);
            return;
        }
        log_line("#%u %s: получен публичный ключ клиента, сеансовый ключ отправлен", c->id, c->peer);
        start_send_frame(c, blob, blen, ST_SEND_KEY);

    } else if (c->state == ST_WAIT_REQUEST) {
        DWORD plen = 0;
        BYTE *plain = crypto_decrypt(c->sess, body, flen, &plen);
        if (!plain) {
            log_line("#%u %s: не удалось расшифровать запрос, соединение закрыто", c->id, c->peer);
            conn_close(c);
            return;
        }
        char summary[1024];
        request_summary((const char *)plain, summary, sizeof(summary));
        log_line("#%u %s: запрос: %s", c->id, c->peer, summary);

        sbuf out;
        sb_init(&out);
        int ok = process_request((const char *)plain, &out);
        free(plain);
        log_line("#%u %s: запрос обработан (%s), ответ %lu байт", c->id, c->peer, ok ? "OK" : "ошибка",
                 (unsigned long)out.len);

        DWORD elen = 0;
        BYTE *enc = crypto_encrypt(c->sess, (const BYTE *)out.data, (DWORD)out.len, &elen);
        sb_free(&out);
        if (!enc) {
            conn_close(c);
            return;
        }
        start_send_frame(c, enc, elen, ST_SEND_RESPONSE);

    } else {
        conn_close(c);
    }
}

static void on_send(conn *c, DWORD n)
{
    c->tx_sent += n;
    if (n == 0) {
        conn_close(c);
        return;
    }
    if (c->tx_sent < c->tx_len) { /* отправлено не всё - досылаем остаток */
        post_send(c);
        return;
    }
    free(c->tx);
    c->tx = NULL;
    if (c->state == ST_SEND_KEY) {
        c->state = ST_WAIT_REQUEST;
        c->rx_len = 0;
        post_recv(c);
    } else {
        conn_close(c); /* ответ отправлен полностью */
    }
}

/* ------------------------------------------------------------------ */
/* Приём подключений                                                   */
/* ------------------------------------------------------------------ */
static void post_accept(accept_ctx *a)
{
    a->sock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (a->sock == INVALID_SOCKET) {
        log_line("WSASocket: ошибка %d", WSAGetLastError());
        free(a);
        return;
    }
    memset(&a->op.ov, 0, sizeof(OVERLAPPED));
    a->op.type = OP_ACCEPT;
    a->bytes = 0;
    if (!AcceptEx(g_listen, a->sock, a->addrs, 0, sizeof(struct sockaddr_in) + 16,
                  sizeof(struct sockaddr_in) + 16, &a->bytes, &a->op.ov) &&
        WSAGetLastError() != WSA_IO_PENDING) {
        log_line("AcceptEx: ошибка %d", WSAGetLastError());
        closesocket(a->sock);
        free(a);
    }
}

static void on_accept(accept_ctx *a, BOOL ok)
{
    SOCKET s = a->sock;
    if (ok) {
        struct sockaddr *local = NULL, *remote = NULL;
        int ll = 0, rl = 0;
        setsockopt(s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)&g_listen, sizeof(g_listen));
        GetAcceptExSockaddrs(a->addrs, 0, sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16,
                             &local, &ll, &remote, &rl);
        conn *c = conn_new(s, (const struct sockaddr_in *)remote);
        if (!c) {
            log_line("достигнут лимит подключений, клиент отклонён");
            closesocket(s);
        } else if (CreateIoCompletionPort((HANDLE)s, g_port, (ULONG_PTR)c, 0) == NULL) {
            log_line("CreateIoCompletionPort: ошибка %lu", (unsigned long)GetLastError());
            conn_close(c);
        } else {
            log_line("#%u %s: клиент подключился", c->id, c->peer);
            c->state = ST_WAIT_PUBKEY;
            post_recv(c);
        }
    } else {
        closesocket(s);
    }
    if (g_stop) {
        free(a);
    } else {
        post_accept(a); /* сразу готовимся к следующему клиенту */
    }
}

/* ------------------------------------------------------------------ */
/* Рабочий поток                                                       */
/* ------------------------------------------------------------------ */
static unsigned __stdcall worker(void *arg)
{
    (void)arg;
    for (;;) {
        DWORD n = 0;
        ULONG_PTR key = 0;
        OVERLAPPED *ov = NULL;
        BOOL ok = GetQueuedCompletionStatus(g_port, &n, &key, &ov, INFINITE);
        DWORD err = ok ? 0 : GetLastError();

        if (!ov) {
            if (key == KEY_STOP || !ok) break;
            continue;
        }
        io_op *op = CONTAINING_RECORD(ov, io_op, ov);
        if (op->type == OP_ACCEPT) {
            on_accept(CONTAINING_RECORD(op, accept_ctx, op), ok);
            continue;
        }

        conn *c = (conn *)key;
        c->last_tick = GetTickCount();
        if (!ok) { /* сброс соединения, отмена по таймауту и т.п. */
            if (err != ERROR_OPERATION_ABORTED && err != ERROR_NETNAME_DELETED)
                log_line("#%u %s: ошибка ввода-вывода %lu", c->id, c->peer, (unsigned long)err);
            conn_close(c);
        } else if (op->type == OP_RECV) {
            on_recv(c, n);
        } else {
            on_send(c, n);
        }
    }
    return 0;
}

/* Закрытие бездействующих подключений */
static void watchdog(void)
{
    DWORD now = GetTickCount();
    EnterCriticalSection(&g_list_cs);
    for (conn *c = g_head; c; c = c->next) {
        if (now - c->last_tick > IDLE_TIMEOUT_MS) {
            log_line("#%u %s: таймаут бездействия", c->id, c->peer);
            c->last_tick = now; /* не спамить в журнал */
            CancelIoEx((HANDLE)c->sock, NULL); /* незавершённая операция вернётся с ошибкой */
        }
    }
    LeaveCriticalSection(&g_list_cs);
}

static BOOL WINAPI ctrl_handler(DWORD type)
{
    (void)type;
    InterlockedExchange(&g_stop, 1);
    return TRUE;
}

int main(int argc, char **argv)
{
    WSADATA wsa;
    unsigned short port = PROTO_PORT;
    HANDLE threads[64];
    int nthreads;

    con_init();
    if (argc > 1) port = (unsigned short)atoi(argv[1]);

    SetErrorMode(SEM_FAILCRITICALERRORS); /* без окон "вставьте диск" при опросе дисков */
    InitializeCriticalSection(&g_list_cs);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        log_line("WSAStartup: ошибка");
        return 1;
    }
    if (!crypto_init()) return 1;

    g_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (!g_port) {
        log_line("CreateIoCompletionPort: ошибка %lu", (unsigned long)GetLastError());
        return 1;
    }

    g_listen = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (g_listen == INVALID_SOCKET || bind(g_listen, (struct sockaddr *)&addr, sizeof(addr)) != 0 ||
        listen(g_listen, SOMAXCONN) != 0) {
        log_line("не удалось занять порт %u (ошибка %d)", port, WSAGetLastError());
        return 1;
    }
    if (!CreateIoCompletionPort((HANDLE)g_listen, g_port, KEY_LISTEN, 0)) {
        log_line("CreateIoCompletionPort(listen): ошибка %lu", (unsigned long)GetLastError());
        return 1;
    }

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    nthreads = (int)si.dwNumberOfProcessors * 2;
    if (nthreads < 2) nthreads = 2;
    if (nthreads > 64) nthreads = 64;
    for (int i = 0; i < nthreads; i++)
        threads[i] = (HANDLE)_beginthreadex(NULL, 0, worker, NULL, 0, NULL);

    for (int i = 0; i < ACCEPT_POSTED; i++) {
        accept_ctx *a = (accept_ctx *)calloc(1, sizeof(accept_ctx));
        if (a) post_accept(a);
    }

    SetConsoleCtrlHandler(ctrl_handler, TRUE);
    log_line("Сервер запущен, порт %u, рабочих потоков: %d (Ctrl+C - выход)", port, nthreads);

    while (!g_stop) {
        Sleep(1000);
        watchdog();
    }

    log_line("Остановка сервера...");
    closesocket(g_listen); /* незавершённые AcceptEx вернутся с ошибкой */
    for (int i = 0; i < nthreads; i++) PostQueuedCompletionStatus(g_port, 0, KEY_STOP, NULL);
    WaitForMultipleObjects((DWORD)nthreads, threads, TRUE, 5000);

    crypto_cleanup();
    WSACleanup();
    return 0;
}
