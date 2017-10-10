//  [5/4/2015 uty]
#ifndef _REGS_ASM_H_
#define _REGS_ASM_H_
//-----------------------------------------------------------------------------//
ULONG64 RegGetCr0 (VOID);
ULONG64 RegGetCr3 (VOID);
ULONG64 RegGetCr4 (VOID);
VOID RegSetCr0(ULONG64);
VOID RegSetCr3(ULONG64);
VOID RegSetCr4(ULONG64);
VOID RegSetBitCr4 (ULONG mask);
VOID RegClearBitCr4 (ULONG mask);

ULONG64 RegGetRflags(VOID);

USHORT RegGetCs(VOID);
USHORT RegGetDs(VOID);
USHORT RegGetEs(VOID);
USHORT RegGetSs(VOID);
USHORT RegGetFs(VOID);
USHORT RegGetGs(VOID);

ULONG64 RegGetIdtBase(VOID);
USHORT RegGetIdtLimit(VOID);
ULONG64 RegGetGdtBase(VOID);
USHORT RegGetGdtLimit(VOID);
USHORT RegGetLdtr(VOID);
USHORT RegGetTr(VOID);

VOID
GetCpuIdInfo (
  __in ULONG32 fn,
  __out PULONG32 ret_eax,
  __out PULONG32 ret_ebx,
  __out PULONG32 ret_ecx,
  __out PULONG32 ret_edx
  );


VOID
ReloadGdtr (
	PVOID GdtBase,
	ULONG GdtLimit
	);

VOID
ReloadIdtr (
	PVOID IdtBase,
	ULONG IdtLimit
	);

//-----------------------------------------------------------------------------//
#endif
