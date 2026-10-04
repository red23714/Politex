/*
 * client.c - клиентская часть: консольное меню, запрос к агенту, форматированный вывод.
 *
 * На каждый запрос открывается новое подключение (сервер stateless):
 *   1. клиент -> сервер: публичный RSA-ключ клиента (пара генерируется один раз при запуске)
 *   2. сервер -> клиент: сеансовый AES-ключ, зашифрованный этим публичным ключом
 *   3. клиент -> сервер: зашифрованный запрос
 *   4. сервер -> клиент: зашифрованный ответ
 */
#include "common.h"
#include <ws2tcpip.h>

static HCRYPTKEY g_pair;
static BYTE     *g_pub;
static DWORD     g_publen;
static char      g_host[256] = "127.0.0.1";
static unsigned short g_port = PROTO_PORT;
static int       g_raw;

/* ------------------------------------------------------------------ */
/* Сеть                                                                */
/* ------------------------------------------------------------------ */
static SOCKET connect_to(const char *host, unsigned short port)
{
    struct addrinfo hints, *res = NULL, *p;
    char portstr[16];
    SOCKET s = INVALID_SOCKET;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    sprintf(portstr, "%u", port);
    if (getaddrinfo(host, portstr, &hints, &res) != 0) {
        con_print("Не удалось определить адрес \"%s\"\n", host);
        return INVALID_SOCKET;
    }
    for (p = res; p; p = p->ai_next) {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s == INVALID_SOCKET) continue;
        if (connect(s, p->ai_addr, (int)p->ai_addrlen) == 0) break;
        closesocket(s);
        s = INVALID_SOCKET;
    }
    freeaddrinfo(res);
    if (s == INVALID_SOCKET) {
        con_print("Не удалось подключиться к %s:%u (ошибка %d)\n", host, port, WSAGetLastError());
        return INVALID_SOCKET;
    }
    DWORD tmo = 30000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tmo, sizeof(tmo));
    return s;
}

/* Выполнить запрос. При успехе возвращает расшифрованный текст ответа (free), иначе NULL. */
static char *do_request(const char *req)
{
    SOCKET s = connect_to(g_host, g_port);
    if (s == INVALID_SOCKET) return NULL;

    BYTE *keyblob = NULL, *enc = NULL, *resp = NULL, *plain = NULL;
    HCRYPTKEY sess = 0;
    DWORD kl = 0, el = 0, rl = 0, pl = 0;
    char *result = NULL;

    if (!net_send_frame(s, g_pub, g_publen)) {
        con_print("Ошибка передачи публичного ключа\n");
        goto done;
    }
    keyblob = net_recv_frame(s, 4096, &kl);
    if (!keyblob) {
        con_print("Не получен сеансовый ключ от сервера\n");
        goto done;
    }
    if (!crypto_import_session_key(g_pair, keyblob, kl, &sess)) goto done;

    enc = crypto_encrypt(sess, (const BYTE *)req, (DWORD)strlen(req), &el);
    if (!enc || !net_send_frame(s, enc, el)) {
        con_print("Ошибка отправки запроса\n");
        goto done;
    }
    resp = net_recv_frame(s, CLIENT_RX_MAX, &rl);
    if (!resp) {
        con_print("Ответ не получен (сервер закрыл соединение или истёк таймаут)\n");
        goto done;
    }
    plain = crypto_decrypt(sess, resp, rl, &pl);
    if (!plain) {
        con_print("Не удалось расшифровать ответ\n");
        goto done;
    }
    result = (char *)plain;
    plain = NULL;
done:
    free(keyblob);
    free(enc);
    free(resp);
    free(plain);
    crypto_free_key(sess);
    closesocket(s);
    return result;
}

/* ------------------------------------------------------------------ */
/* Форматирование                                                      */
/* ------------------------------------------------------------------ */
static const char *get(const record *r, const char *k)
{
    const char *v = rec_get(r, k);
    return v ? v : "";
}

static unsigned __int64 getu(const record *r, const char *k)
{
    return _strtoui64(get(r, k), NULL, 0);
}

