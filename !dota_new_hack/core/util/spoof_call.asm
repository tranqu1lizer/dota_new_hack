.code

InitSpoofCall PROC
	movq   xmm15, rdx
	pinsrq xmm15, rcx, 1
	ret
InitSpoofCall ENDP

SpoofCall PROC
	mov     rbx, qword ptr[rsp]
	movq    qword ptr[rsp], xmm15
	movhlps xmm15, xmm15
	sub     rsp, 8
	movq    qword ptr[rsp], xmm15
	ret
SpoofCall ENDP

END