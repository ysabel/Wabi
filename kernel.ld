OUTPUT_FORMAT(elf64-x86-64)
ENTRY(kmain)
KERNEL_LMA = 0x0000000000100000;
KERNEL_VMA = 0xFFFFFFFF80000000;
SECTIONS
{
    . = KERNEL_LMA;

    . += KERNEL_VMA;

    .text : AT(ADDR(.text) - KERNEL_VMA) {
        *(EXCLUDE_FILE(*start.o) .text)
        *(.rodata*)
        . = ALIGN(4096);
    }

    .data : AT(ADDR(.data) - KERNEL_VMA) {
        *(.data)
        . = ALIGN(4096);
    }

    .ehframe : AT(ADDR(.ehframe) - KERNEL_VMA) {
        *(.ehframe)
        . = ALIGN(4096);
    }

    .bss : AT(ADDR(.bss) - KERNEL_VMA) {
        *(.bss)
        *(COMMON)
        . = ALIGN(4096);
    }

    end = .;

    /DISCARD/ : {
        *(.comment)
    }
}