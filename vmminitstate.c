//  [5/4/2015 uty]
//  [10/10/2017 rohan kumbhar]
#include <ntddk.h>
#include "vmminitstate.h"
//-----------------------------------------------------------------------------//
VMM_INIT_STATE g_VMMInitState[HYPERVISOR_MAX_CPUS] = {0};
//-----------------------------------------------------------------------------//
NTSTATUS
InitializeVMMInitState (
	PVMM_INIT_STATE VMMInitState
	)
{
	//
	//  yes, don't care about memory leak if this fail
	//

	VMMInitState->VMXONRegion = MmAllocateNonCachedMemory(4096);
	if (NULL == VMMInitState->VMXONRegion)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	VMMInitState->PhysicalVMXONRegion = MmGetPhysicalAddress(VMMInitState->VMXONRegion);

	VMMInitState->VMCSRegion = MmAllocateNonCachedMemory(4096);
	if (NULL == VMMInitState->VMCSRegion)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	VMMInitState->PhysicalVMCSRegion = MmGetPhysicalAddress(VMMInitState->VMCSRegion);

	VMMInitState->MsrBitmap = MmAllocateNonCachedMemory(4096);
	if (NULL == VMMInitState->MsrBitmap)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	VMMInitState->PhysicalMsrBitmap = MmGetPhysicalAddress(VMMInitState->MsrBitmap);
	memset(VMMInitState->MsrBitmap, 0, 4096);

	// Allocate stack for the VM Exit Handler.
	VMMInitState->VMMStack = ExAllocatePoolWithTag(NonPagedPool, VMM_STACK_SIZE, 'YHKR');
	if (NULL == VMMInitState->VMMStack)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	memset(VMMInitState->VMMStack, 0, VMM_STACK_SIZE);


	return STATUS_SUCCESS;
}
//-----------------------------------------------------------------------------//
NTSTATUS
UninitializeVMMInitState (
	PVMM_INIT_STATE VMMInitState
	)
{
	if (NULL != VMMInitState->VMXONRegion)
	{
		MmFreeNonCachedMemory(VMMInitState->VMXONRegion, 4096);
	}

	if (NULL != VMMInitState->VMCSRegion)
	{
		MmFreeNonCachedMemory(VMMInitState->VMCSRegion, 4096);
	}
	
	if (NULL != VMMInitState->VMMStack)
	{
		ExFreePoolWithTag(VMMInitState->VMMStack, 'ytaU');
	}

	return STATUS_SUCCESS;
}
//-----------------------------------------------------------------------------//