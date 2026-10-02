#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/personality.h>
#include <cpuid.h>

#define ll unsigned long long

const char* str_open_error =
	"\xB8\xF8\xBC\xD9\x4F\xB9\xE6\xBC\xD8\xBF\xD8\xB5\xD7\xBC\xD1\xB9\xE4\xBD"
	"\xE0\x4F\xB8\xDB\xBD\xEE\xBF\xD2\xB4\xEC\xBD\xE4\xB9\xE7\xBD\xE0\x4F\xB9"
	"\xE1\xBC\xDC\xBF\xD1\xB5\xD7";
const char* str_memory_error =
	"\xB8\xFB\xBD\xE4\xBF\xD0\xB5\xDD\xBC\xD5\xB8\xD5\x4C\xBC\xDD\xB9\xEE\xBC"
	"\xD8\xBF\xDD\xB5\xD7\xBC\xDA\xB8\xD8\xBC\xD4\xBE\xE7\x45\xBC\xD3\xBF\xD8"
	"\xB5\xD0\xBD\xE0\xB9\xE7\xBC\xD4";

const char* str_password_file =
	"\x18\x04\x1F\x1F\x18\x07\x17\x08\x42\x1B\x10\x11";
const char* str_serial_file = "\x1B\x65\x1E\x05\x0E\x04\x4B\x18\x14\x1B";

const char* str_format = "\x4D\x16\x49\x1F\x4A\x1B";
const char* str_print_s = "\x4D\x16";
const char* str_numbers = "\x59\x57\x5F\x58\x5A\x5E\x52\x54\x55";
const char* str_wrong_pass = "\x3F\x17\x03\x02\x08\x48\x15\x0D\x1F\x1C\x62";
const char* str_success = "\x1B\x10\x0F\x0F\x1C\x0D\x16\x1F\x66";

const char* str_write = "\x1F";
const char* str_scan_int = "\x4D\x01";
const char* str_print_ll = "\x4D\x09\x6C\x19\x65";

const char* str_serial_prefix = "\x23\x20\x35\x48";
const char* str_serial_suffix = "\x4C";

char* get_key();

char* xor_str(const char* str, const char* key)
{
	int len = strlen(str);
	int key_len = strlen(key);

	char* new = (char*)malloc(len + 1);

	if (new == NULL)
		return NULL;

	for (int i = 0; i < len; i++)
	{
		new[i] = str[i] ^ key[i % key_len];

		if (new[i] == '\0')
			new[i] = str[i];
	}

	new[len] = '\0';

	return new;
}

