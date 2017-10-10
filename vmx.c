//  [5/4/2015 uty]
#include <ntddk.h>
//#include <intrin.h>
#include "common.h"
#include "vmminitstate.h"
#include "msr.h"
#include "amd64/msr-asm.h"
#include "amd64/regs-asm.h"
#include "amd64/vmx-asm.h"
#include "amd64/common-asm.h"
#include "vmx.h"
#include "cpu.h"
#include "ept.h"
#include "shadowhook.h"
//-----------------------------------------------------------------------------//
ULONG
VmxAdjustControls (
	__in ULONG Ctl,
	__in ULONG Msr
	)
{
	MSR MsrValue = {0};

	MsrValue.Content = MsrRead (Msr);
	Ctl &= MsrValue.High;     /* bit == 0 in high word ==> must be zero */
	Ctl |= MsrValue.Low;      /* bit == 1 in low word  ==> must be one  */
	return Ctl;
}
//-----------------------------------------------------------------------------//
ULONG32 vmx_basic_msr_low;
ULONG32 vmx_basic_msr_high;
/* Dynamic (run-time adjusted) execution control flags. */
ULONG32 vmx_pin_based_exec_control;
ULONG32 vmx_pin_based_exec_default1;
ULONG32 vmx_pin_based_exec_default0;

ULONG32 vmx_cpu_based_exec_control;
ULONG32 vmx_cpu_based_exec_default1;
ULONG32 vmx_cpu_based_exec_default0;

ULONG32 vmx_secondary_exec_control;
ULONG32 vmx_secondary_exec_default1;
ULONG32 vmx_secondary_exec_default0;

ULONG32 vmx_vmexit_control;
ULONG32 vmx_vmexit_default1;
ULONG32 vmx_vmexit_default0;

ULONG32 vmx_vmentry_control;
ULONG32 vmx_vmentry_default1;
ULONG32 vmx_vmentry_default0;

ULONG64 vmx_ept_vpid_cap;
//-----------------------------------------------------------------------------/
#define cpu_has_wbinvd_exiting \
	(vmx_secondary_exec_control & SECONDARY_EXEC_WBINVD_EXITING)
#define cpu_has_vmx_virtualize_apic_accesses \
	(vmx_secondary_exec_control & SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES)
#define cpu_has_vmx_tpr_shadow \
	(vmx_cpu_based_exec_control & CPU_BASED_TPR_SHADOW)
#define cpu_has_vmx_vnmi \
	(vmx_pin_based_exec_control & PIN_BASED_VIRTUAL_NMIS)
#define cpu_has_vmx_msr_bitmap \
	(vmx_cpu_based_exec_control & CPU_BASED_ACTIVATE_MSR_BITMAP)
#define cpu_has_vmx_secondary_exec_control \
	(vmx_cpu_based_exec_control & CPU_BASED_ACTIVATE_SECONDARY_CONTROLS)
#define cpu_has_vmx_ept \
	(vmx_secondary_exec_control & CPU_BASED_CTL2_ENABLE_EPT)
#define cpu_has_vmx_vpid \
	(vmx_secondary_exec_control & CPU_BASED_CTL2_ENABLE_VPID)
#define cpu_has_monitor_trap_flag \
	(vmx_cpu_based_exec_control & CPU_BASED_MONITOR_TRAP_FLAG)
#define cpu_has_vmx_pat \
	(vmx_vmentry_control & VM_ENTRY_LOAD_GUEST_PAT)
#define cpu_has_vmx_unrestricted_guest \
	(vmx_secondary_exec_control & SECONDARY_EXEC_UNRESTRICTED_GUEST)
#define cpu_has_vmx_ple \
	(vmx_secondary_exec_control & SECONDARY_EXEC_PAUSE_LOOP_EXITING)
