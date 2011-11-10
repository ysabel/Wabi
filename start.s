;;;;;;;;; File: start.s
;;; 
;;; Description:
;;;
;;;   Kernel entry point.  Setup a variety of things to let us get
;;;   started.  Contains the multiboot header (in 32-bit mode) and
;;;   the code necessary to bootstrap us into 64-bit mode.
;;; 
;;; Author: Deborah Hooker
;;; 
;;; Revision History: 
;;;    11/02/11 - Initial version. 
;;; 
;;; Current Status: Experimental as hell
;;; 
;;;;;;;;; 
bits 32
global start

   ;; Multiboot macros to make our life a little easier
   MULTIBOOT_PAGE_ALIGN   equ 1<<0
   MULTIBOOT_MEMORY_INFO  equ 1<<1
   MULTIBOOT_ADDRS        equ 1<<16
   MULTIBOOT_HEADER_MAGIC equ 0x1BADB002
   MULTIBOOT_HEADER_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
   MULTIBOOT_CHECKSUM     equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

   ;; GRUB Multiboot header.
   section .text
   align 4
mboot:
   dd    MULTIBOOT_HEADER_MAGIC
   dd    MULTIBOOT_HEADER_FLAGS
   dd    MULTIBOOT_CHECKSUM

start:
   ;; Grab the info about where our kernel got loaded
   mov   edi, [ebx + 24]
   ;; Enable long mode:
   ;; Disable paging (this may not be necessary, not sure what GRUB did for us)
   mov   eax, cr0
   and   eax, 01111111111111111111111111111111b
   mov   cr0, eax
   ;; Clear the tables
   mov   edi, 0x1000
   mov   cr3, edi
   xor   eax, eax
   mov   ecx, 4096
   rep   stosd
   mov   edi, cr3
   ;; PML4T is at 0x1000
   ;; PDPT is at 0x2000
   ;; PDT is at 0x3000
   ;; PT is at 0x4000
   mov   DWORD [edi], 0x2003
   add   edi, 0x1000
   mov   DWORD [edi], 0x3003
   add   edi, 0x1000
   mov   DWORD [edi], 0x4003
   add   edi, 0x1000
   ;; Identity map the first two megabytes
   mov   ebx, 0x00000003
   mov   ecx, 512
set_entry:
   mov   DWORD [edi], ebx
   add   ebx, 0x1000
   add   edi, 8
   loop  set_entry
   ;; Now enable PAE-paging
   mov   eax, cr4
   or    eax, 1<<5              ; Set PAE (5th bit)
   mov   cr4, eax
   ;; Switch to IA-32e mode
   mov   ecx, 0xC0000080
   rdmsr
   or    eax, 1<<8              ; Set LM (8th bit)
   wrmsr
   mov   eax, cr0
   or    eax, 1<<31 | 1<<0      ; Set PG (31st bit) and PM (0th bit)
   mov   cr0, eax
   ;; And then to 64-bit mode
   jmp  load_gdt64

struc gdt_entry
   limit_low:   resb 2
   base_low:    resb 2
   base_middle: resb 1
   access:      resb 1
   granularity: resb 1
   base_high:   resb 1
endstruc

gdt64:
.null: equ $ - gdt64
   ;; Null descriptor
   istruc gdt_entry
   at limit_low, dw   0
   at base_low, dw    0
   at base_middle, db 0
   at access, db      0
   at granularity, db 0
   at base_high, db   0
   iend
.code: equ $ - gdt64
   ;; Code segment
   istruc gdt_entry
   at limit_low, dw   0
   at base_low, dw    0
   at base_middle, db 0
   at access, db      0x98
   at granularity, db 0x20
   at base_high, db   0
   iend
.data: equ $ - gdt64
   ;; Data segment
   istruc gdt_entry
   at limit_low, dw   0
   at base_low, dw    0
   at base_middle, db 0
   at access, db      0x90
   at granularity, db 0x00
   at base_high, db   0
   iend
.pointer:
   dw    $ - gdt64 - 1          ; Limit
   dd    gdt64                  ; Base

load_gdt64:
   lgdt  [gdt64.pointer]
   jmp   gdt64.code:realm64

bits 64
realm64:
   cli
   mov   ax, gdt64.data
   mov   ds, ax
   mov   es, ax
   mov   fs, ax
   mov   gs, ax
   mov   rdi, 0xFFFFFFFFC0000000 ; This should be the mod location instead.
   push  rdi
   ret