static void fmt_bytes(unsigned __int64 b, char *out, size_t sz)
{
    static const char *u[] = { "Б", "КБ", "МБ", "ГБ", "ТБ", "ПБ" };
    double v = (double)b;
    int i = 0;
    while (v >= 1024.0 && i < 5) {
        v /= 1024.0;
        i++;
    }
    _snprintf(out, sz, "%I64u байт (%.2f %s)", b, v, u[i]);
    out[sz - 1] = 0;
}

static const char *dow_name(unsigned d)
{
    static const char *n[] = { "воскресенье", "понедельник", "вторник", "среда",
                               "четверг", "пятница", "суббота" };
    return d < 7 ? n[d] : "?";
}

static void show_os(record *recs, int n)
{
    for (int i = 0; i < n; i++) {
        record *r = &recs[i];
        if (strcmp(r->tag, "OS") != 0) continue;
        const char *pt = get(r, "product_type");
        const char *ptr = !strcmp(pt, "workstation") ? "рабочая станция"
                        : !strcmp(pt, "domain_controller") ? "контроллер домена" : "сервер";
        con_print("Операционная система\n");
        con_print("  Имя компьютера : %s\n", get(r, "computer_name"));
        con_print("  ОС             : %s\n", get(r, "name"));
        con_print("  Product name   : %s\n", get(r, "product_name"));
        if (*get(r, "display_version")) con_print("  Версия выпуска : %s\n", get(r, "display_version"));
        con_print("  Версия         : %s.%s, сборка %s\n", get(r, "major"), get(r, "minor"), get(r, "build"));
        con_print("  Service Pack   : %s (%s.%s)\n", *get(r, "service_pack") ? get(r, "service_pack") : "нет",
                  get(r, "sp_major"), get(r, "sp_minor"));
        con_print("  Тип            : %s\n", ptr);
        con_print("  Архитектура    : %s\n", get(r, "arch"));
    }
}

static void show_time(record *recs, int n)
{
    for (int i = 0; i < n; i++) {
        record *r = &recs[i];
        if (strcmp(r->tag, "TIME") != 0) continue;
        int local = !strcmp(get(r, "scope"), "local");
        con_print("%s\n", local ? "Местное время сервера" : "Время UTC");
        con_print("  Дата и время : %04u-%02u-%02u %02u:%02u:%02u.%03u, %s\n", (unsigned)getu(r, "year"),
                  (unsigned)getu(r, "month"), (unsigned)getu(r, "day"), (unsigned)getu(r, "hour"),
                  (unsigned)getu(r, "minute"), (unsigned)getu(r, "second"),
                  (unsigned)getu(r, "millisecond"), dow_name((unsigned)getu(r, "day_of_week")));
        con_print("  Год %s, месяц %s, день %s, часов %s, минут %s, секунд %s\n", get(r, "year"),
                  get(r, "month"), get(r, "day"), get(r, "hour"), get(r, "minute"), get(r, "second"));
        if (local) {
            long off = atol(get(r, "utc_offset_minutes"));
            con_print("  Часовой пояс : UTC%c%02ld:%02ld (%s)\n", off < 0 ? '-' : '+', labs(off) / 60,
                      labs(off) % 60, get(r, "tz_name"));
        } else {
            con_print("  Unix-время   : %s с\n", get(r, "unix"));
        }
        con_print("  ISO 8601     : %s\n", get(r, "iso"));
    }
}

static void show_uptime(record *recs, int n)
{
    for (int i = 0; i < n; i++) {
        record *r = &recs[i];
        if (strcmp(r->tag, "UPTIME") != 0) continue;
        unsigned __int64 ms = getu(r, "milliseconds");
        unsigned __int64 s = ms / 1000;
        con_print("Время работы с момента запуска ОС\n");
        con_print("  %I64u дн. %02u ч. %02u мин. %02u с. %03u мс\n", s / 86400, (unsigned)(s / 3600 % 24),
                  (unsigned)(s / 60 % 60), (unsigned)(s % 60), (unsigned)(ms % 1000));
        con_print("  Всего: %I64u с = %.2f мин = %.2f ч = %.3f дн.\n", s, (double)ms / 60000.0,
                  (double)ms / 3600000.0, (double)ms / 86400000.0);
        con_print("  ОС запущена (UTC): %s\n", get(r, "boot_time_utc"));
    }
}

