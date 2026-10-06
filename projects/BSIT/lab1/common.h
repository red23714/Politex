#ifndef COMMON_H
#define COMMON_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601 /* Windows 7 */
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <winsock2.h>
#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define PROTO_PORT 9000
#define SERVER_RX_MAX 16384 /* макс. размер кадра, принимаемого сервером */
#define CLIENT_RX_MAX                                                          \
	(4 * 1024 * 1024) /* макс. размер кадра, принимаемого клиентом */
#define AES_BLOCK 16

/* ------------------------------------------------------------------ */
/* Строковый буфер и записи протокола                                  */
/* ------------------------------------------------------------------ */
typedef struct
{
	char* data;
	size_t len;
	size_t cap;
} sbuf;

void sb_init(sbuf* b);
void sb_free(sbuf* b);
void sb_append(sbuf* b, const char* s);
void sb_printf(sbuf* b, const char* fmt, ...);
void sb_append_esc(sbuf* b, const char* s); /* добавить с экранированием */

/* запись: TAG \t key=value \t key=value ... \n */
void sb_rec_begin(sbuf* b, const char* tag);
void sb_kv_str(sbuf* b, const char* key, const char* utf8);
void sb_kv_wstr(sbuf* b, const char* key, const wchar_t* w);
void sb_kv_u64(sbuf* b, const char* key, unsigned __int64 v);
void sb_kv_i64(sbuf* b, const char* key, __int64 v);
void sb_kv_hex(sbuf* b, const char* key, unsigned long v);
void sb_rec_end(sbuf* b);

/* разбор записи (на стороне клиента и при разборе запроса на сервере) */
#define REC_MAX_FIELDS 40
typedef struct
{
	char* tag;
	int n;
	char* key[REC_MAX_FIELDS];
	char* val[REC_MAX_FIELDS];
} record;

void unescape_inplace(char* s);
int rec_parse(char* line, record* r); /* line портится (разрезается) */
const char* rec_get(const record* r, const char* key);

/* ------------------------------------------------------------------ */
/* Юникод и консоль                                                    */
/* ------------------------------------------------------------------ */
wchar_t* utf8_to_wide(const char* s); /* free() после использования */
char* wide_to_utf8(const wchar_t* w);

void con_init(void);
void con_write(const char* utf8);
void con_print(const char* fmt, ...);	/* printf, но строки в UTF-8 */
int con_readline(char* out, int outsz); /* 0 - конец ввода */

/* ------------------------------------------------------------------ */
/* CryptoAPI                                                           */
/* ------------------------------------------------------------------ */
int crypto_init(void);
void crypto_cleanup(void);
void crypto_free_key(HCRYPTKEY k);

/* клиентская часть */
int crypto_gen_keypair(HCRYPTKEY* pair);
BYTE* crypto_export_pubkey(HCRYPTKEY pair, DWORD* len);
int crypto_import_session_key(HCRYPTKEY pair, const BYTE* blob, DWORD len,
							  HCRYPTKEY* sess);

/* серверная часть: импорт публичного ключа клиента, генерация и экспорт
 * сеансового ключа */
int crypto_create_session(const BYTE* pubblob, DWORD publen, HCRYPTKEY* sess,
						  BYTE** outblob, DWORD* outlen);

/* обе стороны. Результат encrypt: IV(16) || шифртекст. decrypt возвращает буфер
 * с завершающим 0 */
BYTE* crypto_encrypt(HCRYPTKEY k, const BYTE* data, DWORD len, DWORD* outlen);
BYTE* crypto_decrypt(HCRYPTKEY k, const BYTE* data, DWORD len, DWORD* outlen);

/* ------------------------------------------------------------------ */
/* Кадры: 4 байта длины (big endian) + тело                            */
/* ------------------------------------------------------------------ */
BYTE* frame_wrap(const BYTE* body, DWORD len,
				 DWORD* outlen); /* для асинхронной отправки */
DWORD frame_be32(const BYTE* p);

/* блокирующие варианты (клиент) */
int net_send_frame(SOCKET s, const BYTE* data, DWORD len);
BYTE* net_recv_frame(SOCKET s, DWORD maxlen, DWORD* outlen);

/* ------------------------------------------------------------------ */
/* sysinfo.c                                                           */
/* ------------------------------------------------------------------ */
/* Разобрать запрос и сформировать ответ. Возвращает 1 при успехе, 0 при ошибке.
 */
int process_request(const char* req, sbuf* resp);
/* Короткое читаемое описание запроса для журнала сервера */
void request_summary(const char* req, char* out, size_t outsz);

#endif
