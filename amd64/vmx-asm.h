//  [5/4/2015 uty]
#ifndef _VMX_ASM_H_
#define _VMX_ASM_H_
//-----------------------------------------------------------------------------//
VOID VmxTurnOn (PHYSICAL_ADDRESS VmxonPA);
VOID VmxPtrld (PHYSICAL_ADDRESS VmcsPA);
VOID VmxClear(PHYSICAL_ADDRESS VmcsPA);
VOID VmxWrite (ULONG64 Field, ULONG64 Value);
ULONG64 VmxRead (ULONG64 Field);
VOID VmxLaunch (VOID);

VOID VmxExitHandler (VOID);

NTSTATUS VmxExitCall();

NTSTATUS VmxExitPoint(VOID);

VOID VmxExitOff(ULONG64 rip, ULONG64 rsp);
//-----------------------------------------------------------------------------//
#endif
