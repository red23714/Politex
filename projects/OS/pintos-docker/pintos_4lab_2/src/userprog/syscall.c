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
	int syscall = *(int*)f->esp;

	switch (syscall)
	{
	case SYS_WRITE:
	{
		const char* string = *(const char**)(f->esp + 8);
		int size = *(int*)(f->esp + 12);

		putbuf(string, size);

		break;
	}
	case SYS_EXIT:
	{
		int exit_code = *(int*)(f->esp + 4);
		struct thread* cur = thread_current();
		cur->exit_code = exit_code;
		printf("%s: exit(%d)\n", cur->name, exit_code);
		thread_exit();
		break;
	}
	default:
		thread_exit();
	}
}
