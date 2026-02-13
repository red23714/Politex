#include "kernel.h"

__asm("jmp kmain");

struct idt_entry g_idt[256]; // Реальная таблица IDT
struct idt_ptr g_idtp;		 // Описатель таблицы для команды lidt

// Пустой обработчик прерываний.
void default_intr_handler()
{
	asm("pusha");
	// TODO обработчик прерываний
	asm("popa; leave; iret");
}

void intr_reg_handler(int num, unsigned short segm_sel, unsigned short flags,
					  intr_handler hndlr)
{
	unsigned int hndlr_addr = (unsigned int)hndlr;

	g_idt[num].base_lo = (unsigned short)(hndlr_addr & 0xFFFF);
	g_idt[num].segm_sel = segm_sel;
	g_idt[num].always0 = 0;
	g_idt[num].flags = flags;
	g_idt[num].base_hi = (unsigned short)(hndlr_addr >> 16);
}

void intr_init()
{
	int i;
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);

	for (i = 0; i < idt_count; i++)
	{
		if (i != 0x09) // только клавиатура
			intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR,
							 default_intr_handler);
	}
}

void intr_start()
{
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);

	g_idtp.base = (unsigned int)(&g_idt[0]);
	g_idtp.limit = (sizeof(struct idt_entry) * idt_count) - 1;

	asm("lidt %0" : : "m"(g_idtp));
}

void intr_enable() { asm("sti"); }

void intr_disable() { asm("cli"); }

bool bm_mode;

extern "C" int kmain()
{
	const char* hello = "Welcum to GoydaOS!\n";

	clear_screen(0x07);
	print_const_str(hello);

	int boot_choice = BOOT_MODE_ADDR;

	bool bm = (boot_choice == 0) ? 1 : 0;

	bm_mode = bm;

	intr_disable();
	intr_init();
	keyb_init();
	intr_start();
	intr_enable();

	while (1)
	{
		asm("hlt");
	}

	return 0;
}
