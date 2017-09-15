	.text
	.file	"a.c"
	.globl	main
	.p2align	2
	.type	main,@function
main:                                   // @main
// BB#0:
	stp	x29, x30, [sp, #-16]!   // 8-byte Folded Spill
	adrp	x0, .Lstr
	add	x0, x0, :lo12:.Lstr
	mov	 x29, sp
	bl	puts
	adrp	x1, :got:main
	ldr	x1, [x1, :got_lo12:main]
	adrp	x0, .L.str.1
	add	x0, x0, :lo12:.L.str.1
	bl	printf
	orr	w0, wzr, #0x6
	ldp	x29, x30, [sp], #16     // 8-byte Folded Reload
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main

	.type	ncy,@object             // @ncy
	.data
	.globl	ncy
	.p2align	2
ncy:
	.word	4                       // 0x4
	.size	ncy, 4

	.type	.L.str.1,@object        // @.str.1
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.1:
	.asciz	"%p\n"
	.size	.L.str.1, 4

	.type	.Lstr,@object           // @str
	.section	.rodata.str1.4,"aMS",@progbits,1
	.p2align	2
.Lstr:
	.asciz	"hello"
	.size	.Lstr, 6


	.ident	"clang version 4.0.1 (tags/RELEASE_401/final)"
	.section	".note.GNU-stack","",@progbits