static void show_memory(record *recs, int n)
{
    static const struct { const char *key, *title; } rows[] = {
        { "total_phys", "Физическая память, всего" },   { "avail_phys", "Физическая память, свободно" },
        { "total_pagefile", "Файл подкачки+ОЗУ, всего" }, { "avail_pagefile", "Файл подкачки+ОЗУ, свободно" },
        { "total_virtual", "Виртуальная память, всего" }, { "avail_virtual", "Виртуальная память, свободно" },
    };
    for (int i = 0; i < n; i++) {
        record *r = &recs[i];
        if (strcmp(r->tag, "MEMORY") != 0) continue;
        con_print("Использование памяти\n");
        con_print("  Загрузка памяти: %s %%\n", get(r, "load_percent"));
        for (int k = 0; k < 6; k++) {
            char b[96];
            fmt_bytes(getu(r, rows[k].key), b, sizeof(b));
            con_print("  %-30s: %s\n", rows[k].title, b);
        }
    }
}

static void show_drives(record *recs, int n)
{
    con_print("Подключённые диски\n");
    con_print("  %-6s %-12s %-10s %-8s %s\n", "Диск", "Тип", "Класс", "ФС", "Метка");
    for (int i = 0; i < n; i++) {
        record *r = &recs[i];
        if (strcmp(r->tag, "DRIVE") != 0) continue;
        const char *cls = get(r, "class");
        const char *clsr = !strcmp(cls, "local") ? "локальный" : !strcmp(cls, "network") ? "сетевой"
                         : !strcmp(cls, "removable") ? "съёмный" : "неизвестно";
        const char *fs = get(r, "fs");
        con_print("  %-6s %-12s %-10s %-8s %s%s\n", get(r, "root"), get(r, "type"), clsr,
                  *fs ? fs : "-", get(r, "label"), !strcmp(get(r, "ready"), "0") ? " (нет данных/не готов)" : "");
    }
}

static void show_freespace(record *recs, int n)
{
    con_print("Свободное место на локальных дисках\n");
    for (int i = 0; i < n; i++) {
        record *r = &recs[i];
        if (strcmp(r->tag, "FREESPACE") != 0) continue;
        if (*get(r, "error")) {
            con_print("  %s: ошибка %s\n", get(r, "root"), get(r, "error"));
            continue;
        }
        char t[96], f[96];
        unsigned __int64 tot = getu(r, "total_bytes"), fr = getu(r, "free_bytes");
        fmt_bytes(tot, t, sizeof(t));
        fmt_bytes(fr, f, sizeof(f));
        con_print("  %s\n    всего   : %s\n    свободно: %s (%.1f %%)\n", get(r, "root"), t, f,
                  tot ? 100.0 * (double)fr / (double)tot : 0.0);
    }
}

static const char *scope_text(const char *code, int reg)
{
    if (!strcmp(code, "this_object_only"))
        return reg ? "только этот раздел" : "только этот объект";
    if (!strcmp(code, "this_folder_subfolders_and_files"))
        return reg ? "этот раздел и подразделы" : "эта папка, вложенные папки и файлы";
    if (!strcmp(code, "this_folder_and_subfolders"))
        return reg ? "этот раздел и подразделы" : "эта папка и вложенные папки";
    if (!strcmp(code, "this_folder_and_files"))
        return "эта папка и файлы";
    if (!strcmp(code, "subfolders_and_files_only"))
        return reg ? "только подразделы" : "только вложенные папки и файлы";
    if (!strcmp(code, "subfolders_only"))
        return reg ? "только подразделы" : "только вложенные папки";
    if (!strcmp(code, "files_only"))
        return "только файлы";
    return code;
}

