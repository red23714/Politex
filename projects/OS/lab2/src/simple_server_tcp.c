#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
// Директива линковщику: использовать библиотеку сокетов
#pragma comment(lib, "ws2_32.lib")
#else // LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include <unistd.h>
#include <netinet/in.h>

#endif

#include <stdio.h>
#include <string.h>

int init()
{
#ifdef _WIN32
	WSADATA wsa_data;
	return (0 == WSAStartup(MAKEWORD(2, 2), &wsa_data));
#else
	return 1;
#endif
}

void deinit()
{
#ifdef _WIN32
	WSACleanup();
#else

#endif
}

int sock_err(const char* function, int s)
{
	int err;
#ifdef _WIN32
	err = WSAGetLastError();
#else
	err = errno;
#endif
	fprintf(stderr, "%s: socket error: %d\n", function, err);
	return -1;
}

void s_close(int s)
{
#ifdef _WIN32
	closesocket(s);
#else
	close(s);
#endif
}

int recv_string(int cs)
{
	char buffer[512];
	int curlen = 0;
	int rcv;

	do
	{
		int i;
		rcv = recv(cs, buffer, sizeof(buffer), 0);
		for (i = 0; i < rcv; i++)
		{
			if (buffer[i] == '\n')
				return curlen;
			curlen++;
		}

		if (curlen > 5000)
		{
			printf("input string too large\n");
			return 5000;
		}
	} while (rcv > 0);

	return curlen;
}
int send_notice(int cs, int len)
{
	char buffer[1024];
	int sent = 0;
	int ret;
#ifdef _WIN32
	int flags = 0;
#else
	int flags = MSG_NOSIGNAL;
#endif

	sprintf(buffer, "Length of your string: %d chars.", len);

	while (sent < (int)strlen(buffer))
	{
		ret = send(cs, buffer + sent, strlen(buffer) - sent, flags);
		if (ret <= 0)
			return sock_err("send", cs);
		sent += ret;
	}

	return 0;
}

int main()
{
	int s;
	struct sockaddr_in addr;

	init();

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		return sock_err("socket", s);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		return sock_err("bind", s);

	if (listen(s, 1) < 0)
		return sock_err("listen", s);

	do
	{
		int addrlen = sizeof(addr);
		int cs = accept(s, (struct sockaddr*)&addr, &addrlen);
		unsigned int ip;
		int len;

		if (cs < 0)
		{
			sock_err("accept", s);
			break;
		}

		ip = ntohl(addr.sin_addr.s_addr);
		printf(" Client connected: %u.%u.%u.%u ", (ip >> 24) & 0xFF,
			   (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, (ip) & 0xFF);

		len = recv_string(cs);

		send_notice(cs, len);

		printf(" string len is: %d\n", len);

		s_close(cs);
	} while (1);

	s_close(s);
	deinit();

	return 0;
}
