//  [5/15/2015 uty]
#ifndef _VMCALL_H_
#define _VMCALL_H_
//-----------------------------------------------------------------------------//
#define VMCALL_EXIT_VMM                         0x20
#define VMCALL_EPT_SWITCH_ORIGINAL              0x25
#define VMCALL_EPT_SWITCH_SHADOW                0x26
#define VMCALL_EPT_SHADOW_PAGE                  0x28
//-----------------------------------------------------------------------------//
VOID
HandleVMCALL (
	__inout PGUEST_REGS GuestRegs
	);
//-----------------------------------------------------------------------------//
#endif