//-----------------------------------------------------------------------------//
/* VMX capabilities detection */
/* Intel IA-32 manual 3B 27.5.1 p. 222 */
void vmx_detect_capability(void)
{
	MSR msr = {0};

	//__debugbreak();


	msr.Content = MsrRead(MSR_IA32_VMX_BASIC);
	vmx_basic_msr_low = msr.Low;
	vmx_basic_msr_high = msr.High;
    

	/* save the revision_id */
    //vmcs_revision_id = vmx_basic_msr_low;

	/* Determine the default1 and default0 for control msrs 
	 *
	 * Intel IA-32 manual 3B Appendix G.3 
	 *
	 * bit == 0 in msr high word ==> must be zero (default0, allowed1)
	 * bit == 1 in msr low word ==> must be one (default1, allowed0)
	 *
	 */

	if ( ! ( vmx_basic_msr_high & (1u<<23) ) )
	{
		// PIN BASED CONTROL
		msr.Content = MsrRead(MSR_IA32_VMX_PINBASED_CTLS);
		vmx_pin_based_exec_default1 = msr.Low;
		vmx_pin_based_exec_default0 = msr.High;

		// PROCESSOR(CPU) BASED CONTROL
		msr.Content = MsrRead(MSR_IA32_VMX_PROCBASED_CTLS);
		vmx_cpu_based_exec_default1 = msr.Low;
		vmx_cpu_based_exec_default0 = msr.High;


		// VMEXIT CONTROL
		msr.Content = MsrRead(MSR_IA32_VMX_EXIT_CTLS);
		vmx_vmexit_default1 = msr.Low;
		vmx_vmexit_default0 = msr.High;

		// VMENTRY CONTROL
		msr.Content = MsrRead(MSR_IA32_VMX_ENTRY_CTLS);
		vmx_vmentry_default1 = msr.Low;
		vmx_vmentry_default0 = msr.High;


		// detect EPT and VPID capability
		if ( vmx_cpu_based_exec_default1 & CPU_BASED_ACTIVATE_SECONDARY_CONTROLS )
		{
			msr.Content = MsrRead(MSR_IA32_VMX_PROCBASED_CTLS2);
			vmx_secondary_exec_default1 = msr.Low;
			vmx_secondary_exec_default0 = msr.High;



			/* The IA32_VMX_EPT_VPID_CAP MSR exists only when EPT or VPID available */
			if ( vmx_secondary_exec_default1 & (CPU_BASED_CTL2_ENABLE_EPT |
				CPU_BASED_CTL2_ENABLE_VPID) )
			{
				vmx_ept_vpid_cap = MsrRead(MSR_IA32_VMX_EPT_VPID_CAP);
			}
		}

		/* First time through. */
		vmx_pin_based_exec_control = vmx_pin_based_exec_default1;
		vmx_cpu_based_exec_control = vmx_cpu_based_exec_default1;
		vmx_secondary_exec_control = vmx_secondary_exec_default1;
		vmx_vmexit_control         = vmx_vmexit_default1;
		vmx_vmentry_control        = vmx_vmentry_default1;
	}
	else /* if the 55 bit is 1 */
	{
		// PIN BASED CONTROL
		msr.Content = MsrRead(MSR_IA32_VMX_TRUE_PINBASED_CTLS);
		vmx_pin_based_exec_default1 = msr.Low;
		vmx_pin_based_exec_default0 = msr.High;

		// PROCESSOR(CPU) BASED CONTROL
		msr.Content = MsrRead(MSR_IA32_VMX_TRUE_PROCBASED_CTLS);
		vmx_cpu_based_exec_default1 = msr.Low;
		vmx_cpu_based_exec_default0 = msr.High;

		// VMEXIT CONTROL
		msr.Content = MsrRead(MSR_IA32_VMX_TRUE_EXIT_CTLS);
		vmx_vmexit_default1 = msr.Low;
		vmx_vmexit_default0 = msr.High;

		// VMENTRY CONTROL
		msr.Content = MsrRead(MSR_IA32_VMX_TRUE_ENTRY_CTLS);
		vmx_vmentry_default1 = msr.Low;
		vmx_vmentry_default0 = msr.High;


		// detect EPT and VPID capability
		if ( vmx_cpu_based_exec_default0 & CPU_BASED_ACTIVATE_SECONDARY_CONTROLS )
		{
			msr.Content = MsrRead(MSR_IA32_VMX_PROCBASED_CTLS2);
			vmx_secondary_exec_default1 = msr.Low;
			vmx_secondary_exec_default0 = msr.High;



			/* The IA32_VMX_EPT_VPID_CAP MSR exists only when EPT or VPID available */
			if ( vmx_secondary_exec_default0 & (CPU_BASED_CTL2_ENABLE_EPT |
				CPU_BASED_CTL2_ENABLE_VPID) )
			{
				//DbgPrint("MSR_IA32_VMX_EPT_VPID_CAP\n");
				vmx_ept_vpid_cap = MsrRead(MSR_IA32_VMX_EPT_VPID_CAP);
			}
		}

		/* First time through. */
		vmx_pin_based_exec_control = vmx_pin_based_exec_default0;
		vmx_cpu_based_exec_control = vmx_cpu_based_exec_default0;
		vmx_secondary_exec_control = vmx_secondary_exec_default0;
		vmx_vmexit_control         = vmx_vmexit_default0;
		vmx_vmentry_control        = vmx_vmentry_default0;
	}
}
//-----------------------------------------------------------------------------//
NTSTATUS
EnableVMX (
	PVMM_INIT_STATE VMMInitState
	)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	IA32_VMX_BASIC_MSR IA32BasicMsr = {0};
	IA32_FEATURE_CONTROL_MSR IA32FeatureControlMsr = {0};

	CR4_REG Cr4 = {0};
	RFLAGS rFlags = {0};
    int cpuInfo[4] = { -1 };


    //
    //  If CPUID.1:ECX.VMX[bit 5] = 1, then VMXoperation is supported.
    //
    // An array of four integers that contains the information returned 
    // in EAX, EBX, ECX, and EDX about supported features of the CPU.
    //

    __cpuid(cpuInfo, 0x1);
    if (0 == _bittest(&cpuInfo[2], 5))
    {
        DbgPrint("EnableVMX(): VMX is not supported.\n");
        Status = STATUS_NOT_SUPPORTED;
        goto Exit0;
    }

	/* Determine the VMX capabilities */
	vmx_detect_capability();

	if ( ! cpu_has_vmx_ept) 
	{
		DbgPrint("EnableVMX(): EPT is not supported.\n");
		Status = STATUS_NOT_SUPPORTED;
		goto Exit0;
	}
	
	RegSetCr4(RegGetCr4() | X86_CR4_VMXE);
	Cr4.Content = RegGetCr4();

	IA32BasicMsr.QuadPart = MsrRead(MSR_IA32_VMX_BASIC);
	IA32FeatureControlMsr.QuadPart = MsrRead(MSR_IA32_FEATURE_CONTROL);

	if (VMX_MEMTYPE_WRITEBACK != IA32BasicMsr.MemType)
	{
		DbgPrint("Unsupported memory type.\n");
		Status = STATUS_NOT_SUPPORTED;
		goto Exit0;
	}

	*(PULONG)(VMMInitState->VMXONRegion) = IA32BasicMsr.RevId;

	VmxTurnOn(VMMInitState->PhysicalVMXONRegion);

	rFlags.Content = RegGetRflags();
	if (1 == rFlags.CF)
	{
		DbgPrint("ERROR: VMXON operation failed.\n");
		Status = STATUS_UNSUCCESSFUL;
		goto Exit0;
	}

	Status = STATUS_SUCCESS;
