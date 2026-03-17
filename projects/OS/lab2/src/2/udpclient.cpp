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
#include <stdlib.h>

#define DEBUG 1

#define MAX_MSG 100000

int on_server[MAX_MSG];

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

void send_request(int s, struct sockaddr_in* addr, unsigned char* datagram,
				  uint32_t datagram_len)
{

#ifdef _WIN32
	int flags = 0;
#else
	int flags = MSG_NOSIGNAL;
#endif

	int res = sendto(s, datagram, datagram_len, flags, (struct sockaddr*)addr,
					 sizeof(struct sockaddr_in));
	if (res <= 0)
		sock_err("sendto", s);
}

unsigned int recv_response(int s)
{
	char datagram[1024];
	struct timeval tv = {0, 100 * 1000}; // 100 msec
	int res;

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(s, &fds);

	res = select(s + 1, &fds, 0, 0, &tv);
	if (res > 0)
	{
		struct sockaddr_in addr;
		int addrlen = sizeof(addr);

		int received = recvfrom(s, datagram, sizeof(datagram), 0,
								(struct sockaddr*)&addr, (socklen_t*)&addrlen);

		uint32_t* nums = (uint32_t*)datagram;
		int count = received / 4;

		for (int i = 0; i < count; i++)
		{
			uint32_t num = ntohl(nums[i]);
#if DEBUG
			printf("%u ", num);
#endif
			if (num < MAX_MSG)
				on_server[num] = 1;
		}

		printf("\n");

		if (received <= 0)
		{
			sock_err("recvfrom", s);
			return 0;
		}

		return 1;
	}
	else if (res == 0)
	{
		return 0;
	}
	else
	{
		sock_err("select", s);
		return 0;
	}
}

void pack_message_data(uint32_t msg_num, uint32_t date, uint32_t time1,
					   uint32_t time2, char* message, unsigned char* buffer)
{
	int offset = 0;
	uint32_t net_value;

	// 1. Номер сообщения (4 байта)
	net_value = htonl(msg_num);
	memcpy(buffer + offset, &net_value, 4);
	offset += 4;

	// 2. Дата (4 байта)
	net_value = htonl(date);
	memcpy(buffer + offset, &net_value, 4);
	offset += 4;

	// 3. Первое время (4 байта)
	net_value = htonl(time1);
	memcpy(buffer + offset, &net_value, 4);
	offset += 4;

	// 4. Второе время (4 байта)
	net_value = htonl(time2);
	memcpy(buffer + offset, &net_value, 4);
	offset += 4;

	// 5. Длина сообщения (4 байта)
	uint32_t msg_len = strlen(message);
	net_value = htonl(msg_len);
	memcpy(buffer + offset, &net_value, 4);
	offset += 4;

	// 6. Текст сообщения (N байт)
	memcpy(buffer + offset, message, msg_len);
}

void addChar(char* s, char c)
{
	// Move pointer to the end
	while (*s++)
		;

	// Append the new character
	*(s - 1) = c;

	// Add null terminator to mark new end
	*s = '\0';
}

int tok_numbers(char* src, const char* delim, bool is_reverse)
{
	char* saveptr;
	char* token;
	int final_date = 0;

	token = strtok_r(src, delim, &saveptr);

	if (is_reverse)
		final_date += atoi(token);
	else
		final_date += 10000 * atoi(token);

	token = strtok_r(NULL, delim, &saveptr);
	final_date += 100 * atoi(token);

	token = strtok_r(NULL, delim, &saveptr);

	if (is_reverse)
		final_date += 10000 * atoi(token);
	else
		final_date += atoi(token);

	return final_date;
}

unsigned int create_response(int s, struct sockaddr_in* addr, FILE* f)
{
	int len_step = 256;

	unsigned int message_counter = 0;

	char* response = (char*)calloc(len_step, sizeof(char));
	if (!response)
	{
		printf("Memory allocation failed\n");
		return -1;
	}

	int character;
	bool find_sym = false;
	int len_str = len_step;
	int buffer_size = 0;
	while (1)
	{
		character = fgetc(f);
		if ((character == '\n' || character == EOF) && find_sym)
		{
			if (on_server[message_counter])
			{
				printf("%d\n", message_counter);
				message_counter++;
				continue;
			}

			char* date;
			char* time1;
			char* time2;

			/* strncpy(msg, response + 29, strlen(response) - 29); */

			size_t resp_len = strlen(response);
			unsigned int msg_len = resp_len - 29;

			char* msg = (char*)malloc(msg_len + 1);

			memcpy(msg, response + 29, msg_len);
			msg[msg_len] = '\0';

			char* saveptr;
			date = strtok_r(response, " ", &saveptr);
			time1 = strtok_r(NULL, " ", &saveptr);
			time2 = strtok_r(NULL, " ", &saveptr);

			unsigned int final_date = tok_numbers(date, ".", true);
			unsigned int final_time1 = tok_numbers(time1, ":", false);
			unsigned int final_time2 = tok_numbers(time2, ":", false);

#if DEBUG
			printf("%d | %d | %d | %d | %d | %s\n", message_counter, final_date,
				   final_time1, final_time2, msg_len, msg);
#endif

			uint32_t packet_size = 20 + msg_len;

			unsigned char* output = (unsigned char*)malloc(packet_size);

			pack_message_data(message_counter, final_date, final_time1,
							  final_time2, msg, output);

			send_request(s, addr, output, packet_size);

			find_sym = false;

			memset(response, 0, len_str);

			message_counter++;
			buffer_size = 0;

			free(msg);
			free(output);
		}

		if (buffer_size >= len_str - 1)
		{
			len_str += len_step;

			char* tmp_str = (char*)realloc(response, len_str * sizeof(char));
			if (tmp_str == NULL)
			{
				printf("Memory reallocation failed\n");
				free(response);
				return -1;
			}

			response = tmp_str;
		}

		if (!find_sym && character >= '0' && character <= '9')
		{
			find_sym = true;
		}

		if (find_sym)
		{
			addChar(response, character);
			buffer_size++;
		}
	}

	free(response);
	response = NULL;

	return message_counter;
}

int main(int argc, char* argv[])
{
	const char* address;
	char filename[255];
	int port;
	if (argv[1])
	{
		char* token = strtok(argv[1], ":");
		address = token;
		token = strtok(NULL, ":");
		port = atoi(token);
#if DEBUG
		printf("%s %d\n", address, port);
#endif
	}

	if (argv[2])
	{
		int len = strlen(argv[2]);
		strncpy(filename, argv[2], len);
		filename[len] = '\0';
#if DEBUG
		printf("%s\n", filename);
#endif
	}

	memset(on_server, 0, sizeof(on_server));

	FILE* f;

	f = fopen(filename, "r");

	int s;
	struct sockaddr_in addr;
	int i;

	init();

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
		return sock_err("socket", s);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(address);

	int count = create_response(s, &addr, f);

	int confirmed = 0;

	while (1)
	{
		fseek(f, 0, SEEK_SET); // читаем файл снова

		create_response(s, &addr, f);

		while (recv_response(s))
		{
		}

		confirmed = 0;
		for (int i = 0; i < count; i++)
			if (on_server[i])
				confirmed++;
#if DEBUG
		printf("confirmed | count: %d %d\n", confirmed, count);
#endif
		if (count < 20)
		{
			if (confirmed >= count)
				break;
		}
		else
		{
			if (confirmed >= 20)
				break;
		}
	}

	fclose(f);

	s_close(s);

	deinit();

	return 0;
}
