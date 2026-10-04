#include "common.h"
#include <wincrypt.h>

/* ================================================================== */
/* sbuf                                                                */
/* ================================================================== */
static void die_oom(void)
{
    fputs("out of memory\n", stderr);
    exit(1);
}

void sb_init(sbuf *b)
{
    b->cap = 1024;
    b->len = 0;
    b->data = (char *)malloc(b->cap);
    if (!b->data) die_oom();
    b->data[0] = 0;
}

void sb_free(sbuf *b)
{
    free(b->data);
    b->data = NULL;
    b->len = b->cap = 0;
}

static void sb_reserve(sbuf *b, size_t add)
{
    if (b->len + add + 1 > b->cap) {
        size_t nc = b->cap * 2;
        while (nc < b->len + add + 1) nc *= 2;
        char *p = (char *)realloc(b->data, nc);
        if (!p) die_oom();
        b->data = p;
        b->cap = nc;
    }
}

void sb_append(sbuf *b, const char *s)
{
    size_t n = strlen(s);
    sb_reserve(b, n);
    memcpy(b->data + b->len, s, n + 1);
    b->len += n;
}

void sb_printf(sbuf *b, const char *fmt, ...)
{
    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);
    int n = _vscprintf(fmt, ap);
    if (n > 0) {
        sb_reserve(b, (size_t)n);
        _vsnprintf(b->data + b->len, (size_t)n + 1, fmt, ap2);
        b->len += (size_t)n;
        b->data[b->len] = 0;
    }
    va_end(ap2);
    va_end(ap);
}

void sb_append_esc(sbuf *b, const char *s)
{
    for (; *s; s++) {
        switch (*s) {
        case '\\': sb_append(b, "\\\\"); break;
        case '\t': sb_append(b, "\\t");  break;
        case '\n': sb_append(b, "\\n");  break;
        case '\r': sb_append(b, "\\r");  break;
        default: {
            char c[2] = { *s, 0 };
            sb_append(b, c);
        }
        }
    }
}

void sb_rec_begin(sbuf *b, const char *tag) { sb_append(b, tag); }
void sb_rec_end(sbuf *b) { sb_append(b, "\n"); }

void sb_kv_str(sbuf *b, const char *key, const char *utf8)
{
    sb_append(b, "\t");
    sb_append(b, key);
    sb_append(b, "=");
    sb_append_esc(b, utf8 ? utf8 : "");
}

void sb_kv_wstr(sbuf *b, const char *key, const wchar_t *w)
{
    char *u = wide_to_utf8(w ? w : L"");
    sb_kv_str(b, key, u);
    free(u);
}

void sb_kv_u64(sbuf *b, const char *key, unsigned __int64 v)
{
    char tmp[32];
    _ui64toa(v, tmp, 10);
    sb_kv_str(b, key, tmp);
}

void sb_kv_i64(sbuf *b, const char *key, __int64 v)
{
    char tmp[32];
    _i64toa(v, tmp, 10);
    sb_kv_str(b, key, tmp);
}

void sb_kv_hex(sbuf *b, const char *key, unsigned long v)
{
    char tmp[32];
    sprintf(tmp, "0x%08lX", v);
    sb_kv_str(b, key, tmp);
}

/* ------------------------------------------------------------------ */
/* Разбор записей                                                      */
/* ------------------------------------------------------------------ */
void unescape_inplace(char *s)
{
    char *w = s;
    for (char *r = s; *r; r++) {
        if (*r == '\\' && r[1]) {
            r++;
            switch (*r) {
            case 't': *w++ = '\t'; break;
            case 'n': *w++ = '\n'; break;
            case 'r': *w++ = '\r'; break;
            default:  *w++ = *r;   break;   /* \\ и любой другой символ */
            }
        } else {
            *w++ = *r;
        }
    }
    *w = 0;
}

int rec_parse(char *line, record *r)
{
    r->n = 0;
    r->tag = line;
    char *p = strchr(line, '\t');
    if (!p) return 1;
    *p++ = 0;
    while (p && *p && r->n < REC_MAX_FIELDS) {
        char *next = strchr(p, '\t');
        if (next) *next++ = 0;
        char *eq = strchr(p, '=');
        if (eq) {
            *eq = 0;
            r->key[r->n] = p;
            r->val[r->n] = eq + 1;
            unescape_inplace(eq + 1);
            r->n++;
        }
        p = next;
    }
    return 1;
}

