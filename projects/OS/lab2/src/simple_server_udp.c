#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <string.h>

unsigned int get_addr_from_dns_datagram(const char* datagram, int size);

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

int main()
{
	int s;
	struct sockaddr_in addr;
	int i;

#ifdef _WIN32
	int flags = 0;
#else
	int flags = MSG_NOSIGNAL;
#endif

	init();

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
		return sock_err("socket", s);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		return sock_err("bind", s);

	do
	{
		char buffer[1024] = {0};
		int len = 0;
		int addrlen = sizeof(addr);

		int rcv = recvfrom(s, buffer, sizeof(buffer), 0,
						   (struct sockaddr*)&addr, &addrlen);

		if (rcv > 0)
		{
			unsigned int ip = ntohl(addr.sin_addr.s_addr);

			printf("Datagram received from address: %u.%u.%u.%u ",
				   (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF,
				   (ip) & 0xFF);

			for (i = 0; i < rcv; i++)
			{
				if (buffer[i] == '\n')
					break;
				len++;
			}

			printf(" string len is: %d\n", len);
		}

		sprintf(buffer, "Length of your string: %d chars.", len);

		sendto(s, buffer, strlen(buffer), flags, (struct sockaddr*)&addr,
			   addrlen);
	} while (1);

	s_close(s);

	deinit();

	return 0;
}
