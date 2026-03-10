#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>

#define DEBUG 1

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

int send_request(int s, char* request, bool is_num)
{
	int size = strlen(request);
	if (is_num)
	{
		size = 4;
	}

	int sent = 0;

#ifdef _WIN32
	int flags = 0;
#else
	int flags = MSG_NOSIGNAL;
#endif

	while (sent < size)
	{
		// Отправка очередного блока данных
		int res = send(s, request + sent, size - sent, flags);

		if (res < 0)
			return sock_err("send", s);

		sent += res;
		printf("%d bytes sent. \n", sent);
	}

	return 0;
}

void pack_data(int num, char* output)
{
	char pack_str[9];
	sprintf(pack_str, "%d", num);
	unsigned short num_len = strlen(pack_str);
	if (num_len < 8)
	{
		int target_len = 8;

		int zeroes_to_add = target_len - num_len;
		char tmp[9];

		for (int i = 0; i < zeroes_to_add; i++)
		{
			tmp[i] = '0';
		}

		for (int i = zeroes_to_add; i < target_len; i++)
		{
			tmp[i] = pack_str[i - zeroes_to_add];
		}

		strcpy(pack_str, tmp);
		num_len = target_len;
	}

	for (int i = 0; i < num_len; i += 2)
	{
		unsigned short num = '0' - pack_str[i];
		num = num << 4;
		num += '0' - pack_str[i + 1];

		output[i] = num;
	}
}

int recv_response(int s)
{
	char buffer[5];
	int res;

	// Принятие очередного блока данных.
	// Если соединение будет разорвано удаленным узлом recv вернет 0
	while ((res = recv(s, buffer, sizeof(buffer), 0)) > 0)
	{
		printf(" %d bytes receveied\n", res);
	}

	if (res < 0)
		return sock_err("recv", s);

	return 0;
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

void create_response(int s, FILE* f)
{
	int len_step = 256;

	unsigned int message_counter = 0;

	char* response = (char*)calloc(len_step, sizeof(char));
	if (!response)
	{
		printf("Memory allocation failed\n");
		return;
	}

	int character;
	bool find_sym = false;
	int len_str = len_step;
	int buffer_size = 0;
	while ((character = fgetc(f)) != EOF)
	{
		if (character == '\n' && find_sym)
		{
			char* date;
			char* time1;
			char* time2;
			char msg[len_str];

			/* strncpy(msg, response + 29, strlen(response) - 29); */

			size_t resp_len = strlen(response);
			unsigned int msg_len = resp_len - 29;

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

			char out[4];
			pack_data(message_counter, out);
			send_request(s, out, true);

			pack_data(final_date, out);
			send_request(s, out, true);

			pack_data(final_time1, out);
			send_request(s, out, true);

			pack_data(final_time2, out);
			send_request(s, out, true);

			pack_data(msg_len, out);
			send_request(s, out, true);

			send_request(s, msg, false);

			find_sym = false;

			printf("%s %d %d\n", response, len_str, buffer_size);

			memset(response, 0, buffer_size);

			message_counter++;

			printf("all ok\n");

			// free(date);
			// printf("d");
			// free(time1);
			// printf("t1");
			// free(time2);
			// printf("t2");
			// free(token);
		}

		if (buffer_size >= len_str - 1)
		{
			len_str += len_step;

			char* tmp_str = (char*)realloc(response, len_str * sizeof(char));
			if (tmp_str == NULL)
			{
				printf("Memory reallocation failed\n");
				free(response);
				return;
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
	printf("gooooood\n");
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

		printf("%s %d\n", address, port);
	}

	if (argv[2])
	{
		int len = strlen(argv[2]);
		strncpy(filename, argv[2], len);
		filename[len] = '\0';
		printf("%s\n", filename);
	}

	int s;
	struct sockaddr_in addr;

	FILE* f;

	f = fopen(filename, "r");

	init();

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		return sock_err("socket", s);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(address);

	if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) != 0)
	{
		s_close(s);
		return sock_err("connect", s);
	}

	send_request(s, "p", false);
	send_request(s, "u", false);
	send_request(s, "t", false);
	create_response(s, f);

	printf("all in out\n");

	// recv_response(s);
	fclose(f);

	s_close(s);

	deinit();

	return 0;
}