static long long get_time_ns(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

const char* first = "he";

// void print_byte_str(const char* str)
// {
// 	int len = strlen(str);
// 	char* x_str = xor_str(str, "hello");

// 	for (int i = 0; i < len; i++)
// 	{
// 		printf("\\x%02X", (unsigned char)x_str[i]);
// 	}

// 	printf("\n");

// 	free(x_str);
// }

uint32_t calculate_crc32(const uint8_t* data, size_t length)
{
	uint32_t crc = 0xFFFFFFFF;
	static uint32_t table[256];
	static int initialized = 0;
	if (!initialized)
	{
		for (uint32_t i = 0; i < 256; i++)
		{
			uint32_t c = i;
			for (int j = 0; j < 8; j++)
			{
				if (c & 1)
					c = 0xEDB88320 ^ (c >> 1);
				else
					c >>= 1;
			}
			table[i] = c;
		}
		initialized = 1;
	}
	for (size_t i = 0; i < length; i++)
	{
		crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
	}
	return crc ^ 0xFFFFFFFF;
}

void generate_random_string(char* str, int length)
{
	char* key = get_key();

	char* charset = xor_str(
		"\x09\x07\x0F\x08\x0A\x0E\x02\x04\x05\x05\x03\x09\x01\x02\x6F\x18\x14"
		"\x1E\x1F\x1B\x1D\x13\x1B\x14\x16\x12\x24\x2E\x2F\x2B\x2D\x23\x2B\x24"
		"\x26\x22\x2E\x20\x21\x21\x27\x35\x3D\x3E\x3C\x3C\x30\x3A\x3B\x37\x31"
		"\x3F\x5C\x5D\x5D\x5B\x51\x59\x5A\x58\x50\x5C",
		key);

	free(key);

	int charset_size = strlen(charset);

	for (int i = 0; i < length; i++)
	{
		int index = rand() % charset_size;
		str[i] = charset[index];
	}

	free(charset);
}

char* read_file_to_string(const char* filename)
{
	FILE* file = fopen(filename, "rb");

	if (file == NULL)
	{
		char* key = get_key();
		char* message = xor_str(str_open_error, key);
		free(key);

		perror(message);
		free(message);

		return NULL;
	}

	fseek(file, 0, SEEK_END);

	long file_size = ftell(file);

	rewind(file);

	char* buffer = (char*)malloc(file_size + 1);

	if (buffer == NULL)
	{
		char* key = get_key();
		char* message = xor_str(str_memory_error, key);

		free(key);
		perror(message);

		free(message);
		fclose(file);

		return NULL;
	}

	size_t bytes_read = fread(buffer, 1, file_size, file);
	buffer[bytes_read] = '\0';
	fclose(file);

	return buffer;
}

const char* second = "ll";

typedef struct
{
	ll* data;
	int size;
} dynarr;

void init_buffer(dynarr* buf)
{
	buf->size = 1;
	buf->data = malloc(sizeof(ll));
	buf->data[0] = 1;
}

void add_element(dynarr* buf, ll value)
{
	buf->data = realloc(buf->data, (buf->size + 1) * sizeof(ll));
	buf->data[buf->size++] = value;
}

void compact_buffer(dynarr* buf, int new_start)
{
	int new_size = buf->size - new_start;

	for (int i = 0; i < new_size; i++)
	{
		buf->data[i] = buf->data[new_start + i];
	}

	buf->size = new_size;
	buf->data = realloc(buf->data, buf->size * sizeof(ll));
}

ll give_n_sequence(int n, char* pass, char* right_pass)
{
	// Another check in program
	if (strncmp(pass, right_pass, 9) != 0)
	{
		return 0;
	}

	dynarr buf;

	init_buffer(&buf);

	int i3 = 0;
	int i5 = 0;
	int i7 = 0;

	int last_compacted = 0;
	int prev_min_used = 0;

	for (int i = 1; i <= n; i++)
	{
		ll next3 = buf.data[i3] * 3;
		ll next5 = buf.data[i5] * 5;
		ll next7 = buf.data[i7] * 7;

		ll next = next3;

		if (next5 < next)
			next = next5;

		if (next7 < next)
			next = next7;

		add_element(&buf, next);

		if (next == next3)
			i3++;

		if (next == next5)
			i5++;

		if (next == next7)
			i7++;

		int min_unused = (i3 < i5) ? i3 : i5;
		min_unused = (min_unused < i7) ? min_unused : i7;
		if (min_unused != prev_min_used)
		{
			compact_buffer(&buf, min_unused);

			i3 -= min_unused;
			i5 -= min_unused;
			i7 -= min_unused;

			last_compacted += min_unused;
		}

		prev_min_used = min_unused;
	}

	ll result = buf.data[n - last_compacted];

	free(buf.data);

	return result;
}

const char* third = "o";

int main()
{
	srand(time(NULL));

	// print_byte_str(str_open_error);
	// print_byte_str(str_memory_error);
	// print_byte_str(str_password_file);
	// print_byte_str(str_serial_file);
	// print_byte_str(str_format);
	// print_byte_str(str_numbers);
	// print_byte_str(str_wrong_pass);
	// print_byte_str(str_success);
	// print_byte_str(str_write);
	// print_byte_str(str_scan_int);
	// print_byte_str(str_print_ll);
	// print_byte_str(str_serial_prefix);
	// print_byte_str(str_serial_suffix);

	extern void block1_start();
	extern void block1_end();

	uint8_t* start_ptr = (uint8_t*) && block1_start;
	uint8_t* end_ptr = (uint8_t*) && block1_end;

	size_t block_size = end_ptr - start_ptr;

	uint32_t real_crc = calculate_crc32(start_ptr, block_size);

	uint32_t expected_crc = 0xe51c7c4c;

	// if (real_crc != expected_crc)
	// {
	// 	printf("[ALERT] Кусок кода модифицирован %x\n", real_crc);
	// 	return 0;
	// }

block1_start:
	__asm__ __volatile__("");

	// Check debugger with ptrace
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
	{
		printf("Ptrace is not work!\n");
		return 1;
	}

	// Check by proccess tree
	pid_t ppid = getppid();

	char path[64];
	char cmdline[256];

	snprintf(path, sizeof(path), "/proc/%d/cmdline", ppid);

	FILE* f_proc = fopen(path, "r");

	if (!f_proc)
		return 1;

	size_t sz = fread(cmdline, 1, sizeof(cmdline) - 1, f_proc);
	fclose(f_proc);

	cmdline[sz] = '\0';

	if (strstr(cmdline, "gdb") || strstr(cmdline, "lldb") ||
		strstr(cmdline, "strace") || strstr(cmdline, "ltrace") ||
		strstr(cmdline, "rr"))
	{
		printf("Debugging tool detected: %s\n", cmdline);
		return 1;
	}

	// Check by time of execution
	const long long threshold = 100000000;
	long long start = get_time_ns();

	volatile unsigned long long x = 0;
	for (unsigned long long i = 0; i < 1000000; i++)
		x += i;

	long long end = get_time_ns();
	long long elapsed = end - start;

	if (elapsed > threshold)
	{
		printf("Time end\n");
		return 1;
	}

	// Check by ASLR enabled
	unsigned long personality_value = personality(0xffffffffUL);

	if (personality_value == (unsigned long)-1)
	{
		perror("personality");
		return 1;
	}

	if (personality_value & ADDR_NO_RANDOMIZE)
	{
		printf("ASLR disabled\n");
		return 1;
	}

	// Check CPUID hypervisor present bit
	unsigned int eax, ebx, ecx, edx;

	if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx))
		return 1;

	if (ecx & (1u << 31))
	{
		printf("CPUID\n");
		return 1;
	}

	// Check BIOS and system info
	const char* files[] = {"/sys/class/dmi/id/product_name",
						   "/sys/class/dmi/id/sys_vendor",
						   "/sys/class/dmi/id/board_vendor"};

	const char* vm_names[] = {
		"VMware", "VirtualBox", "KVM", "QEMU", "Microsoft Corporation",
		"innotek"};

	char line[256];

	for (int i = 0; i < 3; ++i)
	{
		FILE* f = fopen(files[i], "r");

		if (!f)
			continue;

		if (fgets(line, sizeof(line), f))
		{
			printf("%s: %s", files[i], line);

			for (int j = 0; j < 6; ++j)
			{
				if (strstr(line, vm_names[j]))
				{
					printf("Info\n");
					fclose(f);
					return 0;
				}
			}
		}

		fclose(f);
	}

	FILE* f_virt = fopen("/proc/modules", "r");

	if (!f_virt)
		return 1;

	char line_virt[512];

	const char* modules[] = {"vboxguest", "vmw_balloon",  "vmwgfx",
							 "virtio",	  "xen_blkfront", "xen_netfront"};

	while (fgets(line_virt, sizeof(line_virt), f_virt))
	{
		for (int i = 0; i < 6; ++i)
		{
			if (strstr(line_virt, modules[i]))
			{
				printf("Virtualization driver detected: %s", modules[i]);

				fclose(f_virt);
				return 0;
			}
		}
	}

	fclose(f_virt);

	char* key = get_key();
	char* pass_filename = xor_str(str_password_file, key);
	char* print_s = xor_str(str_print_s, key);

	// Read pass from file
	char* pass = read_file_to_string(pass_filename);

	free(pass_filename);

	if (pass == NULL)
		return 0;

	char* right_pass = xor_str(str_numbers, key);

	short right = 0;
	char* wrong = xor_str(str_wrong_pass, key);
	// Fake check
	if (strncmp(pass, right_pass, 9) == 0)
	{
		right = 1;
	}

	char* serial = (char*)malloc(16);

	if (serial == NULL)
		return 0;

	char* prefix = xor_str(str_serial_prefix, key);

	serial[0] = prefix[0];
	serial[1] = prefix[1];
	serial[2] = prefix[2];
	serial[3] = prefix[3];

	free(prefix);

	generate_random_string(serial + 4, 10);

	char* suffix = xor_str(str_serial_suffix, key);
	serial[14] = suffix[0];
	free(suffix);
	serial[15] = '\0';

	key = get_key();
	char* serial_filename = xor_str(str_serial_file, key);

	char* write_mode = xor_str(str_write, key);

	// Real check
	if (strncmp(pass, right_pass, 9) != 0 && (right * (right + 1)) % 2 == 0)
	{
		right = 1;
		printf(print_s, wrong);
		return 0;
	}

	FILE* f = fopen(serial_filename, write_mode);

	free(serial_filename);
	free(write_mode);

	if (f == NULL)
	{
		free(serial);
		return 0;
	}

	fputs(serial, f);

	char* success = xor_str(str_success, key);

	printf(print_s, success);
	free(success);

	fclose(f);
	free(serial);

	int n;

block1_end:
	__asm__ __volatile__("");

	while (1)
	{
		// Check in real program
		if (strncmp(pass, right_pass, 9) != 0 &&
			((7 * right * right) - 1) % 3 != 0)
		{
			return 0;
		}

		// Another fake check
		if (strncmp(pass, right_pass, 9) == 0)
		{
			right = 1;
		}

		char* input_format = xor_str(str_scan_int, key);
		if (scanf(input_format, &n) != 1)
		{
			free(input_format);
			break;
		}
		free(input_format);

		char* output_format = xor_str(str_print_ll, key);
		printf(output_format, give_n_sequence(n, pass, right_pass));
		free(output_format);
	}

	free(key);
	free(right_pass);
	free(pass);

	return 0;
}

char* get_key()
{
	char* key = (char*)malloc(6);

	if (key == NULL)
		return NULL;

	sprintf(key, "%s%s%s", first, second, third);

	return key;
}

void marker_end();
