;;;;;;;;; File: kmain.s
;;; 
;;; Description: 
;;;    Kernel main -- by the time we get here, we're in 64-bit mode,
;;;    and the GDT has been set up for us.
;;; 
;;; Author: Deborah Hooker
;;; 
;;; Revision History: 
;;;    11/03/11 - Initial version. 
;;; 
;;; Current Status: Experimental as hell
;;; 
;;;;;;;;; 
bits 64
section .text
global kmain

;;; Reserve initial kernel stack space
STACKSIZE equ 0x4000            ; 16k stack to start

kmain:
   ;; Make sure interrupts are clear until we get IDT set up.
   cli
   ;; clear screen for now
   ;;mov   edi, 0xB8000
   ;;mov   rax, 0x1F201F201F201F20
   ;;mov   ecx, 500
   ;;rep   movsq
   ;;blah:
   ;;dd    'Istanbul, not Constantinople'
   ;;mov   edi, 0xB8000
   ;;mov   rsi, blah
   ;;mov   ecx, 7
   ;;rep   movsq

   mov   ax, 'I' | 0x0F << 8
   mov   [0xB8000], ax
   mov   ax, 's' | 0x0F << 8
   mov   [0xB8002], ax

   hlt

   ;; mov   esp, _sys_stack + STACKSIZE ; Point the stack at our new stack area.

   ;; BSS section.  Right now, just the stack.
   ;;section .bss
   ;;align 4
_sys_stack:
   ;;resb  STACKSIZE