Exit0:
	return Status;
}
//-----------------------------------------------------------------------------//
NTSTATUS
VmxSetupVMCS (
	PVMM_INIT_STATE VMMInitState,
	PVOID GuestRip,
	PVOID GuestRsp
	)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	IA32_VMX_BASIC_MSR IA32BasicMsr = {0};
	RFLAGS rFlags = {0};
	ULONG64 GdtBase = 0;
	SEGMENT_SELECTOR SegmentSelector = {0};

	IA32BasicMsr.QuadPart = MsrRead(MSR_IA32_VMX_BASIC);

	*(PULONG)VMMInitState->VMCSRegion = IA32BasicMsr.RevId;

	VmxClear(VMMInitState->PhysicalVMCSRegion);
	rFlags.Content = RegGetRflags();
	if (0 != rFlags.CF || 0 != rFlags.ZF)
	{
		DbgPrint("ERROR: VMCLEAR operation failed.\n");
		goto Exit0;
	}

	VmxPtrld(VMMInitState->PhysicalVMCSRegion);


	VmxWrite(HOST_ES_SELECTOR, RegGetEs() & 0xF8);
	VmxWrite(HOST_CS_SELECTOR, RegGetCs() & 0xF8);
	VmxWrite(HOST_SS_SELECTOR, RegGetSs() & 0xF8);
	VmxWrite(HOST_DS_SELECTOR, RegGetDs() & 0xF8);
	VmxWrite(HOST_FS_SELECTOR, RegGetFs() & 0xF8);
	VmxWrite(HOST_GS_SELECTOR, RegGetGs() & 0xF8);
	VmxWrite(HOST_TR_SELECTOR, RegGetTr() & 0xF8);

	VmxWrite(VMCS_LINK_POINTER, 0xFFFFFFFF);
	VmxWrite(VMCS_LINK_POINTER_HIGH, 0xFFFFFFFF);

	VmxWrite(GUEST_IA32_DEBUGCTL, MsrRead(MSR_IA32_DEBUGCTL) & 0xFFFFFFFF);
	VmxWrite(GUEST_IA32_DEBUGCTL_HIGH, MsrRead(MSR_IA32_DEBUGCTL) >> 32);


	/* Time-stamp counter offset */
	VmxWrite(TSC_OFFSET, 0);
	VmxWrite(TSC_OFFSET_HIGH, 0);

	VmxWrite(PAGE_FAULT_ERROR_CODE_MASK, 0);
	VmxWrite(PAGE_FAULT_ERROR_CODE_MATCH, 0);

	VmxWrite(VM_EXIT_MSR_STORE_COUNT, 0);
	VmxWrite(VM_EXIT_MSR_LOAD_COUNT, 0);

	VmxWrite(VM_ENTRY_MSR_LOAD_COUNT, 0);
	VmxWrite(VM_ENTRY_INTR_INFO_FIELD, 0);


	GdtBase = RegGetGdtBase();

	VmxFillGuestSelectorData ((PVOID)GdtBase, ES, RegGetEs ());
	VmxFillGuestSelectorData ((PVOID)GdtBase, CS, RegGetCs ());
	VmxFillGuestSelectorData ((PVOID)GdtBase, SS, RegGetSs ());
	VmxFillGuestSelectorData ((PVOID)GdtBase, DS, RegGetDs ());
	VmxFillGuestSelectorData ((PVOID)GdtBase, FS, RegGetFs ());
	VmxFillGuestSelectorData ((PVOID)GdtBase, GS, RegGetGs ());
	VmxFillGuestSelectorData ((PVOID)GdtBase, LDTR, RegGetLdtr());
	VmxFillGuestSelectorData ((PVOID)GdtBase, TR, RegGetTr());

	VmxWrite(GUEST_FS_BASE, MsrRead(MSR_IA32_FS_BASE));
	VmxWrite(GUEST_GS_BASE, MsrRead(MSR_IA32_GS_BASE));

	VmxWrite(GUEST_INTERRUPTIBILITY_INFO, 0);
	VmxWrite(GUEST_ACTIVITY_STATE, 0);   //Active state 


	VmxWrite(CPU_BASED_VM_EXEC_CONTROL, VmxAdjustControls(CPU_BASED_ACTIVATE_MSR_BITMAP | CPU_BASED_ACTIVATE_SECONDARY_CONTROLS, MSR_IA32_VMX_PROCBASED_CTLS));
	VmxWrite(SECONDARY_VM_EXEC_CONTROL, VmxAdjustControls(CPU_BASED_CTL2_RDTSCP, MSR_IA32_VMX_PROCBASED_CTLS2));


	VmxWrite(PIN_BASED_VM_EXEC_CONTROL, VmxAdjustControls(0, MSR_IA32_VMX_PINBASED_CTLS));
	VmxWrite(VM_EXIT_CONTROLS, VmxAdjustControls(VM_EXIT_IA32E_MODE | VM_EXIT_ACK_INTR_ON_EXIT, MSR_IA32_VMX_EXIT_CTLS));
	VmxWrite(VM_ENTRY_CONTROLS, VmxAdjustControls(VM_ENTRY_IA32E_MODE, MSR_IA32_VMX_ENTRY_CTLS));

	VmxWrite(CR3_TARGET_COUNT,  0);
	VmxWrite(CR3_TARGET_VALUE0, 0);
	VmxWrite(CR3_TARGET_VALUE1, 0);                        
	VmxWrite(CR3_TARGET_VALUE2, 0);
	VmxWrite(CR3_TARGET_VALUE3, 0);

	VmxWrite(GUEST_CR0, RegGetCr0());
	VmxWrite(GUEST_CR3, RegGetCr3());
	VmxWrite(GUEST_CR4, RegGetCr4());

	VmxWrite(GUEST_DR7, 0x400);

	VmxWrite(HOST_CR0, RegGetCr0 ());
	VmxWrite(HOST_CR3, RegGetCr3 ());
	VmxWrite(HOST_CR4, RegGetCr4 ());

	
	VmxWrite(GUEST_GDTR_BASE, RegGetGdtBase());
	VmxWrite(GUEST_IDTR_BASE, RegGetIdtBase());
	VmxWrite(GUEST_GDTR_LIMIT, RegGetGdtLimit());
	VmxWrite(GUEST_IDTR_LIMIT, RegGetIdtLimit());

	VmxWrite(GUEST_RFLAGS, RegGetRflags ());

	VmxWrite(GUEST_SYSENTER_CS, MsrRead (MSR_IA32_SYSENTER_CS));
	VmxWrite(GUEST_SYSENTER_EIP, MsrRead (MSR_IA32_SYSENTER_EIP));
	VmxWrite(GUEST_SYSENTER_ESP, MsrRead (MSR_IA32_SYSENTER_ESP));

	GetSegmentDescriptor(&SegmentSelector, RegGetTr(), (PUCHAR)RegGetGdtBase());
	VmxWrite (HOST_TR_BASE, SegmentSelector.base);

	VmxWrite(HOST_FS_BASE, MsrRead (MSR_IA32_FS_BASE));
	VmxWrite(HOST_GS_BASE, MsrRead (MSR_IA32_GS_BASE));

	VmxWrite(HOST_GDTR_BASE, RegGetGdtBase());
	VmxWrite(HOST_IDTR_BASE, RegGetIdtBase());
	
	VmxWrite(HOST_IA32_SYSENTER_CS, MsrRead (MSR_IA32_SYSENTER_CS));
	VmxWrite(HOST_IA32_SYSENTER_EIP, MsrRead (MSR_IA32_SYSENTER_EIP));
	VmxWrite(HOST_IA32_SYSENTER_ESP, MsrRead (MSR_IA32_SYSENTER_ESP));


	VmxWrite(GUEST_RSP, (ULONG64) GuestRsp);     //setup guest sp
	VmxWrite(GUEST_RIP, (ULONG64) GuestRip);     //setup guest ip

	//
	// Set MSR bitmap, need all 0 to stop vmexit
	//

	VmxWrite(MSR_BITMAP, VMMInitState->PhysicalMsrBitmap.LowPart);
	VmxWrite(MSR_BITMAP_HIGH, VMMInitState->PhysicalMsrBitmap.HighPart);


	VmxWrite(HOST_RSP, ((ULONG64) VMMInitState->VMMStack + VMM_STACK_SIZE - 1));
	VmxWrite(HOST_RIP, (ULONG64) VmxExitHandler);


	Status = STATUS_SUCCESS;
