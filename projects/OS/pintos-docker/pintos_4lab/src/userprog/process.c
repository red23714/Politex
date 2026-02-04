#include "userprog/process.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userprog/gdt.h"
#include "userprog/pagedir.h"
#include "userprog/tss.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/flags.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "lib/kernel/list.h"
#include "lib/string.h"

static thread_func start_process NO_RETURN;
static bool load(const char* cmdline, void (**eip)(void), void** esp);

/* Information about a child process that a parent can wait on. */
struct child_proc
{
	tid_t tid;
	int exit_code;
	bool exited;
	struct semaphore wait_sema;
	struct list_elem elem;
};

/* Global list of child processes for the whole kernel and its lock.
   Note: this is simplified for teaching purposes — in a full system
   you'd likely keep per-process child lists. */
static struct list child_proc_list;
static struct lock child_proc_list_lock;
static bool child_list_inited = false;

static void ensure_child_list_init(void)
{
	if (!child_list_inited)
	{
		list_init(&child_proc_list);
		lock_init(&child_proc_list_lock);
		child_list_inited = true;
	}
}

/* Starts a new thread running a user program loaded from FILE_NAME.
   Returns the new process's tid, or TID_ERROR on failure. */
tid_t process_execute(const char* file_name)
{
	ensure_child_list_init();

	/* Extract program name for the new thread's name (limited length). */
	char prog_name[NAME_MAX + 1];
	const char* space = strchr(file_name, ' ');
	if (space != NULL)
	{
		size_t len = (size_t)(space - file_name);
		if (len > NAME_MAX)
			len = NAME_MAX;
		strlcpy(prog_name, file_name, len + 1);
		prog_name[len] = '\0';
	}
	else
	{
		strlcpy(prog_name, file_name, sizeof(prog_name));
	}

	/* Make a private copy of the command line for the new process. */
	char* cmd_copy = palloc_get_page(0);
	if (cmd_copy == NULL)
		return TID_ERROR;
	strlcpy(cmd_copy, file_name, PGSIZE);

	tid_t tid = thread_create(prog_name, PRI_DEFAULT, start_process, cmd_copy);
	if (tid == TID_ERROR)
	{
		palloc_free_page(cmd_copy);
		return TID_ERROR;
	}

	/* Create child record so parent may wait on it. */
	struct child_proc* cp = calloc(1, sizeof(*cp));
	if (cp != NULL)
	{
		cp->tid = tid;
		cp->exit_code = 0;
		cp->exited = false;
		sema_init(&cp->wait_sema, 0);

		lock_acquire(&child_proc_list_lock);
		list_push_back(&child_proc_list, &cp->elem);
		lock_release(&child_proc_list_lock);
	}

	return tid;
}

static void start_process(void* file_name_)
{
	char* cmd_line = file_name_;
	struct intr_frame if_;
	bool success;

	memset(&if_, 0, sizeof if_);
	if_.gs = if_.fs = if_.es = if_.ds = if_.ss = SEL_UDSEG;
	if_.cs = SEL_UCSEG;
	if_.eflags = FLAG_IF | FLAG_MBS;

	success = load(cmd_line, &if_.eip, &if_.esp);

	/* Free the page we allocated in process_execute. */
	palloc_free_page(cmd_line);

	if (!success)
		thread_exit();

	/* Start user process by returning from an interrupt. */
	asm volatile("movl %0, %%esp; jmp intr_exit" : : "g"(&if_) : "memory");
	NOT_REACHED();
}

int process_wait(tid_t child_tid)
{
	struct list_elem* e;
	struct child_proc* found = NULL;

	ensure_child_list_init();

	lock_acquire(&child_proc_list_lock);
	for (e = list_begin(&child_proc_list); e != list_end(&child_proc_list);
		 e = list_next(e))
	{
		struct child_proc* cp = list_entry(e, struct child_proc, elem);
		if (cp->tid == child_tid)
		{
			found = cp;
			break;
		}
	}
	if (found == NULL)
	{
		lock_release(&child_proc_list_lock);
		return -1;
	}
	lock_release(&child_proc_list_lock);

	/* Wait until child reports exit. */
	sema_down(&found->wait_sema);

	int code = found->exit_code;

	lock_acquire(&child_proc_list_lock);
	list_remove(&found->elem);
	lock_release(&child_proc_list_lock);

	free(found);
	return code;
}

void process_exit(void)
{
	struct thread* cur = thread_current();
	struct list_elem* e;

	ensure_child_list_init();

	lock_acquire(&child_proc_list_lock);
	for (e = list_begin(&child_proc_list); e != list_end(&child_proc_list);
		 e = list_next(e))
	{
		struct child_proc* cp = list_entry(e, struct child_proc, elem);
		if (cp->tid == cur->tid)
		{
			cp->exit_code = cur->exit_code;
			cp->exited = true;
			sema_up(&cp->wait_sema);
			break;
		}
	}
	lock_release(&child_proc_list_lock);

	uint32_t* pd = cur->pagedir;
	if (pd != NULL)
	{
		cur->pagedir = NULL;
		pagedir_activate(NULL);
		pagedir_destroy(pd);
	}
}

