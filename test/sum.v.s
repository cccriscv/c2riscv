	.file	"sum.c"
	.option nopic
	.section	.rodata
	.align	3
.LC0:
	.string	"i=%d s=%d n=%d\n"
	.align	3
.LC1:
	.string	"i=%d s=%d\n"
	.text
	.align	1
	.globl	sum
	.type	sum, @function
sum:
	addi	sp,sp,-48
	sd	ra,40(sp)
	sd	s0,32(sp)
	addi	s0,sp,48
	mv	a5,a0
	sw	a5,-36(s0)
	sw	zero,-20(s0)
	li	a5,1
	sw	a5,-24(s0)
	lw	a3,-36(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	mv	a2,a4
	mv	a1,a5
	lui	a5,%hi(.LC0)
	addi	a0,a5,%lo(.LC0)
	call	printf
	j	.L2
.L3:
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	addw	a5,a4,a5
	sw	a5,-20(s0)
	lw	a4,-20(s0)
	lw	a5,-24(s0)
	mv	a2,a4
	mv	a1,a5
	lui	a5,%hi(.LC1)
	addi	a0,a5,%lo(.LC1)
	call	printf
	lw	a5,-24(s0)
	addiw	a5,a5,1
	sw	a5,-24(s0)
.L2:
	lw	a4,-24(s0)
	lw	a5,-36(s0)
	sext.w	a4,a4
	sext.w	a5,a5
	ble	a4,a5,.L3
	lw	a5,-20(s0)
	mv	a0,a5
	ld	ra,40(sp)
	ld	s0,32(sp)
	addi	sp,sp,48
	jr	ra
	.size	sum, .-sum
	.section	.rodata
	.align	3
.LC2:
	.string	"sum(10)=%d\n"
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-16
	sd	ra,8(sp)
	sd	s0,0(sp)
	addi	s0,sp,16
	li	a0,10
	call	sum
	mv	a5,a0
	mv	a1,a5
	lui	a5,%hi(.LC2)
	addi	a0,a5,%lo(.LC2)
	call	printf
	li	a5,0
	mv	a0,a5
	ld	ra,8(sp)
	ld	s0,0(sp)
	addi	sp,sp,16
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 7.2.0"
