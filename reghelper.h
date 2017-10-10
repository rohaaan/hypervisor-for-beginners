//  [5/11/2015 uty]
#ifndef _REGHELPER_H_
#define _REGHELPER_H_
//-----------------------------------------------------------------------------//
NTSTATUS
NIAPGetRegValue (
	__in PWCHAR pwszRegPath ,
	__in PWCHAR pwszValueName,
	__inout PVOID Buffer,
	__in ULONG Length,
	__out PULONG puType, 
	__out PULONG puReturnLength
	);
//-----------------------------------------------------------------------------//
#endif