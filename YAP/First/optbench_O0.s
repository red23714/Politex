	.file	"optbench.c"
	.text
	.globl	i
	.bss
	.align 4
	.type	i, @object
	.size	i, 4
i:
	.zero	4
	.globl	j
	.align 4
	.type	j, @object
	.size	j, 4
j:
	.zero	4
	.globl	k
	.align 4
	.type	k, @object
	.size	k, 4
k:
	.zero	4
	.globl	l
	.align 4
	.type	l, @object
	.size	l, 4
l:
	.zero	4
	.globl	m
	.align 4
	.type	m, @object
	.size	m, 4
m:
	.zero	4
	.globl	i2
	.align 4
	.type	i2, @object
	.size	i2, 4
i2:
	.zero	4
	.globl	j2
	.align 4
	.type	j2, @object
	.size	j2, 4
j2:
	.zero	4
	.globl	k2
	.align 4
	.type	k2, @object
	.size	k2, 4
k2:
	.zero	4
	.globl	g3
	.align 4
	.type	g3, @object
	.size	g3, 4
g3:
	.zero	4
	.globl	h3
	.align 4
	.type	h3, @object
	.size	h3, 4
h3:
	.zero	4
	.globl	i3
	.align 4
	.type	i3, @object
	.size	i3, 4
i3:
	.zero	4
	.globl	k3
	.align 4
	.type	k3, @object
	.size	k3, 4
k3:
	.zero	4
	.globl	m3
	.align 4
	.type	m3, @object
	.size	m3, 4
m3:
	.zero	4
	.globl	i4
	.align 4
	.type	i4, @object
	.size	i4, 4
i4:
	.zero	4
	.globl	j4
	.align 4
	.type	j4, @object
	.size	j4, 4
j4:
	.zero	4
	.globl	i5
	.align 4
	.type	i5, @object
	.size	i5, 4
i5:
	.zero	4
	.globl	j5
	.align 4
	.type	j5, @object
	.size	j5, 4
j5:
	.zero	4
	.globl	k5
	.align 4
	.type	k5, @object
	.size	k5, 4
k5:
	.zero	4
	.globl	flt_1
	.align 8
	.type	flt_1, @object
	.size	flt_1, 8
flt_1:
	.zero	8
	.globl	flt_2
	.align 8
	.type	flt_2, @object
	.size	flt_2, 8
flt_2:
	.zero	8
	.globl	flt_3
	.align 8
	.type	flt_3, @object
	.size	flt_3, 8
flt_3:
	.zero	8
	.globl	flt_4
	.align 8
	.type	flt_4, @object
	.size	flt_4, 8
flt_4:
	.zero	8
	.globl	flt_5
	.align 8
	.type	flt_5, @object
	.size	flt_5, 8
flt_5:
	.zero	8
	.globl	flt_6
	.align 8
	.type	flt_6, @object
	.size	flt_6, 8
flt_6:
	.zero	8
	.globl	ivector
	.align 8
	.type	ivector, @object
	.size	ivector, 12
ivector:
	.zero	12
	.globl	ivector2
	.type	ivector2, @object
	.size	ivector2, 3
ivector2:
	.zero	3
	.globl	ivector4
	.align 8
	.type	ivector4, @object
	.size	ivector4, 12
ivector4:
	.zero	12
	.globl	ivector5
	.align 32
	.type	ivector5, @object
	.size	ivector5, 400
ivector5:
	.zero	400
	.section	.rodata
	.align 8
.LC1:
	.string	"This compiler handles divide-by-zero as                     an error"
	.align 8
.LC5:
	.string	"Common subexpression elimination"
	.align 8
.LC6:
	.string	"This line should not be printed"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	$2, j4(%rip)
	movl	i2(%rip), %edx
	movl	j4(%rip), %eax
	cmpl	%eax, %edx
	jge	.L2
	movl	i4(%rip), %edx
	movl	j4(%rip), %eax
	cmpl	%eax, %edx
	jge	.L2
	movl	$2, i2(%rip)
.L2:
	movl	k5(%rip), %eax
	movl	%eax, j4(%rip)
	movl	i2(%rip), %edx
	movl	j4(%rip), %eax
	cmpl	%eax, %edx
	jge	.L3
	movl	i4(%rip), %edx
	movl	j4(%rip), %eax
	cmpl	%eax, %edx
	jge	.L3
	movl	$3, i5(%rip)
