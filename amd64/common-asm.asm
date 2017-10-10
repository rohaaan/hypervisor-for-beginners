EXTERN doStartVMX:PROC

.code

StartVMX PROC

	push rax
	push rcx
	push rdx
	push rbx
	push rbp
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	
	sub rsp, 28h
	
	mov rcx, rsp
	call doStartVMX
	
	;jmp StartVMXBack  ; test
	
StartVMX ENDP

StartVMXBack PROC

	add rsp, 28h
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rbp
	pop rbx
	pop rdx
	pop rcx
	pop rax
	
	ret
	
StartVMXBack ENDP


_INVD PROC

	invd
	ret
	
_INVD ENDP

End
