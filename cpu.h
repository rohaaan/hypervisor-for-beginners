//  [5/4/2015 uty]
#ifndef _CPU_H_
#define _CPU_H_
//-----------------------------------------------------------------------------//
/*
 * Intel CPU flags in CR0
 */
#define X86_CR0_PE              0x00000001      /* Enable Protected Mode    (RW) */
#define X86_CR0_MP              0x00000002      /* Monitor Coprocessor      (RW) */
#define X86_CR0_EM              0x00000004      /* Require FPU Emulation    (RO) */
#define X86_CR0_TS              0x00000008      /* Task Switched            (RW) */
#define X86_CR0_ET              0x00000010      /* Extension type           (RO) */
#define X86_CR0_NE              0x00000020      /* Numeric Error Reporting  (RW) */
#define X86_CR0_WP              0x00010000      /* Supervisor Write Protect (RW) */
#define X86_CR0_AM              0x00040000      /* Alignment Checking       (RW) */
#define X86_CR0_NW              0x20000000      /* Not Write-Through        (RW) */
#define X86_CR0_CD              0x40000000      /* Cache Disable            (RW) */
#define X86_CR0_PG              0x80000000      /* Paging     

/*
 * Intel CPU features in CR4
 */
#define X86_CR4_VME		0x0001  /* enable vm86 extensions */
#define X86_CR4_PVI		0x0002  /* virtual interrupts flag enable */
#define X86_CR4_TSD		0x0004  /* disable time stamp at ipl 3 */
#define X86_CR4_DE		0x0008  /* enable debugging extensions */
#define X86_CR4_PSE		0x0010  /* enable page size extensions */
#define X86_CR4_PAE		0x0020  /* enable physical address extensions */
#define X86_CR4_MCE		0x0040  /* Machine check enable */
#define X86_CR4_PGE		0x0080  /* enable global pages */
#define X86_CR4_PCE		0x0100  /* enable performance counters at ipl 3 */
#define X86_CR4_OSFXSR		0x0200  /* enable fast FPU save and restore */
#define X86_CR4_OSXMMEXCPT	0x0400  /* enable unmasked SSE exceptions */
#define X86_CR4_VMXE		0x2000  /* enable VMX */

//-----------------------------------------------------------------------------//
//////////////
//  EFLAGS  //
//////////////
typedef struct _EFLAGS
{
	unsigned Reserved1	:10;
	unsigned ID		:1;		// Identification flag
	unsigned VIP		:1;		// Virtual interrupt pending
	unsigned VIF		:1;		// Virtual interrupt flag
	unsigned AC		:1;		// Alignment check
	unsigned VM		:1;		// Virtual 8086 mode
	unsigned RF		:1;		// Resume flag
	unsigned Reserved2	:1;
	unsigned NT		:1;		// Nested task flag
	unsigned IOPL		:2;		// I/O privilege level
	unsigned OF		:1;
	unsigned DF		:1;
	unsigned IF		:1;		// Interrupt flag
	unsigned TF		:1;		// Task flag
	unsigned SF		:1;		// Sign flag
	unsigned ZF		:1;		// Zero flag
	unsigned Reserved3	:1;
	unsigned AF		:1;		// Borrow flag
	unsigned Reserved4	:1;
	unsigned PF		:1;		// Parity flag
	unsigned Reserved5	:1;
	unsigned CF		:1;		// Carry flag [Bit 0]
} EFLAGS;

typedef union _RFLAGS
{
	struct  
	{
		unsigned Reserved1	:10;
		unsigned ID		:1;		// Identification flag
		unsigned VIP		:1;		// Virtual interrupt pending
		unsigned VIF		:1;		// Virtual interrupt flag
		unsigned AC		:1;		// Alignment check
		unsigned VM		:1;		// Virtual 8086 mode
		unsigned RF		:1;		// Resume flag
		unsigned Reserved2	:1;
		unsigned NT		:1;		// Nested task flag
		unsigned IOPL		:2;		// I/O privilege level
		unsigned OF		:1;
		unsigned DF		:1;
		unsigned IF		:1;		// Interrupt flag
		unsigned TF		:1;		// Task flag
		unsigned SF		:1;		// Sign flag
		unsigned ZF		:1;		// Zero flag
		unsigned Reserved3	:1;
		unsigned AF		:1;		// Borrow flag
		unsigned Reserved4	:1;
		unsigned PF		:1;		// Parity flag
		unsigned Reserved5	:1;
		unsigned CF		:1;		// Carry flag [Bit 0]
		unsigned Reserved6	:32;
	};

	ULONG64 Content;
} RFLAGS;