void process_activate(void)
{
	struct thread* t = thread_current();
	pagedir_activate(t->pagedir);
	tss_update();
}

/* ELF types and structures (unchanged, kept for compatibility). */
typedef uint32_t Elf32_Word, Elf32_Addr, Elf32_Off;
typedef uint16_t Elf32_Half;

struct Elf32_Ehdr
{
	unsigned char e_ident[16];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off e_phoff;
	Elf32_Off e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
};

struct Elf32_Phdr
{
	Elf32_Word p_type;
	Elf32_Off p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
};

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_STACK 0x6474e551

#define PF_X 1
#define PF_W 2
#define PF_R 4

static bool setup_stack(void** esp);
static bool validate_segment(const struct Elf32_Phdr*, struct file*);
static bool load_segment(struct file* file, off_t ofs, uint8_t* upage,
						 uint32_t read_bytes, uint32_t zero_bytes,
						 bool writable);

/* Parses command line arguments and pushes them to the user stack.
   This implementation keeps limited resource usage and returns false
   on any allocation/formatting error. */
static bool setup_stack_with_args(void** esp, const char* cmdline)
{
	if (!setup_stack(esp))
		return false;

	const size_t MAX_ARGS = 64;
	char* argv[MAX_ARGS];
	int argc = 0;

	char* tmp = palloc_get_page(0);
	if (tmp == NULL)
		return false;
	strlcpy(tmp, cmdline, PGSIZE);

	char* saveptr = NULL;
	char* tok = strtok_r(tmp, " ", &saveptr);
	while (tok != NULL && argc < (int)MAX_ARGS)
	{
		argv[argc++] = tok;
		tok = strtok_r(NULL, " ", &saveptr);
	}

	if (argc == 0)
	{
		palloc_free_page(tmp);
		return false;
	}

	/* Calculate sizes for strings and pointers and align to 4 bytes. */
	int strings_sz = 0;
	for (int i = 0; i < argc; ++i)
		strings_sz += (int)strlen(argv[i]) + 1;
	strings_sz = (strings_sz + 3) & ~3;

	int pointers_sz = (argc + 1) * sizeof(char*);
	int header_sz =
		sizeof(void*) + sizeof(int) + sizeof(char*); /* ret addr, argc, argv */
	int total = header_sz + pointers_sz + strings_sz;

	/* Reserve stack space */
	uint8_t* stack_top = (uint8_t*)*esp - total;

	/* String area starts after header and pointer array */
	char* str_area = (char*)(stack_top + header_sz + pointers_sz);
	char* cur_str = str_area;

	/* Copy strings */
	for (int i = 0; i < argc; ++i)
	{
		size_t l = strlen(argv[i]);
		memcpy(cur_str, argv[i], l);
		cur_str[l] = '\0';
		cur_str += l + 1;
	}

	/* Build argv array (user addresses) */
	char** user_argv = (char**)(stack_top + header_sz);
	char* it = str_area;
	for (int i = 0; i < argc; ++i)
	{
		user_argv[i] = it;
		it += strlen(it) + 1;
	}
	user_argv[argc] = NULL;

	/* Write header: return addr (0), argc, argv pointer */
	void** header = (void**)stack_top;
	header[0] = NULL;				   /* return address */
	header[1] = (void*)(intptr_t)argc; /* argc as integer-sized value */
	header[2] = (void*)user_argv;	   /* argv (pointer into stack) */

	*esp = stack_top;

	palloc_free_page(tmp);
	return true;
}

