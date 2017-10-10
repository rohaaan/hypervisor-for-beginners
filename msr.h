//  [5/4/2015 uty]
#ifndef _MSR_H_
#define _MSR_H_
//-----------------------------------------------------------------------------//
/* MSRs */
#define MSR_IA32_FEATURE_CONTROL                    0x03A
#define MSR_IA32_SYSENTER_CS                        0x174
#define MSR_IA32_SYSENTER_ESP                       0x175
#define MSR_IA32_SYSENTER_EIP                       0x176
#define MSR_IA32_DEBUGCTL                           0x1D9
#define MSR_IA32_VMX_BASIC                          0x480
#define MSR_IA32_VMX_PINBASED_CTLS                  0x481
#define MSR_IA32_VMX_PROCBASED_CTLS                 0x482
#define MSR_IA32_VMX_EXIT_CTLS                      0x483
#define MSR_IA32_VMX_ENTRY_CTLS                     0x484
#define MSR_IA32_VMX_MISC                           0x485
#define MSR_IA32_VMX_CR0_FIXED0                     0x486
#define MSR_IA32_VMX_CR0_FIXED1                     0x487
#define MSR_IA32_VMX_CR4_FIXED0                     0x488
#define MSR_IA32_VMX_CR4_FIXED1                     0x489
#define MSR_IA32_VMX_PROCBASED_CTLS2				0x48B
#define MSR_IA32_VMX_EPT_VPID_CAP					0x48C
#define MSR_IA32_VMX_TRUE_PINBASED_CTLS				0x48D
#define MSR_IA32_VMX_TRUE_PROCBASED_CTLS			0x48E
#define MSR_IA32_VMX_TRUE_EXIT_CTLS					0x48F
#define MSR_IA32_VMX_TRUE_ENTRY_CTLS				0x490
#define MSR_IA32_VMX_VMFUNC							0x491

#define IA32_FEATURE_CONTROL_MSR_LOCK                     0x0001
#define IA32_FEATURE_CONTROL_MSR_ENABLE_VMXON_INSIDE_SMX  0x0002
#define IA32_FEATURE_CONTROL_MSR_ENABLE_VMXON_OUTSIDE_SMX 0x0004
#define IA32_FEATURE_CONTROL_MSR_SENTER_PARAM_CTL         0x7f00
#define IA32_FEATURE_CONTROL_MSR_ENABLE_SENTER            0x8000

#define	MSR_IA32_FS_BASE    				   0xc0000100
#define	MSR_IA32_GS_BASE	                   0xc0000101
//-----------------------------------------------------------------------------//
typedef union _IA32_VMX_BASIC_MSR
{
	struct {
		unsigned RevId		    :32;	// Bits 31...0 contain the VMCS revision identifier
		unsigned szVmxOnRegion  :12;	// Bits 43...32 report # of bytes for VMXON region 
		unsigned RegionClear	:1;	// Bit 44 set only if bits 32-43 are clear
		unsigned Reserved1		:3;	// Undefined
		unsigned PhyAddrWidth	:1;	// Physical address width for referencing VMXON, VMCS, etc.
		unsigned DualMon		:1;	// Reports whether the processor supports dual-monitor
		// treatment of SMI and SMM
		unsigned MemType		:4;	// Memory type that the processor uses to access the VMCS
		unsigned VmExitReport 	:1;	// Reports weather the procesor reports info in the VM-exit
		// instruction information field on VM exits due to execution
		// of the INS and OUTS instructions
		unsigned Reserved2		:9;	// Undefined
	};

	ULONG64 QuadPart;

} IA32_VMX_BASIC_MSR;


typedef union _IA32_FEATURE_CONTROL_MSR
{
	struct  
	{
		unsigned Lock			:1;	// Bit 0 is the lock bit - cannot be modified once lock is set
		unsigned Reserved1		:1;	// Undefined
		unsigned EnableVmxon	:1;	// Bit 2. If this bit is clear, VMXON causes a general protection exception
		unsigned Reserved2		:29;	// Undefined
		unsigned Reserved3		:32;	// Undefined
	};
	
	ULONG64 QuadPart;

} IA32_FEATURE_CONTROL_MSR;
//-----------------------------------------------------------------------------//
typedef union _MSR
{
	struct  
	{
		ULONG Low;
		ULONG High;
	};
	
	ULONG64 Content;
} MSR;
//-----------------------------------------------------------------------------//
#endif