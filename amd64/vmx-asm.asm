EXTERN doVmxExitHandler:PROC
EXTERN VmxRestoreOldRegs:PROC 

HVM_SAVE_ALL_NOSEGREGS MACRO
        push r15
        push r14
        push r13
        push r12
        push r11
        push r10
        push r9
        push r8        
        push rdi
        push rsi
        push rbp
        push rbp	; rsp
        push rbx
        push rdx
        push rcx
        push rax
ENDM

HVM_RESTORE_ALL_NOSEGREGS MACRO
        pop rax
        pop rcx
        pop rdx
        pop rbx
        pop rbp		; rsp
        pop rbp
        pop rsi
        pop rdi 
        pop r8
        pop r9
        pop r10
        pop r11
        pop r12
        pop r13
        pop r14
        pop r15
ENDM

vmx_call MACRO
	BYTE	0Fh, 01h, 0C1h
ENDM

vmx_clear MACRO
	BYTE	066h, 0Fh, 0C7h
ENDM

vmx_ptrld MACRO
	BYTE	0Fh, 0C7h
ENDM

vmx_ptrst MACRO
	BYTE	0Fh, 0C7h
ENDM

vmx_read MACRO
	BYTE	0Fh, 078h
ENDM

vmx_write MACRO
	BYTE	0Fh, 079h
ENDM

vmx_on MACRO
	BYTE	0F3h, 0Fh, 0C7h
ENDM

vmx_off MACRO
	BYTE	0Fh, 01h, 0C4h
ENDM

vmx_resume MACRO
	BYTE	0Fh, 01h, 0C3h
ENDM

vmx_launch MACRO
	BYTE	0Fh, 01h, 0C2h
ENDM


MODRM_EAX_06 MACRO   ;/* [EAX], with reg/opcode: /6 */
	BYTE	030h
ENDM

MODRM_EAX_07 MACRO   ;/* [EAX], with reg/opcode: /7 */
	BYTE	038h
ENDM

MODRM_EAX_ECX MACRO  ;/* [EAX], [ECX] */
	BYTE	0C1h
ENDM


.CODE


;void vmxPtrld(u64 addr)
VmxPtrld PROC 
	push rcx
	mov rax,rsp
	vmx_ptrld
	MODRM_EAX_06
	pop rcx
	ret
VmxPtrld ENDP

;void vmxPtrst(u64 addr)
VmxPtrst PROC 
	push rcx
	mov rax,rsp
	vmx_ptrst
	MODRM_EAX_07
	pop rcx
	ret
VmxPtrst ENDP

;void vmxClear(u64 addr)
VmxClear PROC 
	push rcx
	mov rax,rsp
	vmx_clear
	MODRM_EAX_06
	pop rcx
	ret
VmxClear ENDP

; vmxRead( field)
VmxRead PROC 
	mov rax,rcx
	vmx_read
	MODRM_EAX_ECX  ;read value stored in ecx
	mov rax,rcx    ;return value stored in eax, so pull ecx to eax
	ret
VmxRead ENDP

; void vmxWrite( field,value)
VmxWrite PROC 
	mov rax,rcx
	mov rcx,rdx	
	vmx_write
	MODRM_EAX_ECX  ;read value stored in ecx
	ret
VmxWrite ENDP

;_vmxOff()
VmxTurnOff PROC 
	vmx_off
	ret
VmxTurnOff ENDP

;void vmxOn(addr)
VmxTurnOn PROC 
	push rcx
	mov rax,rsp
	vmx_on
	MODRM_EAX_06
	pop rcx
	ret
VmxTurnOn ENDP

;vmxVmCall(HypercallNumber)
VmxVmCall PROC 
	mov rdx,rcx
	vmx_call
	ret
VmxVmCall ENDP

VmxVmCall2 PROC
	push rax
	push rbx
	push rcx
	
	mov rax, rcx
	mov rbx, rdx
	mov rcx, r8
	vmx_call

	pop rcx
	pop rbx
	pop rax
	ret
VmxVmCall2 ENDP

VmxLaunch PROC	
	vmx_launch
	ret
VmxLaunch ENDP


VmxExitHandler PROC   

	HVM_SAVE_ALL_NOSEGREGS
	
	mov rcx, rsp		;GuestRegs
	sub	rsp, 28h

	;rdtsc
	
	call	doVmxExitHandler
	add	rsp, 28h	
	HVM_RESTORE_ALL_NOSEGREGS

	vmx_resume
	ret

VmxExitHandler ENDP


;VmxExitCall(ExitCallNumber1,ExitCallNumber2)
VmxExitCall PROC 

	mov rax, 20h
    pushfq
	HVM_SAVE_ALL_NOSEGREGS
	
	vmcall
	
	HVM_RESTORE_ALL_NOSEGREGS
	popfq
	
	mov    rax, -1 ;STATUS_UNSUCCESSFUL
	ret
VmxExitCall ENDP

VmxExitPoint PROC

    HVM_RESTORE_ALL_NOSEGREGS
	popfq
	
	mov    rax, 0 ;STATUS_SUCCESS
	ret
	
VmxExitPoint ENDP

;void VmxExitOff(rip,rsp)
VmxExitOff PROC 

    push    rcx
    push    rdx

	call    VmxRestoreOldRegs
	
	pop     rdx
	pop     rcx
    
	vmxoff
	mov	    rsp,rdx
	jmp	    rcx
VmxExitOff ENDP


END