bool load(const char* cmdline, void (**eip)(void), void** esp)
{
	char file_name[128];
	const char* space = strchr(cmdline, ' ');
	if (space != NULL)
	{
		size_t len = (size_t)(space - cmdline);
		if (len >= sizeof(file_name))
			len = sizeof(file_name) - 1;
		strlcpy(file_name, cmdline, len + 1);
		file_name[len] = '\0';
	}
	else
	{
		strlcpy(file_name, cmdline, sizeof(file_name));
	}

	struct thread* cur = thread_current();
	struct Elf32_Ehdr ehdr;
	struct file* file = NULL;
	off_t file_ofs;
	bool success = false;

	cur->pagedir = pagedir_create();
	if (cur->pagedir == NULL)
		goto done;
	process_activate();

	file = filesys_open(file_name);
	if (file == NULL)
	{
		printf("load: %s: open failed\n", file_name);
		goto done;
	}

	if (file_read(file, &ehdr, sizeof ehdr) != sizeof ehdr ||
		memcmp(ehdr.e_ident, "\177ELF\1\1\1", 7) || ehdr.e_type != 2 ||
		ehdr.e_machine != 3 || ehdr.e_version != 1 ||
		ehdr.e_phentsize != sizeof(struct Elf32_Phdr) || ehdr.e_phnum > 1024)
	{
		printf("load: %s: error loading executable\n", file_name);
		goto done;
	}

	file_ofs = ehdr.e_phoff;
	for (int i = 0; i < ehdr.e_phnum; ++i)
	{
		struct Elf32_Phdr phdr;
		if (file_ofs < 0 || file_ofs > file_length(file))
			goto done;
		file_seek(file, file_ofs);
		if (file_read(file, &phdr, sizeof phdr) != sizeof phdr)
			goto done;
		file_ofs += sizeof phdr;

		switch (phdr.p_type)
		{
		case PT_NULL:
		case PT_NOTE:
		case PT_PHDR:
		case PT_STACK:
			break;
		case PT_DYNAMIC:
		case PT_INTERP:
		case PT_SHLIB:
			goto done;
		case PT_LOAD:
			if (!validate_segment(&phdr, file))
				goto done;
			{
				bool writable = (phdr.p_flags & PF_W) != 0;
				uint32_t file_page = phdr.p_offset & ~PGMASK;
				uint32_t mem_page = phdr.p_vaddr & ~PGMASK;
				uint32_t page_offset = phdr.p_vaddr & PGMASK;
				uint32_t read_bytes, zero_bytes;

				if (phdr.p_filesz > 0)
				{
					read_bytes = page_offset + phdr.p_filesz;
					zero_bytes = (ROUND_UP(page_offset + phdr.p_memsz, PGSIZE) -
								  read_bytes);
				}
				else
				{
					read_bytes = 0;
					zero_bytes = ROUND_UP(page_offset + phdr.p_memsz, PGSIZE);
				}

				if (!load_segment(file, file_page, (void*)mem_page, read_bytes,
								  zero_bytes, writable))
					goto done;
			}
			break;
		default:
			break;
		}
	}

	if (!setup_stack_with_args(esp, cmdline))
		goto done;

	*eip = (void (*)(void))ehdr.e_entry;
	success = true;

done:
	file_close(file);
	return success;
}

static bool validate_segment(const struct Elf32_Phdr* phdr, struct file* file)
{
	if ((phdr->p_offset & PGMASK) != (phdr->p_vaddr & PGMASK))
		return false;
	if (phdr->p_offset > (Elf32_Off)file_length(file))
		return false;
	if (phdr->p_memsz < phdr->p_filesz)
		return false;
	if (phdr->p_memsz == 0)
		return false;
	if (!is_user_vaddr((void*)phdr->p_vaddr))
		return false;
	if (!is_user_vaddr((void*)(phdr->p_vaddr + phdr->p_memsz)))
		return false;
	if (phdr->p_vaddr + phdr->p_memsz < phdr->p_vaddr)
		return false;
	if (phdr->p_vaddr < PGSIZE)
		return false;
	return true;
}

static bool install_page(void* upage, void* kpage, bool writable)
{
	struct thread* t = thread_current();
	return (pagedir_get_page(t->pagedir, upage) == NULL &&
			pagedir_set_page(t->pagedir, upage, kpage, writable));
}

static bool load_segment(struct file* file, off_t ofs, uint8_t* upage,
						 uint32_t read_bytes, uint32_t zero_bytes,
						 bool writable)
{
	ASSERT((read_bytes + zero_bytes) % PGSIZE == 0);
	ASSERT(pg_ofs(upage) == 0);
	ASSERT(ofs % PGSIZE == 0);

	file_seek(file, ofs);
	while (read_bytes > 0 || zero_bytes > 0)
	{
		size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
		size_t page_zero_bytes = PGSIZE - page_read_bytes;

		uint8_t* kpage = palloc_get_page(PAL_USER);
		if (kpage == NULL)
			return false;

		if (page_read_bytes > 0)
		{
			if (file_read(file, kpage, page_read_bytes) != (int)page_read_bytes)
			{
				palloc_free_page(kpage);
				return false;
			}
		}
		if (page_zero_bytes > 0)
			memset(kpage + page_read_bytes, 0, page_zero_bytes);

		if (!install_page((void*)upage, kpage, writable))
		{
			palloc_free_page(kpage);
			return false;
		}

		read_bytes -= page_read_bytes;
		zero_bytes -= page_zero_bytes;
		upage += PGSIZE;
	}
	return true;
}

static bool setup_stack(void** esp)
{
	uint8_t* kpage = palloc_get_page(PAL_USER | PAL_ZERO);
	if (kpage == NULL)
		return false;
	if (!install_page((void*)(PHYS_BASE - PGSIZE), kpage, true))
	{
		palloc_free_page(kpage);
		return false;
	}
	*esp = PHYS_BASE;
	return true;
}
