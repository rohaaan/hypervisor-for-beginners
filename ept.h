//  [5/7/2015 uty]
#ifndef _EPT_H_
#define _EPT_H_
//-----------------------------------------------------------------------------//
#include "vmminitstate.h"
//-----------------------------------------------------------------------------//
#define CACHE_TYPE_UC		0x00 /* Uncacheable */
#define CACHE_TYPE_WC		0x01 /* Write-Combining */
#define CACHE_TYPE_WT		0x04 /* Writethrough */
#define CACHE_TYPE_WP		0x05 /* Write-Protect */
#define CACHE_TYPE_WB		0x06 /* Writeback */
#define CACHE_TYPE_UC_MINUS	0x07 /* UC minus */
#define GMTRR_VCNT		MTRR_VCNT_MAX

#define MAX_NUM_OF_PAGES    0x20000
#define EPTE_READ       0x1
#define EPTE_READEXEC   0x5
#define EPTE_WRITE      0x2
#define EPTE_EXECUTE    0x4
#define EPTE_ATTR_MASK  0xFFF
#define EPTE_MT_SHIFT   3
#define EPT_LEVELS      4

struct vt_ept {
	int cnt;
	PVOID pml4;
	ULONG64 pml4_phys;
	PVOID pages[MAX_NUM_OF_PAGES];
	//PMDL mdls[MAX_NUM_OF_PAGES];
	ULONG64 pages_phys[MAX_NUM_OF_PAGES];
};
//-----------------------------------------------------------------------------//
extern struct vt_ept g_ept;
//-------------------------------------------------------------------------//
typedef union _EPT_PML4E 
{
	struct  
	{
		ULONGLONG Read : 1;
		ULONGLONG Write : 1;
		ULONGLONG Execute : 1;
		ULONGLONG Reserved : 5;
		ULONGLONG AccessIgnored : 1;
		ULONGLONG Ignored0 : 3;
		ULONGLONG PageFrameNumber : 40;
		ULONGLONG Ignored1 : 12;
	};
	ULONGLONG QuardPart;

} EPT_PML4E, *PEPT_PML4E;

typedef union _EPT_PDPTE 
{
	struct  
	{
		ULONGLONG Read : 1;
		ULONGLONG Write : 1;
		ULONGLONG Execute : 1;
		ULONGLONG Reserve : 5;
		ULONGLONG AccessIgnored : 1;
		ULONGLONG Ignored0: 3;
		ULONGLONG PageFrameNumber : 40;
		ULONGLONG Ignored1 : 12;
	};
	ULONGLONG QuardPart;

} EPT_PDPTE, *PEPT_PDPTE ;

typedef union _EPT_PDE 
{
	struct  
	{
		ULONGLONG Read : 1;
		ULONGLONG Write : 1;
		ULONGLONG Execute : 1;
		ULONGLONG Reserve : 4;
		ULONGLONG MustBeZero : 1;
		ULONGLONG AccessIgnored : 1;
		ULONGLONG Ignored0: 3;
		ULONGLONG PageFrameNumber : 40;
		ULONGLONG Ignored1 : 12;
	};
	ULONGLONG QuardPart;

} EPT_PDE, *PEPT_PDE ;

typedef union _EPT_PTE 
{
	struct  
	{
		ULONGLONG Read : 1;
		ULONGLONG Write : 1;
		ULONGLONG Execute : 1;
		ULONGLONG MemoryType : 3;
		ULONGLONG IgnorePAT : 1;
		ULONGLONG Ignored0 : 1;
		ULONGLONG AccessIgnored : 1;
		ULONGLONG Ignored1: 3;
		ULONGLONG PageFrameNumber : 40;
		ULONGLONG Ignored2 : 11;
		ULONGLONG SuppressVE: 1;
	};
	ULONGLONG QuardPart;

} EPT_PTE, *PEPT_PTE;
//-----------------------------------------------------------------------------//
typedef union _EPT_PHYSICAL_ADDRESS 
{
	struct  
	{
		ULONGLONG Offset : 12;
		ULONGLONG PteIndex : 9;
		ULONGLONG PdeIndex : 9;
		ULONGLONG PdpteIndex : 9;
		ULONGLONG Pml4eIndex : 9;
		ULONGLONG Reserved : 16;
	};
	ULONGLONG QuardPart;
} EPT_PHYSICAL_ADDRESS, *PEPT_PHYSICAL_ADDRESS;
//-----------------------------------------------------------------------------//
extern KSPIN_LOCK g_EptSpinLock;
//-----------------------------------------------------------------------------//
// in bytes
extern ULONG64 g_TotalPhysicalMemory;
//-----------------------------------------------------------------------------//
//
//-----------------------------------------------------------------------------//
VOID
HandleEptViolation (
	VOID
	);

NTSTATUS
EptInit (
	VOID
	);

NTSTATUS
GetE820FromRegistry (
	__out PULONG64 TotalPhysicalMemory
	);

NTSTATUS
InitEptTable (
	VOID
	);

VOID
EptMapPage (
	__in struct vt_ept* Ept,
	__in BOOLEAN Write,
	__in ULONG64 GuestPhys,
	__in ULONG64 HostPhys, 
	__in ULONG Access,
	__in ULONG CacheType,
	__in BOOLEAN IgnoreHostPhys,  // only change access or CacheType
	__in_opt PKSPIN_LOCK SpinLock
	);

VOID
EptSetPageAccess (
	__in struct vt_ept* Ept,
	__in BOOLEAN Write,
	__in ULONG64 GuestPhys,
	__in ULONG Access,
	__in_opt PKSPIN_LOCK SpinLock
	);

NTSTATUS
SwitchToEPTOriginal (
	__inout PVMM_INIT_STATE VMMInitState
	);

NTSTATUS
SwitchToEPTShadow (
	__inout PVMM_INIT_STATE VMMInitState
	);
//-----------------------------------------------------------------------------//
#endif