const char *rec_get(const record *r, const char *key)
{
    for (int i = 0; i < r->n; i++)
        if (strcmp(r->key[i], key) == 0) return r->val[i];
    return NULL;
}

/* ================================================================== */
/* Юникод и консоль                                                    */
/* ================================================================== */
wchar_t *utf8_to_wide(const char *s)
{
    int n = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (n <= 0) return _wcsdup(L"");
    wchar_t *w = (wchar_t *)malloc((size_t)n * sizeof(wchar_t));
    if (!w) die_oom();
    MultiByteToWideChar(CP_UTF8, 0, s, -1, w, n);
    return w;
}

char *wide_to_utf8(const wchar_t *w)
{
    int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, NULL, 0, NULL, NULL);
    if (n <= 0) return _strdup("");
    char *s = (char *)malloc((size_t)n);
    if (!s) die_oom();
    WideCharToMultiByte(CP_UTF8, 0, w, -1, s, n, NULL, NULL);
    return s;
}

static CRITICAL_SECTION g_con_cs;
static HANDLE g_out, g_in;
static BOOL   g_out_console, g_in_console;
static int    g_con_ready;

void con_init(void)
{
    DWORD m;
    if (g_con_ready) return;
    InitializeCriticalSection(&g_con_cs);
    g_out = GetStdHandle(STD_OUTPUT_HANDLE);
    g_in  = GetStdHandle(STD_INPUT_HANDLE);
    g_out_console = GetConsoleMode(g_out, &m);
    g_in_console  = GetConsoleMode(g_in, &m);
    g_con_ready = 1;
}

void con_write(const char *utf8)
{
    if (!g_con_ready) con_init();
    EnterCriticalSection(&g_con_cs);
    if (g_out_console) {
        wchar_t *w = utf8_to_wide(utf8);
        DWORD written;
        WriteConsoleW(g_out, w, (DWORD)wcslen(w), &written, NULL);
        free(w);
    } else {
        fwrite(utf8, 1, strlen(utf8), stdout);
        fflush(stdout);
    }
    LeaveCriticalSection(&g_con_cs);
}

void con_print(const char *fmt, ...)
{
    va_list ap, ap2;
    va_start(ap, fmt);
    va_copy(ap2, ap);
    int n = _vscprintf(fmt, ap);
    if (n > 0) {
        char *buf = (char *)malloc((size_t)n + 1);
        if (!buf) die_oom();
        _vsnprintf(buf, (size_t)n + 1, fmt, ap2);
        buf[n] = 0;
        con_write(buf);
        free(buf);
    }
    va_end(ap2);
    va_end(ap);
}

int con_readline(char *out, int outsz)
{
    if (!g_con_ready) con_init();
    if (g_in_console) {
        wchar_t w[1024];
        DWORD n = 0;
        if (!ReadConsoleW(g_in, w, 1023, &n, NULL) || n == 0) return 0;
        while (n > 0 && (w[n - 1] == L'\r' || w[n - 1] == L'\n')) n--;
        w[n] = 0;
        if (WideCharToMultiByte(CP_UTF8, 0, w, -1, out, outsz, NULL, NULL) == 0)
            out[0] = 0;
        return 1;
    }
    if (!fgets(out, outsz, stdin)) return 0;
    size_t l = strlen(out);
    while (l > 0 && (out[l - 1] == '\n' || out[l - 1] == '\r')) out[--l] = 0;
    return 1;
}

/* ================================================================== */
/* CryptoAPI                                                           */
/* ================================================================== */
static HCRYPTPROV       g_prov;
static CRITICAL_SECTION g_crypto_cs;

static void crypto_err(const char *what)
{
    con_print("CryptoAPI: %s завершилась ошибкой 0x%08lX\n", what, (unsigned long)GetLastError());
}

