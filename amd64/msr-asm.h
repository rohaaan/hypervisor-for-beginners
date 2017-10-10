//  [5/4/2015 uty]
#ifndef _MSR_ASM_H_
#define _MSR_ASM_H_
//-----------------------------------------------------------------------------//
ULONG64 
NTAPI
MsrRead (
	__in ULONG32 reg
	);

VOID
NTAPI
MsrWrite (
  __in ULONG32 reg,
  __in ULONG64 MsrValue
);
//-----------------------------------------------------------------------------//
#endif
