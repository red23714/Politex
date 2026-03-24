#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAX_CLIENTS 100
#define MAX_PORTS 32
#define MAX_MSG_PER_CLIENT 1000
#define BUFFER_SIZE 65535

typedef int socket_t;

typedef struct
{
	struct sockaddr_in addr;
	uint32_t msgs[MAX_MSG_PER_CLIENT];
	int msg_count;
	time_t last_tm;
	int active;
} client_t;

void set_nonblocking(socket_t s) { fcntl(s, F_SETFL, O_NONBLOCK); }

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

int same_client(struct sockaddr_in* a, struct sockaddr_in* b)
{
	return a->sin_addr.s_addr == b->sin_addr.s_addr &&
		   a->sin_port == b->sin_port;
}

client_t* get_client(client_t* clients, struct sockaddr_in* addr)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
		if (clients[i].active && same_client(&clients[i].addr, addr))
			return &clients[i];

	for (int i = 0; i < MAX_CLIENTS; i++)
		if (!clients[i].active)
		{
			clients[i].active = 1;
			clients[i].addr = *addr;
			clients[i].msg_count = 0;
			clients[i].last_tm = time(NULL);

			printf("New client %s:%d\n", inet_ntoa(addr->sin_addr),
				   ntohs(addr->sin_port));

			return &clients[i];
		}

	return NULL;
}

int has_msg(client_t* c, uint32_t id)
{
	for (int i = 0; i < c->msg_count; i++)
		if (c->msgs[i] == id)
			return 1;
	return 0;
}

void add_msg(client_t* c, uint32_t id)
{
	if (c->msg_count < MAX_MSG_PER_CLIENT)
		c->msgs[c->msg_count++] = id;
}

void send_response(socket_t s, client_t* c, struct sockaddr_in* addr)
{
	uint32_t buf[20];
	int count = 0;

	for (int i = c->msg_count - 1; i >= 0 && count < 20; i--)
		buf[count++] = htonl(c->msgs[i]);

	sendto(s, buf, count * 4, 0, (struct sockaddr*)addr, sizeof(*addr));
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("usage: udpserv port_from port_to\n");
		return 1;
	}

	int port_from = atoi(argv[1]);
	int port_to = (argc > 2) ? atoi(argv[2]) : port_from;

	socket_t socks[MAX_PORTS];
	int sock_count = 0;

	FILE* f = fopen("msg.txt", "a");

	client_t clients[MAX_CLIENTS];
	memset(clients, 0, sizeof(clients));

	for (int p = port_from; p <= port_to; p++)
	{
		socket_t s = socket(AF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in addr;

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(p);
		addr.sin_addr.s_addr = INADDR_ANY;

		if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
			printf("bind error\n");
			return 1;
		}

		set_nonblocking(s);

		socks[sock_count++] = s;

		printf("Listening on port %d\n", p);
	}

	int stop = 0;

	while (!stop)
	{
		fd_set rfds;

		FD_ZERO(&rfds);

		int maxfd = 0;

		for (int i = 0; i < sock_count; i++)
		{
			FD_SET(socks[i], &rfds);
			if (socks[i] > maxfd)
				maxfd = socks[i];
		}

		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int r = select(maxfd + 1, &rfds, NULL, NULL, &tv);

		if (r <= 0)
			continue;

		for (int i = 0; i < sock_count; i++)
		{
			if (!FD_ISSET(socks[i], &rfds))
				continue;

			unsigned char buf[BUFFER_SIZE];

			struct sockaddr_in cli;
			socklen_t len = sizeof(cli);

			int n = recvfrom(socks[i], buf, BUFFER_SIZE, 0,
							 (struct sockaddr*)&cli, &len);

			if (n <= 0)
				continue;

			uint32_t msgnum = read_u32(buf);

			client_t* c = get_client(clients, &cli);

			if (!c)
				continue;

			if (!has_msg(c, msgnum))
			{
				uint32_t date = read_u32(buf + 4);
				uint32_t t1 = read_u32(buf + 8);
				uint32_t t2 = read_u32(buf + 12);
				uint32_t mlen = read_u32(buf + 16);

				if (20 + mlen <= n)
				{
					char* msg = (char*)malloc(mlen + 1);
					if (!msg)
						continue;

					memcpy(msg, buf + 20, mlen);
					msg[mlen] = 0;

					write_msg(f, &cli, date, t1, t2, msg);

					add_msg(c, msgnum);

					if (strcmp(msg, "stop") == 0)
						stop = 1;

					free(msg);
				}
			}

			c->last_tm = time(NULL);

			send_response(socks[i], c, &cli);
		}
	}

	for (int i = 0; i < sock_count; i++)
		close(socks[i]);

	fclose(f);

	return 0;
}
