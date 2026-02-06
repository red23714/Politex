#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler(struct intr_frame*);

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void syscall_handler(struct intr_frame* f)
{
	int syscall_number = *(int*)f->esp;

	switch (syscall_number)
	{
	case SYS_EXIT:
	{
		int exit_code = *(int*)(f->esp + 4);
		struct thread* cur = thread_current();
		cur->exit_code = exit_code;
		printf("%s: exit(%d)\n", cur->name, exit_code);
		thread_exit();
		break;
	}
	case SYS_WRITE:
	{
		int fd = *(int*)(f->esp + 4);
		const char* buffer = *(const char**)(f->esp + 8);
		int size = *(int*)(f->esp + 12);

		if (fd == 1)
		{ /* STDOUT */
			putbuf(buffer, size);
		}
		break;
	}
	default:
		printf("Unknown system call: %d\n", syscall_number);
		thread_exit();
	}
}
