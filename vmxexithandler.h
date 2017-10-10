//  [5/7/2015 uty]
#ifndef _VMXEXITHANDLER_H_
#define _VMXEXITHANDLER_H_
//-----------------------------------------------------------------------------//
typedef struct _GUEST_REGS
{
	ULONG64 rax;                  // 0x00         // NOT VALID FOR SVM
	ULONG64 rcx;
	ULONG64 rdx;                  // 0x10
	ULONG64 rbx;
	ULONG64 rsp;                  // 0x20         // rsp is not stored here on SVM
	ULONG64 rbp;
	ULONG64 rsi;                  // 0x30
	ULONG64 rdi;
	ULONG64 r8;                   // 0x40
	ULONG64 r9;
	ULONG64 r10;                  // 0x50
	ULONG64 r11;
	ULONG64 r12;                  // 0x60
	ULONG64 r13;
	ULONG64 r14;                  // 0x70
	ULONG64 r15;
} GUEST_REGS, *PGUEST_REGS;
//-----------------------------------------------------------------------------//
#endif