#define FLAGS_CF_MASK (1 << 0)
#define FLAGS_PF_MASK (1 << 2)
#define FLAGS_AF_MASK (1 << 4)
#define FLAGS_ZF_MASK (1 << 6)
#define FLAGS_SF_MASK (1 << 7)
#define FLAGS_TF_MASK (1 << 8)
#define FLAGS_IF_MASK (1 << 9)
#define FLAGS_RF_MASK (1 << 16)
#define FLAGS_TO_ULONG(f) (*(ULONG32*)(&f))
//-----------------------------------------------------------------------------//
/////////////////////////
//  CONTROL REGISTERS  //
/////////////////////////
typedef union _CR0_REG
{
	struct  
	{
		unsigned PE		:1;			// Protected Mode Enabled [Bit 0]
		unsigned MP		:1;			// Monitor Coprocessor FLAG
		unsigned EM		:1;			// Emulate FLAG
		unsigned TS		:1;			// Task Switched FLAG
		unsigned ET		:1;			// Extension Type FLAG
		unsigned NE		:1;			// Numeric Error
		unsigned Reserved1	:10;	        	// 
		unsigned WP		:1;			// Write Protect
		unsigned Reserved2	:1;			// 
		unsigned AM		:1;			// Alignment Mask
		unsigned Reserved3	:10;     		// 
		unsigned NW		:1;			// Not Write-Through
		unsigned CD		:1;			// Cache Disable
		unsigned PG		:1;			// Paging Enabled
#ifdef _AMD64_
		unsigned Reserved4   :32;
#endif
	};
	
#ifdef _AMD64_
	ULONG64 Content;
#else
	ULONG32 Content;
#endif

} CR0_REG;

typedef union _CR4_REG
{
	struct  
	{
		unsigned VME		:1;			// Virtual Mode Extensions
		unsigned PVI		:1;			// Protected-Mode Virtual Interrupts
		unsigned TSD		:1;			// Time Stamp Disable
		unsigned DE		:1;			// Debugging Extensions
		unsigned PSE		:1;			// Page Size Extensions
		unsigned PAE		:1;			// Physical Address Extension
		unsigned MCE		:1;			// Machine-Check Enable
		unsigned PGE		:1;			// Page Global Enable
		unsigned PCE		:1;			// Performance-Monitoring Counter Enable
		unsigned OSFXSR	:1;			// OS Support for FXSAVE/FXRSTOR
		unsigned OSXMMEXCPT	:1;			// OS Support for Unmasked SIMD Floating-Point Exceptions
		unsigned Reserved1	:2;			// 
		unsigned VMXE		:1;			// Virtual Machine Extensions Enabled
		unsigned Reserved2	:18;		        // 

#ifdef _AMD64_
		unsigned Reserved3   :32;
#endif
	};

#ifdef _AMD64_
	ULONG64 Content;
#else
	ULONG32 Content;
#endif
	
} CR4_REG;
//-----------------------------------------------------------------------------//
//////////////////////////
//  SELECTOR REGISTERS  //
//////////////////////////

#define LA_ACCESSED		0x01
#define LA_READABLE		0x02    // for code segments
#define LA_WRITABLE		0x02    // for data segments
#define LA_CONFORMING	0x04    // for code segments
#define LA_EXPANDDOWN	0x04    // for data segments
#define LA_CODE			0x08
#define LA_STANDARD		0x10
#define LA_DPL_0		0x00
#define LA_DPL_1		0x20
#define LA_DPL_2		0x40
#define LA_DPL_3		0x60
#define LA_PRESENT		0x80

#define LA_LDT64		0x02
#define LA_ATSS64		0x09
#define LA_BTSS64		0x0b
#define LA_CALLGATE64	0x0c
#define LA_INTGATE64	0x0e
#define LA_TRAPGATE64	0x0f

#define HA_AVAILABLE	0x01
#define HA_LONG			0x02
#define HA_DB			0x04
#define HA_GRANULARITY	0x08


#pragma pack (push, 1)

/* 
* Attribute for segment selector. This is a copy of bit 40:47 & 52:55 of the
* segment descriptor. 
*/
typedef union
{
	USHORT UCHARs;
	struct
	{
		USHORT type:4;              /* 0;  Bit 40-43 */
		USHORT s:1;                 /* 4;  Bit 44 */
		USHORT dpl:2;               /* 5;  Bit 45-46 */
		USHORT p:1;                 /* 7;  Bit 47 */
		
		USHORT avl:1;               /* 8;  Bit 52 */
		USHORT l:1;                 /* 9;  Bit 53 */
		USHORT db:1;                /* 10; Bit 54 */
		USHORT g:1;                 /* 11; Bit 55 */
		USHORT Gap:4;

	} fields;
} SEGMENT_ATTRIBUTES;


typedef struct
{
	USHORT sel;
	SEGMENT_ATTRIBUTES attributes;
	ULONG32 limit;
	ULONG64 base;
} SEGMENT_SELECTOR, *PSEGMENT_SELECTOR;

typedef struct _SEGMENT_DESCRIPTOR
{
	USHORT limit0;
	USHORT base0;
	UCHAR  base1;
	UCHAR  attr0;
	UCHAR  limit1attr1;
	UCHAR  base2;
} SEGMENT_DESCRIPTOR,*PSEGMENT_DESCRIPTOR;

#pragma pack(pop)
//-----------------------------------------------------------------------------//
NTSTATUS
GetSegmentDescriptor (
	__in PSEGMENT_SELECTOR SegmentSelector,
	__in USHORT Selector,
	__in PUCHAR GdtBase
	);

NTSTATUS
VmxFillGuestSelectorData (
	__in PVOID GdtBase,
	__in ULONG Segreg,
	__in USHORT Selector
	);
//-----------------------------------------------------------------------------//
#endif