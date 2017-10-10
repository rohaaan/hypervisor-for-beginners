.CODE

; MsrRead (ULONG32 reg (rcx));

MsrRead PROC
;	xor		rax, rax
	rdmsr				; MSR[ecx] --> edx:eax
	shl		rdx, 32
	or		rax, rdx
	ret
MsrRead ENDP

; MsrWrite (ULONG32 reg (rcx), ULONG64 MsrValue (rdx));

MsrWrite PROC
	mov		rax, rdx
	shr		rdx, 32
	wrmsr
	ret
MsrWrite ENDP

END
