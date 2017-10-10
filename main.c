//  [5/2/2015 uty]
//	[10/10/2017 rohan kumbhar]
#include <ntddk.h>
#include "vmminitstate.h"
#include "amd64/common-asm.h"
#include "amd64/vmx-asm.h"
#include "vmx.h"
#include "ept.h"

//-----------------------------------------------------------------------------//
typedef void (*PFUNC)(void);
//-----------------------------------------------------------------------------//
NTSTATUS
RunOnProcessor (
	__in ULONG ProcessorNumber,
	__in PFUNC Routine
	)
{
	KIRQL OldIrql;

	KeSetSystemAffinityThread((KAFFINITY)(1 << ProcessorNumber));
	
	OldIrql = KeRaiseIrqlToDpcLevel();

	//
	// Initialize VMX on every CPU
	//

	Routine();

	KeLowerIrql(OldIrql);

	KeRevertToUserAffinityThread();

	return STATUS_SUCCESS;
}
//-----------------------------------------------------------------------------//
//DEV_EXT g_DevExt = {0};
//-----------------------------------------------------------------------------//
NTSTATUS
ExitVMM (
	VOID
	)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	KAFFINITY Affinity = 0;
	LONG i = 0;

	Affinity = KeQueryActiveProcessors();

	DbgPrint("KeQueryActiveProcessors: %p\n", Affinity);

	for (i = 0; i < 64; i++)
	{
		if (TRUE == _bittest64(&Affinity, i))
		{
			DbgPrint("CPU %d\n", i);
			Status = RunOnProcessor(i, StopVMX);
			if (STATUS_SUCCESS != Status)
			{
				DbgPrint("RunOnProcessor failed on processor %d\n", i);
				break;
			}
		}
	}

	return Status;
}
//-----------------------------------------------------------------------------//
NTSTATUS
EnterVMM (
	VOID
	)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	KAFFINITY Affinity = 0;
	LONG i = 0;

	Affinity = KeQueryActiveProcessors();

	DbgPrint("KeQueryActiveProcessors: %p\n", Affinity);

	for (i = 0; i < 64; i++)
	{
		if (TRUE == _bittest64(&Affinity, i))
		{
			Status = InitializeVMMInitState(&g_VMMInitState[i]);

			DbgPrint("CPU %d\n", i);
			Status = RunOnProcessor(i, StartVMX);
			if (STATUS_SUCCESS != Status)
			{
				DbgPrint("RunOnProcessor failed on processor %d\n", i);
				break;
			}
		}
	}

	return Status;
}
//-----------------------------------------------------------------------------//
NTSTATUS
DriverEntry (
	__in PDRIVER_OBJECT DriverObject,
	__in PUNICODE_STRING RegistryPath
	)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
    
	LONG i = 0;
	
	Status = GetE820FromRegistry(&g_TotalPhysicalMemory);
	if (STATUS_SUCCESS != Status)
	{
		//DbgPrint("GetE820FromRegistry fail 0x%x\n", Status);
		goto Exit0;
	}

	Status = InitEptTable();
	if (STATUS_SUCCESS != Status)
	{
		goto Exit0;
	}

	EnterVMM();

	DbgPrint("VMM running!\n");


	Status = STATUS_SUCCESS;
Exit0:

	if (STATUS_SUCCESS != Status)
	{
		for (i = 0; i < KeNumberProcessors; i++)
		{
			UninitializeVMMInitState(&g_VMMInitState[i]);
		}
	}
	

	return Status;
}
//-----------------------------------------------------------------------------//