int crypto_init(void)
{
    InitializeCriticalSection(&g_crypto_cs);
    /* VERIFYCONTEXT: контейнер ключей на диске не нужен, ключи эфемерные */
    if (!CryptAcquireContextW(&g_prov, NULL, MS_ENH_RSA_AES_PROV_W, PROV_RSA_AES,
                              CRYPT_VERIFYCONTEXT)) {
        crypto_err("CryptAcquireContext");
        return 0;
    }
    return 1;
}

void crypto_cleanup(void)
{
    if (g_prov) CryptReleaseContext(g_prov, 0);
    g_prov = 0;
}

void crypto_free_key(HCRYPTKEY k)
{
    if (!k) return;
    EnterCriticalSection(&g_crypto_cs);
    CryptDestroyKey(k);
    LeaveCriticalSection(&g_crypto_cs);
}

/* ---------------- клиент ---------------- */
int crypto_gen_keypair(HCRYPTKEY *pair)
{
    int ok;
    EnterCriticalSection(&g_crypto_cs);
    ok = CryptGenKey(g_prov, AT_KEYEXCHANGE, (2048u << 16) | CRYPT_EXPORTABLE, pair);
    if (!ok) crypto_err("CryptGenKey(RSA)");
    LeaveCriticalSection(&g_crypto_cs);
    return ok;
}

BYTE *crypto_export_pubkey(HCRYPTKEY pair, DWORD *len)
{
    BYTE *blob = NULL;
    DWORD n = 0;
    EnterCriticalSection(&g_crypto_cs);
    if (CryptExportKey(pair, 0, PUBLICKEYBLOB, 0, NULL, &n) &&
        (blob = (BYTE *)malloc(n)) != NULL) {
        if (!CryptExportKey(pair, 0, PUBLICKEYBLOB, 0, blob, &n)) {
            crypto_err("CryptExportKey(PUBLICKEYBLOB)");
            free(blob);
            blob = NULL;
        } else {
            *len = n;
        }
    } else if (!blob) {
        crypto_err("CryptExportKey(PUBLICKEYBLOB)");
    }
    LeaveCriticalSection(&g_crypto_cs);
    return blob;
}

int crypto_import_session_key(HCRYPTKEY pair, const BYTE *blob, DWORD len, HCRYPTKEY *sess)
{
    int ok;
    EnterCriticalSection(&g_crypto_cs);
    /* SIMPLEBLOB расшифровывается приватным ключом пары */
    ok = CryptImportKey(g_prov, blob, len, pair, 0, sess);
    if (!ok) crypto_err("CryptImportKey(SIMPLEBLOB)");
    LeaveCriticalSection(&g_crypto_cs);
    return ok;
}

/* ---------------- сервер ---------------- */
int crypto_create_session(const BYTE *pubblob, DWORD publen,
                          HCRYPTKEY *sess, BYTE **outblob, DWORD *outlen)
{
    HCRYPTKEY pub = 0, key = 0;
    BYTE *blob = NULL;
    DWORD blen = 0;
    int ok = 0;

    EnterCriticalSection(&g_crypto_cs);
    if (!CryptImportKey(g_prov, pubblob, publen, 0, 0, &pub)) {
        crypto_err("CryptImportKey(PUBLICKEYBLOB)");
        goto done;
    }
    /* случайный сеансовый ключ AES-256 (режим CBC, дополнение PKCS#5 - по умолчанию) */
    if (!CryptGenKey(g_prov, CALG_AES_256, CRYPT_EXPORTABLE, &key)) {
        crypto_err("CryptGenKey(AES)");
        goto done;
    }
    /* шифруем сеансовый ключ публичным ключом клиента */
    if (!CryptExportKey(key, pub, SIMPLEBLOB, 0, NULL, &blen)) {
        crypto_err("CryptExportKey(SIMPLEBLOB)");
        goto done;
    }
    blob = (BYTE *)malloc(blen);
    if (!blob) goto done;
    if (!CryptExportKey(key, pub, SIMPLEBLOB, 0, blob, &blen)) {
        crypto_err("CryptExportKey(SIMPLEBLOB)");
        goto done;
    }
    *sess = key;
    *outblob = blob;
    *outlen = blen;
    key = 0;
    blob = NULL;
    ok = 1;
done:
    if (pub) CryptDestroyKey(pub);
    if (key) CryptDestroyKey(key);
    free(blob);
    LeaveCriticalSection(&g_crypto_cs);
    return ok;
}

