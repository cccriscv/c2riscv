#include "riscv.h"

void as(char *x, char *rx) {
  int i = -1;
  if (isdigit(*x)) { // Number
    sprintf(rx, "$%s", x);
  } else if (*x == '_') { // Global
    sprintf(rx, "%s", x);
  } else if (*x == 'L') { // Local
    sscanf(x, "L%d", &i);
    sprintf(rx, "-%d(%%ebp)", (i+1)*8);
  } else if (*x == 'P') { // Param 參考: https://eli.thegreenplace.net/2011/02/04/where-the-top-of-the-stack-is-on-x86
    sscanf(x, "P%d", &i);
    sprintf(rx, "%d(%%ebp)", (i+2)*8); // 0: saved ebp, 1: return address, 2: 第一個參數
  } else { // LABEL, ....
    sprintf(rx, "%s", x);
  }
}

// Intel 指令集參考 -- https://web.itu.edu.tr/kesgin/mul06/intel/index.html
// https://www.aldeid.com/wiki/X86-assembly/Instructions
// 注意: GNU 語法是目標在後的，所以 cmpl %eax, %ebx 會造成 (%ebx-%eax) 狀況的旗標改變。
void vCode(char *op, char *_d, char *_p1, char *_p2, char *x) {
  char d[SMAX], p1[SMAX], p2[SMAX];
  as(_d, d); as(_p1, p1); as(_p2, p2);
  if (strcmp(op, "label") == 0) {
    vEmit("%s:\n", d);
  } else if (strcmp(op, "file") == 0) {
    vEmit("	.file %s\n", d);
    vEmit("	.option nopic\n	.section 	.rodata\n	.align 3");
  } else if (strcmp(op, "-file") == 0) {
    vEmit("	.ident	\"c0c: 0.0.1\"\n");
  } else if (strcmp(op, "global") == 0) {
    vEmit("	.comm	%s, 4, 2\n", d);
  } else if (strcmp(op, "jmp") == 0) {
    vEmit("	j	%s\n", d);
  } else if (strcmp(op, "call") == 0) {
    vEmit("	call %s\n	ld a0, %s\n", p1, d);
  } else if (strcmp(op, "arg") == 0) {
    int argIdx = atoi(x);
    vEmit("	ld a%d %s\n", argIdx, d);
  } else if (strcmp(op, "str") == 0) {
    char literal[SMAX];
    sscanf(_p1, "\"%[^\"]*\"", literal);
    vEmit("	.section .rodata\n	.align 3\n%s:\n	.string \"%s\"\n	.text\n", d, literal);
  } else if (strcmp(op, "jz") == 0) {
    vEmit("	ld a4,%s\n	beq	a4,zero,%s\n", p1, d);
  } else if (strcmp(op, "jnz") == 0) {
    vEmit("	ld a4,%s\n	bne	a4,zero,%s\n", p1, d);
  } else if (strcmp(op, "++")==0) {
    char *sizeStr = (strcmp(x, "char*") == 0) ? "1" : "4";
    xCode("+", d, d, sizeStr, x);
  } else if (strcmp(op, "--")==0) {
    char *sizeStr = (strcmp(x, "char*") == 0) ? "1" : "4";
    xCode("-", d, d, sizeStr, x);
  } else if (*op=='=') {
    vEmit("	ld a4,%s\n	sd	a4, %s\n", p1, d);
  } else if (strcmp(op, "function")==0) {
    vEmit("	.text\n	.align 1\n	.globl	_%s\n	.type	%s, @function\n_%s\n", d, d, d);
    vEmit("	pushl	%%ebp\n	movl	%%esp, %%ebp\n");
    int frameSize = atoi(x);
    vEmit("	addi	sp,sp,%d\n", frameSize*8);
    vEmit("	sd	ra, %d(sp)\n	sd	s0,%d(sp)\naddi	s0,sp,%d\n", (frameSize-1)*8, (frameSize-2)*8, frameSize*8); 
    // if (strcmp(d, "main") == 0) { vEmit("	call	___main\n"); }
  } else if (strcmp(op, "return") == 0) {
    vEmit("	ld	ra,%d(sp)\n,	ld	s0,%d(sp)\n	ld a0, %s\n	jr ra\n", (frameSize-1)*8, (frameSize-2)*8, d);
  } else if (strcmp(op, "-function") == 0) {
    xCode("return");
    vEmit("	.size	%s, .-%s\n", d, d);
  } else if (strcmp(op, "param")==0) {
  } else if (strcmp(op, "local") == 0) {
  } else if (strcmp(op, "extern") == 0) {
  } else if (isMember(op, "~ + - * / & | ^ && || [] ptr*")) {
    vEmit("	ld	a4,%s\n", p1);
    switch (*op) {
      // uniary operator
      case '~': vEmit("	not	a4,a4\n"); break;
      case 'p': vEmit("	?? movl	(%%eax), %%eax\n"); break; // ptr*
      // binary operator
      case '+': vEmit("	add	a4, %s\n", p2); break;
      case '-': vEmit("	sub	a4, %s\n", p2); break;
      case '*': vEmit("	mul	a4, %s\n", p2); break;
      case '/': vEmit("	div	a4, %s\n", p2); break; // ??　	movl	_a, %eax	movl	_b, %ecx	cltd	idivl	%ecx	 # D.1567
      case '&': vEmit("	and	a4, %s\n", p2); break;
      case '|': vEmit("	orl	a4, %s\n", p2); break;
      case '^': vEmit("	xor	a4, %s\n", p2); break;
      case '[': 
        vEmit("	add	a4,%s\n", p2);
        if (strcmp("int*", x)==0) vEmit("	shl	3, a4\n"); // int* a[i] => a[i*4] => a[i<<2]
        break;
    }
    vEmit("	sd	a4, %s\n", d);
  } else if (isMember(op, "< > == != >= <=")) {
    vEmit("	ld a4,%s\n	ldl a5, %s\n", p1, p2); // cmpl %%ebx, %%eax 是 eax-ebx
    if (strcmp(op, "==")==0) vEmit("	??sete %%al");
    if (strcmp(op, "!=")==0) vEmit("	??setne %%al");
    if (strcmp(op, "<")==0) vEmit("	??setl %%al");
    if (strcmp(op, "<=")==0) vEmit("	??setle %%al");
    if (strcmp(op, ">")==0) vEmit("	??setg %%al");
    if (strcmp(op, ">=")==0) vEmit("	??setge %%al");
    vEmit("	sd a4,%s\n", d); // 不能用 movzbl, 應該用 movsbl (move and sign extend)
  } else {
    vEmit(" --- not handle yet ! ----\n");
  }
}
