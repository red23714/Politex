typedef unsigned long size_t;

extern long sys_write(int fd, const void* buf, size_t count);

int kmain(void)
{
	const char msg[] = "hello\n";
	sys_write(1, msg, sizeof(msg) - 1);
	return 0;
}
