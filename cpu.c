//  [5/5/2015 uty]
#include <ntddk.h>
#include "cpu.h"
#include "amd64/vmx-asm.h"
#include "vmx.h"
//-----------------------------------------------------------------------------//
NTSTATUS
GetSegmentDescriptor (
	__in PSEGMENT_SELECTOR SegmentSelector,
	__in USHORT Selector,
	__in PUCHAR GdtBase
	)
{
	PSEGMENT_DESCRIPTOR SegDesc;

	if (!SegmentSelector)
		return STATUS_INVALID_PARAMETER;

	if (Selector & 0x4) {
		return STATUS_INVALID_PARAMETER;
	}

	SegDesc = (PSEGMENT_DESCRIPTOR) ((PUCHAR) GdtBase + (Selector & ~0x7));

	SegmentSelector->sel   = Selector;
	SegmentSelector->base  = SegDesc->base0 | SegDesc->base1 << 16 | SegDesc->base2 << 24;
	SegmentSelector->limit = SegDesc->limit0 | (SegDesc->limit1attr1 & 0xf) << 16;
	SegmentSelector->attributes.UCHARs = SegDesc->attr0 | (SegDesc->limit1attr1 & 0xf0) << 4;

	if (!(SegDesc->attr0 & LA_STANDARD)) {
		ULONG64 tmp;
		// this is a TSS or callgate etc, save the base high part
		tmp = (*(PULONG64) ((PUCHAR) SegDesc + 8));
		SegmentSelector->base = (SegmentSelector->base & 0xffffffff) | (tmp << 32);
	}

	if (SegmentSelector->attributes.fields.g) {
		// 4096-bit granularity is enabled for this segment, scale the limit
		SegmentSelector->limit = (SegmentSelector->limit << 12) + 0xfff;
	}

	return STATUS_SUCCESS;
}
//-----------------------------------------------------------------------------//
NTSTATUS
VmxFillGuestSelectorData (
	__in PVOID GdtBase,
	__in ULONG Segreg,
	__in USHORT Selector
	)
{
	SEGMENT_SELECTOR SegmentSelector = { 0 };
	ULONG            uAccessRights;

	GetSegmentDescriptor(&SegmentSelector, Selector, GdtBase);
	uAccessRights = ((PUCHAR) & SegmentSelector.attributes)[0] + (((PUCHAR) & SegmentSelector.attributes)[1] << 12);

	if (!Selector)
		uAccessRights |= 0x10000;

	VmxWrite (GUEST_ES_SELECTOR + Segreg * 2, Selector);
	VmxWrite (GUEST_ES_LIMIT + Segreg * 2, SegmentSelector.limit);
	VmxWrite (GUEST_ES_AR_BYTES + Segreg * 2, uAccessRights);
	VmxWrite (GUEST_ES_BASE + Segreg * 2, SegmentSelector.base);

	return STATUS_SUCCESS;
}
//-----------------------------------------------------------------------------//