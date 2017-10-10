//  [5/15/2015 uty]
//	[10/10/2017 rohan kumbhar]
#include <ntddk.h>
#include "vmx.h"
#include "ept.h"
#include "amd64/vmx-asm.h"
#include "cpu.h"
#include "vmminitstate.h"
//-----------------------------------------------------------------------------//
VOID
HandleExceptionNmi (
	VOID
	)
{
	ULONG trap = 0;
	ULONG64 rFlags = 0;
	ULONG64 ExitInterruptionInformation = 0;

	ExitInterruptionInformation = VmxRead(VM_EXIT_INTR_INFO);
	trap = (ULONG)ExitInterruptionInformation & INTR_INFO_VECTOR_MASK;

	switch (trap)
	{
	case TRAP_PAGE_FAULT:
		break;
	case TRAP_DEBUG:
		{
			PVMM_INIT_STATE pCurrentVMMInitState = NULL;
			pCurrentVMMInitState = &g_VMMInitState[KeGetCurrentProcessorNumber()];

			rFlags = VmxRead(GUEST_RFLAGS);
			rFlags = rFlags & ~FLAGS_TF_MASK & ~FLAGS_RF_MASK;
			VmxWrite(GUEST_RFLAGS, rFlags);
			break;
		}
	default:
		break;
	}
}
//-----------------------------------------------------------------------------//