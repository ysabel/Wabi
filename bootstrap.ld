OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
KERNEL_PHYS = 0x00100000;
SECTIONS
{
    .text KERNEL_PHYS : AT(KERNEL_PHYS) {
        code = .;
        *(.text)
        *(.rodata)
        . = ALIGN(4096);
    } 
    .data : AT(KERNEL_PHYS + (data - code)) {
        data = .;
        *(.data)
        . = ALIGN(4096);
    } 
    .bss : AT(KERNEL_PHYS + (bss - code)) {
        bss = .;
        *(.bss)
        . = ALIGN(4096);
    } 
    end = .;
}