Exit0:
	return Status;
}
//-----------------------------------------------------------------------------//
NTSTATUS
doStartVMX (
	PVOID GuestRsp
	)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	PVMM_INIT_STATE pCurrentVMMInitState = NULL;

	ULONG64 ErrorCode = 0;

	

	pCurrentVMMInitState = &g_VMMInitState[KeGetCurrentProcessorNumber()];

	Status = EnableVMX(pCurrentVMMInitState);
	if (STATUS_SUCCESS != Status)
	{
		DbgPrint("EnableVMX failed.\n");
		goto Exit0;
	}

	//
	// minimal vmcs settings
	//

	Status = VmxSetupVMCS(pCurrentVMMInitState, StartVMXBack, GuestRsp);
	if (STATUS_SUCCESS != Status)
	{
		//DbgPrint("VmxSetupVMCS failed.\n");
		goto Exit0;
	}

 	Status = EptInit();
 	if (STATUS_SUCCESS != Status)
 	{
 		DbgPrint("EptInit failed 0x%x\n", Status);
 		goto Exit0;
 	}

	VmxLaunch();

	// if VmxLaunch success, never be here.

	ErrorCode = VmxRead(VM_INSTRUCTION_ERROR);
	DbgPrint("VM Instruction Error 0x%x\n", (ULONG)ErrorCode);

	Status = STATUS_SUCCESS;
Exit0:
	return Status;
}
//-----------------------------------------------------------------------------//
VOID
StopVMX (
	VOID
	)
{
	VmxExitCall();
	RegSetCr4(RegGetCr4() & (~X86_CR4_VMXE));
}
//-----------------------------------------------------------------------------//
//restore old regs.
VOID
VmxRestoreOldRegs (
	VOID
	)
{
	RegSetCr0((ULONG_PTR)VmxRead(GUEST_CR0));
	RegSetCr3((ULONG_PTR)VmxRead(GUEST_CR3));
	RegSetCr4((ULONG_PTR)VmxRead(GUEST_CR4));

	// restore old GDTR
	ReloadGdtr ((PVOID) VmxRead (GUEST_GDTR_BASE), (ULONG) VmxRead (GUEST_GDTR_LIMIT));

	MsrWrite (MSR_IA32_GS_BASE, VmxRead (GUEST_GS_BASE));
	MsrWrite (MSR_IA32_FS_BASE, VmxRead (GUEST_FS_BASE));

	// restore old IDTR
	ReloadIdtr ((PVOID) VmxRead (GUEST_IDTR_BASE), (ULONG) VmxRead (GUEST_IDTR_LIMIT));
}
//-----------------------------------------------------------------------------//