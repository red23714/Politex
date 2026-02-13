ENTRY(kmain)
SECTIONS
{
    . = 0x10000;         /* адрес загрузки ядра */

    .text : { *(.text) }         /* весь код */
    .rodata : { *(.rodata) }     /* строки */
    .data : { *(.data) }         /* глобальные изменяемые */
    .bss : { *(.bss) }    /* нули */
}