static void show_ace(const record *r, int reg)
{
    const char *type = get(r, "ace_type");
    const char *trn = !strncmp(type, "ACCESS_ALLOWED", 14) ? "разрешающий"
                    : !strncmp(type, "ACCESS_DENIED", 13) ? "запрещающий" : "прочий";
    const char *use = get(r, "name_use");
    con_print("  ACE #%u\n", (unsigned)getu(r, "index") + 1);
    con_print("    SID субъекта : %s\n", get(r, "sid"));
    if (*get(r, "name"))
        con_print("    Субъект      : %s (%s)\n", get(r, "name"), use);
    else
        con_print("    Субъект      : <имя не определено> (%s)\n", use);
    con_print("    Тип ACE      : %s (%s), код %s\n", type, trn, get(r, "ace_type_id"));
    con_print("    Флаги ACE    : %s%s%s\n", get(r, "ace_flags"), *get(r, "ace_flag_names") ? " = " : "",
              get(r, "ace_flag_names"));
    con_print("    Область      : %s%s%s\n", scope_text(get(r, "scope"), reg),
              strstr(get(r, "ace_flag_names"), "NO_PROPAGATE") ? ", не дальше непосредственных потомков" : "",
              strstr(get(r, "ace_flag_names"), "INHERITED_ACE") ? " [унаследован]" : " [задан явно]");
    con_print("    Маска доступа: %s\n", get(r, "mask"));

    char *bits = _strdup(get(r, "bits")), *names = _strdup(get(r, "bit_names"));
    char *bs = bits, *ns = names;
    con_print("    Биты маски (номер - название):\n");
    while (bs && *bs) {
        char *bn = strchr(bs, ','), *nn = strchr(ns, ',');
        if (bn) *bn++ = 0;
        if (nn) *nn++ = 0;
        con_print("      %2s  %s\n", bs, ns);
        bs = bn;
        ns = nn ? nn : (char *)"";
    }
    free(bits);
    free(names);
}

static void show_acl(record *recs, int n)
{
    int reg = 0;
    for (int i = 0; i < n; i++) {
        record *r = &recs[i];
        if (!strcmp(r->tag, "ACL")) {
            const char *ot = get(r, "object_type");
            reg = !strcmp(ot, "registry");
            con_print("Права доступа (DACL)\n");
            con_print("  Объект : %s (%s)\n", get(r, "path"),
                      reg ? "ключ реестра" : !strcmp(ot, "directory") ? "папка" : "файл");
            if (!strcmp(get(r, "null_dacl"), "1"))
                con_print("  DACL   : NULL - доступ разрешён всем без ограничений\n");
            else
                con_print("  Записей ACE: %s, наследование от родителя: %s\n", get(r, "ace_count"),
                          !strcmp(get(r, "dacl_protected"), "1") ? "отключено" : "включено");
        } else if (!strcmp(r->tag, "ACE")) {
            show_ace(r, reg);
        }
    }
}

static void show_owner(record *recs, int n)
{
    for (int i = 0; i < n; i++) {
        record *r = &recs[i];
        if (strcmp(r->tag, "OWNER") != 0) continue;
        const char *ot = get(r, "object_type");
        con_print("Владелец объекта\n");
        con_print("  Объект : %s (%s)\n", get(r, "path"),
                  !strcmp(ot, "registry") ? "ключ реестра" : !strcmp(ot, "directory") ? "папка" : "файл");
        con_print("  SID    : %s\n", get(r, "sid"));
        con_print("  Имя    : %s (%s)\n", *get(r, "name") ? get(r, "name") : "<имя не определено>",
                  get(r, "name_use"));
    }
}

/* ------------------------------------------------------------------ */
/* Выполнение команды и вывод                                          */
/* ------------------------------------------------------------------ */
static void run_command(const char *cmd, const char *a1, const char *a2)
{
    sbuf req;
    sb_init(&req);
    sb_append_esc(&req, cmd);
    if (a1) {
        sb_append(&req, "\t");
        sb_append_esc(&req, a1);
    }
    if (a2) {
        sb_append(&req, "\t");
        sb_append_esc(&req, a2);
    }

    char *resp = do_request(req.data);
    sb_free(&req);
    if (!resp) return;

    if (g_raw) con_print("--- сырой ответ ---\n%s--- конец ---\n", resp);

    /* разбор строк ответа */
    int cap = 64, n = 0;
    record *recs = (record *)malloc(sizeof(record) * (size_t)cap);
    char *p = resp;
    while (*p) {
        char *eol = strchr(p, '\n');
        if (eol) *eol = 0;
        if (*p) {
            if (n == cap) {
                cap *= 2;
                recs = (record *)realloc(recs, sizeof(record) * (size_t)cap);
            }
            rec_parse(p, &recs[n++]);
        }
        if (!eol) break;
        p = eol + 1;
    }

    if (n == 0 || strcmp(recs[0].tag, "RESULT") != 0) {
        con_print("Некорректный ответ сервера\n");
    } else if (strcmp(get(&recs[0], "status"), "OK") != 0) {
        con_print("Сервер вернул ошибку %s (%s): %s\n", get(&recs[0], "code"), get(&recs[0], "source"),
                  get(&recs[0], "message"));
    } else if (!strcmp(cmd, "GET_OS"))        show_os(recs, n);
    else if (!strcmp(cmd, "GET_TIME"))        show_time(recs, n);
    else if (!strcmp(cmd, "GET_UPTIME"))      show_uptime(recs, n);
    else if (!strcmp(cmd, "GET_MEMORY"))      show_memory(recs, n);
    else if (!strcmp(cmd, "GET_DRIVES"))      show_drives(recs, n);
    else if (!strcmp(cmd, "GET_FREESPACE"))   show_freespace(recs, n);
    else if (!strcmp(cmd, "GET_ACL"))         show_acl(recs, n);
    else if (!strcmp(cmd, "GET_OWNER"))       show_owner(recs, n);

    free(recs);
    free(resp);
}

