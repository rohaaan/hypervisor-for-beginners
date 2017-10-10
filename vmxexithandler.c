//  [5/5/2015 uty]
//  [10/10/2017 rohan kumbhar]
#include <ntddk.h>
#include "vmx.h"
#include "amd64/vmx-asm.h"
#include "amd64/common-asm.h"
#include "amd64/regs-asm.h"
#include "amd64/msr-asm.h"
#include "vmxexithandler.h"
#include "ept.h"
#include "exceptionnmi.h"
#include "vmcall.h"
#include "msr.h"
#pragma warning(disable:4296)
//-----------------------------------------------------------------------------//
VOID
ResumeToNextInstruction (
	VOID
	)
{
	PVOID pResumeRIP = NULL;
	PVOID pCurrentRIP = NULL;
	ULONG ulExitInstructionLength = 0;

	pCurrentRIP = (PVOID)VmxRead(GUEST_RIP);
	ulExitInstructionLength = (ULONG)VmxRead(VM_EXIT_INSTRUCTION_LEN);

	pResumeRIP = (PCHAR)pCurrentRIP + ulExitInstructionLength;

	VmxWrite(GUEST_RIP, (ULONG64)pResumeRIP);
}
//-----------------------------------------------------------------------------//
VOID
HandleCPUID (
	__inout PGUEST_REGS GuestRegs
	)
{
	ULONG     Function, eax=0, ebx=0, ecx=0, edx=0;

	Function = (ULONG)GuestRegs->rax;
	GetCpuIdInfo(Function, &eax, &ebx, &ecx, &edx);

	GuestRegs->rax = (ULONG64)eax;
	GuestRegs->rbx = (ULONG64)ebx;
	GuestRegs->rcx = (ULONG64)ecx;
	GuestRegs->rdx = (ULONG64)edx;
}
//-----------------------------------------------------------------------------//
VOID
HandleCR (
	__inout PGUEST_REGS GuestRegs
	)
{
	ULONG movcrControlRegister = 0;
	ULONG movcrAccessType = 0;
	ULONG movcrOperandType = 0;
	ULONG movcrGeneralPurposeRegister = 0;

	ULONG64 ExitQualification = 0;
	ULONG64 GuestCR0 = 0;
	ULONG64 GuestCR3 = 0;
	ULONG64 GuestCR4 = 0;

	ULONG64 x = 0;

	ExitQualification = VmxRead(EXIT_QUALIFICATION);
	GuestCR0 = VmxRead(GUEST_CR0);
	GuestCR3 = VmxRead(GUEST_CR3);
	GuestCR4 = VmxRead(GUEST_CR4);

	movcrControlRegister = (ULONG)(ExitQualification & 0x0000000F);
	movcrAccessType = (ULONG)((ExitQualification & 0x00000030) >> 4);
	movcrOperandType = (ULONG)((ExitQualification & 0x00000040) >> 6);
	movcrGeneralPurposeRegister = (ULONG)((ExitQualification & 0x00000F00) >> 8);


	/* Process the event (only for MOV CRx, REG instructions) */
	if (movcrOperandType == 0 && (movcrControlRegister == 0 || movcrControlRegister == 3 || movcrControlRegister == 4)) 
	{
		if (movcrAccessType == 0) 
		{
			/* CRx <-- reg32 */

			if (movcrControlRegister == 0) 
				x = GUEST_CR0;
			else if (movcrControlRegister == 3)
				x = GUEST_CR3;
			else
				x = GUEST_CR4;	  

			switch(movcrGeneralPurposeRegister) 
			{
			case 0:  VmxWrite(x, GuestRegs->rax); break;
			case 1:  VmxWrite(x, GuestRegs->rcx); break;
			case 2:  VmxWrite(x, GuestRegs->rdx); break;
			case 3:  VmxWrite(x, GuestRegs->rbx); break;
			case 4:  VmxWrite(x, GuestRegs->rsp); break;
			case 5:  VmxWrite(x, GuestRegs->rbp); break;
			case 6:  VmxWrite(x, GuestRegs->rsi); break;
			case 7:  VmxWrite(x, GuestRegs->rdi); break;
			default: break;
			}
		} 
		else if (movcrAccessType == 1) 
		{
			/* reg32 <-- CRx */

			if (movcrControlRegister == 0)
				x = GuestCR0;
			else if (movcrControlRegister == 3)
				x = GuestCR3;
			else
				x = GuestCR4;

			switch(movcrGeneralPurposeRegister) 
			{
			case 0:  GuestRegs->rax = x; break;
			case 1:  GuestRegs->rcx = x; break;
			case 2:  GuestRegs->rdx = x; break;
			case 3:  GuestRegs->rbx = x; break;
			case 4:  GuestRegs->rsp = x; break;
			case 5:  GuestRegs->rbp = x; break;
			case 6:  GuestRegs->rsi = x; break;
			case 7:  GuestRegs->rdi = x; break;
			default: break;
			}
		}
	}
}
//-----------------------------------------------------------------------------//
VOID
HandleMsrRead (
	__inout PGUEST_REGS GuestRegs
	)
{
	MSR msr = {0};

	//
	// RDMSR. The RDMSR instruction causes a VM exit if any of the following are true:
	// 
	// The "use MSR bitmaps" VM-execution control is 0.
	// The value of ECX is not in the ranges 00000000H - 00001FFFH and C0000000H - C0001FFFH
	// The value of ECX is in the range 00000000H - 00001FFFH and bit n in read bitmap for low MSRs is 1,
	//   where n is the value of ECX.
	// The value of ECX is in the range C0000000H - C0001FFFH and bit n in read bitmap for high MSRs is 1,
	//   where n is the value of ECX & 00001FFFH.
	// 

	if (((0x00000000 <= GuestRegs->rcx) && (GuestRegs->rcx <= 0x00001FFF)) || ((0xC0000000 <= GuestRegs->rcx) && (GuestRegs->rcx <= 0xC0001FFF)))
	{
		msr.Content = MsrRead((ULONG)GuestRegs->rcx);
	}
	else
	{
		msr.Content = 0;
	}

	GuestRegs->rax = msr.Low;
	GuestRegs->rdx = msr.High;
}
//-----------------------------------------------------------------------------//
VOID
HandleMsrWrite (
	__inout PGUEST_REGS GuestRegs
	)
{
	MSR msr = {0};
	
	if (((0x00000000 <= GuestRegs->rcx) && (GuestRegs->rcx <= 0x00001FFF)) || ((0xC0000000 <= GuestRegs->rcx) && (GuestRegs->rcx <= 0xC0001FFF)))
	{
		msr.Low = (ULONG)GuestRegs->rax;
		msr.High = (ULONG)GuestRegs->rdx;
		MsrWrite((ULONG)GuestRegs->rcx, msr.Content);
	}
	else
	{
		// do nothing
	}
}
//-----------------------------------------------------------------------------//
VOID doVmxExitHandler (PGUEST_REGS GuestRegs)
{
	ULONG ulExitReason = 0;

	ulExitReason = (ULONG)VmxRead(VM_EXIT_REASON);

	switch (ulExitReason)
	{
		//
		// 25.1.2  Instructions That Cause VM Exits Unconditionally
		// The following instructions cause VM exits when they are executed in VMX non-root operation: CPUID, GETSEC,
		// INVD, and XSETBV. This is also true of instructions introduced with VMX, which include: INVEPT, INVVPID, 
		// VMCALL, VMCLEAR, VMLAUNCH, VMPTRLD, VMPTRST, VMRESUME, VMXOFF, and VMXON.
		//

	case EXIT_REASON_VMCLEAR:
	case EXIT_REASON_VMPTRLD: 
	case EXIT_REASON_VMPTRST: 
	case EXIT_REASON_VMREAD:  
	case EXIT_REASON_VMRESUME:
	case EXIT_REASON_VMWRITE:
	case EXIT_REASON_VMXOFF:
	case EXIT_REASON_VMXON:
	case EXIT_REASON_VMLAUNCH:
		{
			VmxWrite(GUEST_RFLAGS, VmxRead(GUEST_RFLAGS) | 0x1); // cf=1 indicate vm instructions fail
			ResumeToNextInstruction();
			break;
		}

	case EXIT_REASON_EXCEPTION_NMI:
		{
			HandleExceptionNmi();
			ResumeToNextInstruction();
			break;
		}

	case EXIT_REASON_CPUID:
		{
			HandleCPUID(GuestRegs);
			ResumeToNextInstruction();
			break;
		}

	case EXIT_REASON_INVD:
		{
			_INVD();
			ResumeToNextInstruction();
			break;
		}

	case EXIT_REASON_VMCALL:
		{
			HandleVMCALL(GuestRegs);
			ResumeToNextInstruction();
			break;
		}

	case EXIT_REASON_CR_ACCESS:
		{
			//
			// The first processors to support the virtual-machine extensions 
			// supported only the 1-setting of this control.
			//

			HandleCR(GuestRegs);
			ResumeToNextInstruction();
			break;
		}

	case EXIT_REASON_MSR_READ:
		{
			HandleMsrRead(GuestRegs);
			ResumeToNextInstruction();
			break;
		}

	case EXIT_REASON_MSR_WRITE:
		{
			HandleMsrWrite(GuestRegs);
			ResumeToNextInstruction();
			break;
		}

	case EXIT_REASON_EPT_VIOLATION:
		{
			HandleEptViolation();
			break;
		}

	default:
		{
			DbgPrint("VM_EXIT_REASION %d\n", ulExitReason);
		}
		break;
	}
}
//-----------------------------------------------------------------------------//