/* ---------------- симметричное шифрование ---------------- */
BYTE *crypto_encrypt(HCRYPTKEY k, const BYTE *data, DWORD len, DWORD *outlen)
{
    BYTE *out = (BYTE *)malloc(AES_BLOCK + len + AES_BLOCK);
    DWORD n = len;
    int ok = 0;
    if (!out) return NULL;

    EnterCriticalSection(&g_crypto_cs);
    /* для каждого сообщения - новый случайный IV, он передаётся перед шифртекстом */
    if (CryptGenRandom(g_prov, AES_BLOCK, out) &&
        CryptSetKeyParam(k, KP_IV, out, 0)) {
        memcpy(out + AES_BLOCK, data, len);
        ok = CryptEncrypt(k, 0, TRUE, 0, out + AES_BLOCK, &n, len + AES_BLOCK);
    }
    if (!ok) crypto_err("CryptEncrypt");
    LeaveCriticalSection(&g_crypto_cs);

    if (!ok) {
        free(out);
        return NULL;
    }
    *outlen = AES_BLOCK + n;
    return out;
}

BYTE *crypto_decrypt(HCRYPTKEY k, const BYTE *data, DWORD len, DWORD *outlen)
{
    if (len < 2 * AES_BLOCK || (len % AES_BLOCK) != 0) return NULL;
    DWORD n = len - AES_BLOCK;
    BYTE *out = (BYTE *)malloc((size_t)n + 1);
    int ok;
    if (!out) return NULL;
    memcpy(out, data + AES_BLOCK, n);

    EnterCriticalSection(&g_crypto_cs);
    ok = CryptSetKeyParam(k, KP_IV, (BYTE *)data, 0) &&
         CryptDecrypt(k, 0, TRUE, 0, out, &n);
    LeaveCriticalSection(&g_crypto_cs);

    if (!ok) {
        free(out);
        return NULL;
    }
    out[n] = 0;
    *outlen = n;
    return out;
}

/* ================================================================== */
/* Кадры                                                               */
/* ================================================================== */
DWORD frame_be32(const BYTE *p)
{
    return ((DWORD)p[0] << 24) | ((DWORD)p[1] << 16) | ((DWORD)p[2] << 8) | (DWORD)p[3];
}

static void put_be32(BYTE *p, DWORD v)
{
    p[0] = (BYTE)(v >> 24);
    p[1] = (BYTE)(v >> 16);
    p[2] = (BYTE)(v >> 8);
    p[3] = (BYTE)v;
}

BYTE *frame_wrap(const BYTE *body, DWORD len, DWORD *outlen)
{
    BYTE *f = (BYTE *)malloc((size_t)len + 4);
    if (!f) return NULL;
    put_be32(f, len);
    memcpy(f + 4, body, len);
    *outlen = len + 4;
    return f;
}

static int send_all(SOCKET s, const BYTE *p, int n)
{
    while (n > 0) {
        int r = send(s, (const char *)p, n, 0);
        if (r <= 0) return 0;
        p += r;
        n -= r;
    }
    return 1;
}

static int recv_all(SOCKET s, BYTE *p, int n)
{
    while (n > 0) {
        int r = recv(s, (char *)p, n, 0);
        if (r <= 0) return 0;
        p += r;
        n -= r;
    }
    return 1;
}

int net_send_frame(SOCKET s, const BYTE *data, DWORD len)
{
    BYTE hdr[4];
    put_be32(hdr, len);
    return send_all(s, hdr, 4) && send_all(s, data, (int)len);
}

BYTE *net_recv_frame(SOCKET s, DWORD maxlen, DWORD *outlen)
{
    BYTE hdr[4];
    if (!recv_all(s, hdr, 4)) return NULL;
    DWORD len = frame_be32(hdr);
    if (len == 0 || len > maxlen) return NULL;
    BYTE *p = (BYTE *)malloc(len);
    if (!p) return NULL;
    if (!recv_all(s, p, (int)len)) {
        free(p);
        return NULL;
    }
    *outlen = len;
    return p;
}
