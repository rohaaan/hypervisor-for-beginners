// [5/15/2015 uty]
#include <ntddk.h>
//-----------------------------------------------------------------------------//
#define BITS_PER_BYTE           8
#define BITS_PER_LONG           (sizeof(LONG) * BITS_PER_BYTE)
#define BIT_WORD(nr)            ((nr) / BITS_PER_LONG)
#define BIT_MASK(nr)            (1UL << ((nr) % BITS_PER_LONG))
//-----------------------------------------------------------------------------//
VOID ClearBit(ULONG nr, PULONG addr)
{
	ULONG  mask = BIT_MASK(nr);
	PULONG p = ((PULONG)addr) + BIT_WORD(nr);

	*p &= ~mask;
}
//-----------------------------------------------------------------------------//
VOID SetBit(ULONG nr, PULONG addr)
{
	ULONG  mask = BIT_MASK(nr);
	PULONG p = ((PULONG)addr) + BIT_WORD(nr);

	*p |= mask;
}
//-----------------------------------------------------------------------------//
NTSTATUS
UtLockPagablePage (
	__in PVOID Address	// address within the page
	)
{
	PMDL pMdl = NULL;

	pMdl = IoAllocateMdl((PVOID)((ULONG_PTR)Address & ~(PAGE_SIZE - 1)), PAGE_SIZE, FALSE, FALSE, NULL);
	if (NULL == pMdl)
	{
		return STATUS_UNSUCCESSFUL;
	}

	__try
	{
		MmProbeAndLockPages(pMdl, KernelMode, IoReadAccess);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_UNSUCCESSFUL;
	}
	
	return STATUS_SUCCESS;
}
//-----------------------------------------------------------------------------//