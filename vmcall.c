//  [5/15/2015 uty]
//	[10/10/2017 rohan kumbhar]
#include <ntddk.h>
#include "ept.h"
#include "vmxexithandler.h"
#include "common.h"
#include "amd64/vmx-asm.h"
#include "vmx.h"
#include "vmcall.h"
//-----------------------------------------------------------------------------//
VOID
HandleVMCALL (
	__inout PGUEST_REGS GuestRegs
	)
{
	PVMM_INIT_STATE pCurrentVMMInitState = NULL;
	pCurrentVMMInitState = &g_VMMInitState[KeGetCurrentProcessorNumber()];

	if (VMCALL_EXIT_VMM == GuestRegs->rax)
	{
		// Exit VMM
		VmxExitOff((ULONG64)VmxExitPoint, VmxRead(GUEST_RSP));
	}
}
//-----------------------------------------------------------------------------//