static void ask_server(void)
{
    char line[300];
    con_print("Адрес сервера (IP или имя, можно host:порт) [%s]: ", g_host);
    if (!con_readline(line, sizeof(line)) || !line[0]) return;
    char *colon = strrchr(line, ':');
    if (colon) {
        *colon++ = 0;
        int p = atoi(colon);
        if (p > 0 && p < 65536) g_port = (unsigned short)p;
    }
    if (line[0]) {
        strncpy(g_host, line, sizeof(g_host) - 1);
        g_host[sizeof(g_host) - 1] = 0;
    }
}

static void ask_object(const char *cmd)
{
    char kind[16], path[1024];
    con_print("Тип объекта: 1 - файл/папка, 2 - ключ реестра [1]: ");
    if (!con_readline(kind, sizeof(kind))) return;
    int reg = (kind[0] == '2');
    if (reg)
        con_print("Путь к ключу (например HKLM\\SOFTWARE\\Microsoft, HKCU\\Software): ");
    else
        con_print("Путь к файлу или папке (например C:\\Windows): ");
    if (!con_readline(path, sizeof(path)) || !path[0]) return;
    run_command(cmd, reg ? "REG" : "FILE", path);
}

int main(void)
{
    WSADATA wsa;
    char line[64];

    con_init();
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        con_print("WSAStartup: ошибка\n");
        return 1;
    }
    if (!crypto_init()) return 1;

    con_print("Генерация пары ключей RSA-2048...\n");
    if (!crypto_gen_keypair(&g_pair)) return 1;
    g_pub = crypto_export_pubkey(g_pair, &g_publen);
    if (!g_pub) return 1;

    ask_server();
    for (;;) {
        con_print("\n=== Сервер: %s:%u%s ===\n", g_host, g_port, g_raw ? "  [сырой вывод]" : "");
        con_print(" 1  Тип и версия ОС\n"
                  " 2  Текущее время\n"
                  " 3  Время с момента запуска ОС\n"
                  " 4  Информация о памяти\n"
                  " 5  Типы подключённых дисков и файловые системы\n"
                  " 6  Свободное место на локальных дисках\n"
                  " 7  Права доступа к файлу/папке/ключу реестра\n"
                  " 8  Владелец файла/папки/ключа реестра\n"
                  " s  Сменить адрес сервера\n"
                  " r  Вкл/выкл вывод сырого ответа\n"
                  " 0  Выход\n"
                  "Выбор: ");
        if (!con_readline(line, sizeof(line))) break;
        con_print("\n");
        switch (line[0]) {
        case '1': run_command("GET_OS", NULL, NULL); break;
        case '2': run_command("GET_TIME", NULL, NULL); break;
        case '3': run_command("GET_UPTIME", NULL, NULL); break;
        case '4': run_command("GET_MEMORY", NULL, NULL); break;
        case '5': run_command("GET_DRIVES", NULL, NULL); break;
        case '6': run_command("GET_FREESPACE", NULL, NULL); break;
        case '7': ask_object("GET_ACL"); break;
        case '8': ask_object("GET_OWNER"); break;
        case 's': case 'S': ask_server(); break;
        case 'r': case 'R': g_raw = !g_raw; break;
        case '0': goto out;
        default: break;
        }
    }
out:
    free(g_pub);
    crypto_free_key(g_pair);
    crypto_cleanup();
    WSACleanup();
    return 0;
}
