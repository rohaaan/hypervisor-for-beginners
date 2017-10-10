//  [5/4/2015 uty]
//  [10/10/2017 rohan kumbhar]
#ifndef _VMMINITSTATE_H_
#define _VMMINITSTATE_H_
//-----------------------------------------------------------------------------//
#define VMM_STACK_SIZE      0x8000

typedef struct _VMM_INIT_STATE 
{
	PVOID VMXONRegion;                     /* VMA of VMXON region */
	PHYSICAL_ADDRESS PhysicalVMXONRegion;  /* PMA of VMXON region */

	PVOID VMCSRegion;                      /* VMA of VMCS region */
	PHYSICAL_ADDRESS PhysicalVMCSRegion;   /* PMA of VMCS region */

	PVOID MsrBitmap;                       /* VMA of MSR bitmap */
	PHYSICAL_ADDRESS PhysicalMsrBitmap;    /* PMA of MSR bitmap */

	PVOID VMMStack;                        /* VMM stack area */

	
} VMM_INIT_STATE, *PVMM_INIT_STATE;

#define HYPERVISOR_MAX_CPUS    64
extern VMM_INIT_STATE g_VMMInitState[HYPERVISOR_MAX_CPUS];
//-----------------------------------------------------------------------------//
NTSTATUS
InitializeVMMInitState (
	PVMM_INIT_STATE VMMInitState
	);

NTSTATUS
UninitializeVMMInitState (
	PVMM_INIT_STATE VMMInitState
	);
//-----------------------------------------------------------------------------//
#endif