.L3:
	movl	$3, i3(%rip)
	movsd	.LC0(%rip), %xmm0
	movsd	%xmm0, flt_1(%rip)
	movl	$5, i2(%rip)
	movl	i(%rip), %eax
	movl	%eax, j2(%rip)
	movl	i(%rip), %eax
	movl	%eax, k2(%rip)
	movl	i(%rip), %eax
	movl	%eax, i4(%rip)
	movl	$0, i5(%rip)
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movsd	.LC2(%rip), %xmm0
	movsd	%xmm0, flt_3(%rip)
	movsd	.LC3(%rip), %xmm0
	movsd	%xmm0, flt_4(%rip)
	movsd	flt_6(%rip), %xmm1
	pxor	%xmm0, %xmm0
	mulsd	%xmm1, %xmm0
	movsd	%xmm0, flt_5(%rip)
	movsd	flt_2(%rip), %xmm1
	movsd	flt_3(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	movsd	%xmm0, flt_6(%rip)
	movl	$1, k3(%rip)
	movl	$1, k3(%rip)
	movl	j5(%rip), %eax
	sall	$2, %eax
	movl	%eax, k2(%rip)
	movl	$0, i(%rip)
	jmp	.L4
.L5:
	movl	i(%rip), %eax
	leal	(%rax,%rax), %edx
	movl	i(%rip), %eax
	movl	%edx, %ecx
	cltq
	leaq	(%rax,%rax), %rdx
	leaq	ivector4(%rip), %rax
	movw	%cx, (%rdx,%rax)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L4:
	movl	i(%rip), %eax
	cmpl	$5, %eax
	jle	.L5
	movl	$0, j5(%rip)
	movl	$10000, k5(%rip)
.L6:
	movl	k5(%rip), %eax
	subl	$1, %eax
	movl	%eax, k5(%rip)
	movl	j5(%rip), %eax
	addl	$1, %eax
	movl	%eax, j5(%rip)
	movl	k5(%rip), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	leal	(%rax,%rdx), %ecx
	movl	j5(%rip), %edx
	movl	%edx, %eax
	sall	$2, %eax
	leal	(%rdx,%rax), %esi
	movl	%ecx, %eax
	cltd
	idivl	%esi
	movl	%eax, i5(%rip)
	movl	k5(%rip), %eax
	testl	%eax, %eax
	jg	.L6
	movl	$0, i(%rip)
	jmp	.L7
.L8:
	movl	i(%rip), %eax
	addl	%eax, %eax
	addl	$3, %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	ivector5(%rip), %rax
	movl	$5, (%rdx,%rax)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L7:
	movl	i(%rip), %eax
	cmpl	$99, %eax
	jle	.L8
	movl	i(%rip), %eax
	cmpl	$9, %eax
	jg	.L9
	movl	i5(%rip), %edx
	movl	i2(%rip), %eax
	addl	%edx, %eax
	movl	%eax, j5(%rip)
	jmp	.L10
.L9:
	movl	i5(%rip), %edx
	movl	i2(%rip), %eax
	addl	%edx, %eax
	movl	%eax, k5(%rip)
.L10:
	movl	$1, ivector(%rip)
	movl	i2(%rip), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	ivector(%rip), %rax
	movl	$2, (%rdx,%rax)
	movl	i2(%rip), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	ivector(%rip), %rax
	movl	$2, (%rdx,%rax)
	movl	$3, 8+ivector(%rip)
	movl	h3(%rip), %edx
	movl	k3(%rip), %eax
	addl	%edx, %eax
	testl	%eax, %eax
	js	.L11
	movl	h3(%rip), %edx
	movl	k3(%rip), %eax
	addl	%edx, %eax
	cmpl	$5, %eax
	jle	.L12
.L11:
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	jmp	.L13
.L12:
	movl	h3(%rip), %edx
	movl	k3(%rip), %eax
	addl	%edx, %eax
	movl	i3(%rip), %esi
	cltd
	idivl	%esi
	movl	%eax, m3(%rip)
	movl	h3(%rip), %edx
	movl	k3(%rip), %eax
	addl	%eax, %edx
	movl	i3(%rip), %eax
	addl	%edx, %eax
	movl	%eax, g3(%rip)
.L13:
	movl	$0, i4(%rip)
	jmp	.L14
.L15:
	movl	j(%rip), %eax
	movl	%eax, %esi
	movl	k(%rip), %eax
	movl	%eax, %edx
	movl	i4(%rip), %ecx
	movl	%esi, %eax
	imull	%edx, %eax
	movl	%eax, %edx
	movslq	%ecx, %rax
	leaq	ivector2(%rip), %rcx
	movb	%dl, (%rax,%rcx)
	movl	i4(%rip), %eax
	addl	$1, %eax
	movl	%eax, i4(%rip)
.L14:
	movl	i4(%rip), %eax
	cmpl	$2, %eax
	jle	.L15
	leaq	.LC6(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	dead_code
	call	unnecessary_loop
	movl	$7, %edi
	call	loop_jamming
	movl	$7, %edi
	call	loop_unrolling
	movl	$5, %r8d
	movl	$4, %ecx
	movl	$3, %edx
	movl	$2, %esi
	movl	$1, %edi
	call	jump_compression
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	dead_code
	.type	dead_code, @function
dead_code:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, -4(%rbp)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	dead_code, .-dead_code
	.globl	unnecessary_loop
	.type	unnecessary_loop, @function
unnecessary_loop:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$0, -4(%rbp)
	movl	$0, i(%rip)
	jmp	.L19
.L20:
	movl	j5(%rip), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, k5(%rip)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L19:
	movl	i(%rip), %eax
	cmpl	$4, %eax
	jle	.L20
	nop
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	unnecessary_loop, .-unnecessary_loop
	.globl	loop_jamming
	.type	loop_jamming, @function
loop_jamming:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	$0, i(%rip)
	jmp	.L22
.L23:
	movl	j5(%rip), %edx
	movl	i(%rip), %eax
	imull	%eax, %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, k5(%rip)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L22:
	movl	i(%rip), %eax
	cmpl	$4, %eax
	jle	.L23
	movl	$0, i(%rip)
	jmp	.L24
.L25:
	movl	k5(%rip), %eax
	imull	-4(%rbp), %eax
	movl	%eax, %edx
	movl	i(%rip), %eax
	imull	%edx, %eax
	movl	%eax, i5(%rip)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L24:
	movl	i(%rip), %eax
	cmpl	$4, %eax
	jle	.L25
	nop
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	loop_jamming, .-loop_jamming
	.globl	loop_unrolling
	.type	loop_unrolling, @function
loop_unrolling:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	$0, i(%rip)
	jmp	.L27
.L28:
	movl	i(%rip), %eax
	cltq
	leaq	(%rax,%rax), %rdx
	leaq	ivector4(%rip), %rax
	movw	$0, (%rdx,%rax)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L27:
	movl	i(%rip), %eax
	cmpl	$5, %eax
	jle	.L28
	nop
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	loop_unrolling, .-loop_unrolling
	.globl	jump_compression
	.type	jump_compression, @function
jump_compression:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, -16(%rbp)
	movl	%r8d, -20(%rbp)
.L30:
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jge	.L31
	movl	-8(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jge	.L32
	movl	-12(%rbp), %eax
	cmpl	-16(%rbp), %eax
	jge	.L33
	movl	-16(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jge	.L38
	movl	-20(%rbp), %eax
	addl	%eax, -16(%rbp)
	jmp	.L35
.L33:
	movl	-16(%rbp), %eax
	addl	%eax, -12(%rbp)
	jmp	.L35
.L32:
	movl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	jmp	.L30
.L38:
	nop
.L36:
	jmp	.L30
.L31:
	movl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
.L35:
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	addl	%eax, %edx
	movl	-12(%rbp), %eax
	addl	%eax, %edx
	movl	-16(%rbp), %eax
	addl	%eax, %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	jump_compression, .-jump_compression
	.section	.rodata
	.align 8
.LC0:
	.long	1717986918
	.long	1075930726
	.align 8
.LC2:
	.long	858993459
	.long	1073951539
	.align 8
.LC3:
	.long	450359963
	.long	1072693248
	.ident	"GCC: (GNU) 15.2.1 20250813"
	.section	.note.GNU-stack,"",@progbits
