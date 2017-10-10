.CODE


; void RegSetBitCr4(mask)
RegSetBitCr4 PROC 
	mov rax,cr4
	or  rcx,rax
	mov cr4,rcx	
	ret
RegSetBitCr4 ENDP

; void RegClearBitCr4(mask)
RegClearBitCr4 PROC 
	mov rax,cr4
	not rcx
	and rcx,rax
	mov cr4,rcx	
	ret
RegClearBitCr4 ENDP

RegGetRflags PROC
	pushfq
	pop		rax
	ret
RegGetRflags ENDP



RegGetTSC PROC
;	rdtscp
	rdtsc
	shl		rdx, 32
	or		rax, rdx
	ret
RegGetTSC ENDP

RegGetRax PROC
	mov		rax, rax
	ret
RegGetRax ENDP


RegGetRbx PROC
	mov		rax, rbx
	ret
RegGetRbx ENDP


RegGetCs PROC
	mov		rax, cs
	ret
RegGetCs ENDP

RegGetDs PROC
	mov		rax, ds
	ret
RegGetDs ENDP

RegGetEs PROC
	mov		rax, es
	ret
RegGetEs ENDP

RegGetSs PROC
	mov		rax, ss
	ret
RegGetSs ENDP

RegGetFs PROC
	mov		rax, fs
	ret
RegGetFs ENDP

RegGetGs PROC
	mov		rax, gs
	ret
RegGetGs ENDP

RegGetCr0 PROC
	mov		rax, cr0
	ret
RegGetCr0 ENDP

RegGetCr2 PROC
	mov		rax, cr2
	ret
RegGetCr2 ENDP

RegGetCr3 PROC
	mov		rax, cr3
	ret
RegGetCr3 ENDP

RegSetCr3 PROC
	mov		cr3, rcx
	ret
RegSetCr3 ENDP

RegGetCr4 PROC
	mov		rax, cr4
	ret
RegGetCr4 ENDP

RegSetCr0 PROC
	mov		cr0, rcx
	ret
RegSetCr0 ENDP

RegSetCr4 PROC
	mov		cr4, rcx
	ret
RegSetCr4 ENDP

RegGetCr8 PROC
	mov		rax, cr8
	ret
RegGetCr8 ENDP

RegSetCr8 PROC
	mov		cr8, rcx
	ret
RegSetCr8 ENDP

RegGetDr6 PROC
	mov		rax, dr6
	ret
RegGetDr6 ENDP

RegGetDr0 PROC
	mov		rax, dr0
	ret
RegGetDr0 ENDP

RegGetDr1 PROC
	mov		rax, dr1
	ret
RegGetDr1 ENDP

RegGetDr2 PROC
	mov		rax, dr2
	ret
RegGetDr2 ENDP

RegGetDr3 PROC
	mov		rax, dr3
	ret
RegGetDr3 ENDP

RegSetDr0 PROC
	mov		dr0, rcx
	ret
RegSetDr0 ENDP

RegSetDr1 PROC
	mov		dr1, rcx
	ret
RegSetDr1 ENDP

RegSetDr2 PROC
	mov		dr2, rcx
	ret
RegSetDr2 ENDP

RegSetDr3 PROC
	mov		dr3, rcx
	ret
RegSetDr3 ENDP


RegGetRsp PROC
	mov		rax, rsp
	add		rax, 8
	ret
RegGetRsp ENDP

RegGetIdtBase PROC
	LOCAL	idtr[10]:BYTE
	
	sidt	idtr
	mov		rax, QWORD PTR idtr[2]
	ret
RegGetIdtBase ENDP

RegGetIdtLimit PROC
	LOCAL	idtr[10]:BYTE
	
	sidt	idtr
	mov		ax, WORD PTR idtr[0]
	ret
RegGetIdtLimit ENDP

RegGetGdtBase PROC
	LOCAL	gdtr[10]:BYTE

	sgdt	gdtr
	mov		rax, QWORD PTR gdtr[2]
	ret
RegGetGdtBase ENDP

RegGetGdtLimit PROC
	LOCAL	gdtr[10]:BYTE

	sgdt	gdtr
	mov		ax, WORD PTR gdtr[0]
	ret
RegGetGdtLimit ENDP

RegGetLdtr PROC
	sldt	rax
	ret
RegGetLdtr ENDP

RegGetTr PROC
	str	rax
	ret
RegGetTr ENDP

GetCpuIdInfo PROC
	push	rbp
	mov		rbp, rsp
	push	rbx
	push	rsi

	mov		[rbp+18h], rdx
	mov		eax, ecx
	cpuid
	mov		rsi, [rbp+18h]
	mov		[rsi], eax
	mov		[r8], ebx
	mov		[r9], ecx
	mov		rsi, [rbp+30h]
	mov		[rsi], edx	

	pop		rsi
	pop		rbx
	mov		rsp, rbp
	pop		rbp
	ret
GetCpuIdInfo ENDP

; ReloadGdtr (PVOID GdtBase (rcx), ULONG GdtLimit (rdx) );

ReloadGdtr PROC
	push	rcx
	shl		rdx, 48
	push	rdx
	lgdt	fword ptr [rsp+6]	; do not try to modify stack selector with this ;)
	pop		rax
	pop		rax
	ret
ReloadGdtr ENDP

; ReloadIdtr (PVOID IdtBase (rcx), ULONG IdtLimit (rdx) );

ReloadIdtr PROC
	push	rcx
	shl		rdx, 48
	push	rdx
	lidt	fword ptr [rsp+6]
	pop		rax
	pop		rax
	ret
ReloadIdtr ENDP

END
