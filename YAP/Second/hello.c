/* Минимальный Hello World на C без библиотек */
typedef unsigned long size_t;

/* объявляем syscall, реализованный в sys_write.S */
extern long sys_write(int fd, const void *buf, size_t count);

/* точка входа (вместо main) */
int kmain(void) {
  const char msg[] = "hello\n";
  sys_write(1, msg, sizeof(msg) - 1);
  return 0;
}
