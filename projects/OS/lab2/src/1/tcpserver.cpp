#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#define poll WSAPoll
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 8192

#ifdef _WIN32
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

typedef struct
{
	socket_t sock;
	int active;
	int ready;
	unsigned char* buf; // динамический буфер
	int len;
	int capacity;
	struct sockaddr_in addr;
} client_t;

void close_socket(socket_t s)
{
#ifdef _WIN32
	closesocket(s);
#else
	close(s);
#endif
}

void set_nonblocking(socket_t s)
{
#ifdef _WIN32
	u_long mode = 1;
	ioctlsocket(s, FIONBIO, &mode);
#else
	fcntl(s, F_SETFL, O_NONBLOCK);
#endif
}

uint32_t read_u32(unsigned char* p)
{
	uint32_t v;
	memcpy(&v, p, 4);
	return ntohl(v);
}

void write_msg(FILE* f, struct sockaddr_in* addr, uint32_t date, uint32_t t1,
			   uint32_t t2, char* msg)
{
	int yyyy = date / 10000;
	int mm = (date / 100) % 100;
	int dd = date % 100;

	int h1 = t1 / 10000;
	int m1 = (t1 / 100) % 100;
	int s1 = t1 % 100;

	int h2 = t2 / 10000;
	int m2 = (t2 / 100) % 100;
	int s2 = t2 % 100;

	fprintf(f, "%s:%d %02d.%02d.%04d %02d:%02d:%02d %02d:%02d:%02d %s\n",
			inet_ntoa(addr->sin_addr), ntohs(addr->sin_port), dd, mm, yyyy, h1,
			m1, s1, h2, m2, s2, msg);

	fflush(f);
}

int main(int argc, char* argv[])
{

#ifdef _WIN32
	WSADATA w;
	WSAStartup(MAKEWORD(2, 2), &w);
#endif

	if (argc < 2)
	{
		printf("usage: tcpserver <port>\n");
		return 1;
	}

	int port = atoi(argv[1]);

	FILE* f = fopen("msg.txt", "a");
	if (!f)
	{
		printf("cannot open msg.txt\n");
		return 1;
	}

	socket_t ls = socket(AF_INET, SOCK_STREAM, 0);

	if (ls < 0)
	{
		printf("socket error\n");
		return 1;
	}

	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(ls, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		printf("bind error\n");
		return 1;
	}

	if (listen(ls, 10) < 0)
	{
		printf("listen error\n");
		return 1;
	}

	set_nonblocking(ls);

	client_t clients[MAX_CLIENTS];
	struct pollfd pfd[MAX_CLIENTS + 1];

	int i;

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		clients[i].active = 0;
		pfd[i].fd = -1;
	}

	pfd[MAX_CLIENTS].fd = ls;
	pfd[MAX_CLIENTS].events = POLLIN;

	int stop = 0;

	while (!stop)
	{

		int n = poll(pfd, MAX_CLIENTS + 1, 1000);

		if (n <= 0)
			continue;

		if (pfd[MAX_CLIENTS].revents & POLLIN)
		{
			struct sockaddr_in caddr;
#ifdef _WIN32
			int len = sizeof(caddr);
#else
			socklen_t len = sizeof(caddr);
#endif

			socket_t cs = accept(ls, (struct sockaddr*)&caddr, &len);

			if (cs >= 0)
			{
				set_nonblocking(cs);

				for (i = 0; i < MAX_CLIENTS; i++)
					if (!clients[i].active)
					{
						clients[i].active = 1;
						clients[i].sock = cs;
						clients[i].ready = 0;
						clients[i].len = 0;
						clients[i].capacity = BUFFER_SIZE;
						clients[i].buf =
							(unsigned char*)malloc(clients[i].capacity);
						clients[i].addr = caddr;

						pfd[i].fd = cs;
						pfd[i].events = POLLIN;
						break;
					}

				if (i == MAX_CLIENTS)
					close_socket(cs);
			}
		}

		for (i = 0; i < MAX_CLIENTS; i++)
		{

			if (!clients[i].active)
				continue;

			if (pfd[i].revents & (POLLERR | POLLHUP))
			{
				free(clients[i].buf);
				close_socket(clients[i].sock);
				clients[i].active = 0;
				pfd[i].fd = -1;
				continue;
			}

			if (pfd[i].revents & POLLIN)
			{

				// расширяем буфер если заполнен
				if (clients[i].len == clients[i].capacity)
				{
					clients[i].capacity *= 2;
					clients[i].buf = (unsigned char*)realloc(
						clients[i].buf, clients[i].capacity);
				}

				int r = recv(clients[i].sock,
							 (char*)clients[i].buf + clients[i].len,
							 clients[i].capacity - clients[i].len, 0);

				if (r <= 0)
				{
					free(clients[i].buf);
					close_socket(clients[i].sock);
					clients[i].active = 0;
					pfd[i].fd = -1;
					continue;
				}

				clients[i].len += r;

				if (!clients[i].ready)
				{
					if (clients[i].len >= 3)
					{
						if (memcmp(clients[i].buf, "put", 3) != 0)
						{
							free(clients[i].buf);
							close_socket(clients[i].sock);
							clients[i].active = 0;
							pfd[i].fd = -1;
							continue;
						}

						memmove(clients[i].buf, clients[i].buf + 3,
								clients[i].len - 3);
						clients[i].len -= 3;
						clients[i].ready = 1;
					}
				}

				while (clients[i].ready && clients[i].len >= 20)
				{

					unsigned char* p = clients[i].buf;

					uint32_t msgnum = read_u32(p);
					uint32_t date = read_u32(p + 4);
					uint32_t t1 = read_u32(p + 8);
					uint32_t t2 = read_u32(p + 12);
					uint32_t mlen = read_u32(p + 16);

					// гарантируем что буфер влезет
					if (clients[i].capacity < 20 + mlen)
					{
						while (clients[i].capacity < 20 + mlen)
							clients[i].capacity *= 2;

						clients[i].buf = (unsigned char*)realloc(
							clients[i].buf, clients[i].capacity);
					}

					if (clients[i].len < 20 + mlen)
						break;

					char* msg = (char*)malloc(mlen + 1);
					memcpy(msg, p + 20, mlen);
					msg[mlen] = 0;

					write_msg(f, &clients[i].addr, date, t1, t2, msg);

					send(clients[i].sock, "ok", 2, 0);

					if (strcmp(msg, "stop") == 0)
						stop = 1;

					free(msg);

					memmove(clients[i].buf, clients[i].buf + 20 + mlen,
							clients[i].len - (20 + mlen));

					clients[i].len -= 20 + mlen;
				}
			}
		}
	}

	for (i = 0; i < MAX_CLIENTS; i++)
		if (clients[i].active)
		{
			free(clients[i].buf);
			close_socket(clients[i].sock);
		}

	close_socket(ls);

	fclose(f);

#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}
