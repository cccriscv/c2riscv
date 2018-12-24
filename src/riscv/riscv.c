#include "x86.h"

void as(char *x, char *rx) {
  int i = -1;
  if (isdigit(*x)) { // Number
    sprintf(rx, "$%s", x);
  } else if (*x == '_') { // Global
    sprintf(rx, "%s", x);
  } else if (*x == 'L') { // Local
    sscanf(x, "L%d", &i);
    sprintf(rx, "-%d(%%ebp)", (i+1)*4);
  } else if (*x == 'P') { // Param 參考: https://eli.thegreenplace.net/2011/02/04/where-the-top-of-the-stack-is-on-x86
    sscanf(x, "P%d", &i);
    sprintf(rx, "%d(%%ebp)", (i+2)*4); // 0: saved ebp, 1: return address, 2: 第一個參數
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
    xEmit("%s:\n", d);
  } else if (strcmp(op, "file") == 0) {
    xEmit("	.file %s\n", d);
    xEmit("	.option nopic\n\t.section \t.rodata\n\t.align 3");
  } else if (strcmp(op, "-file") == 0) {
    xEmit("	.ident	\"c0c: 0.0.1\"\n");
  } else if (strcmp(op, "global") == 0) {
    xEmit("	.comm	%s, 4, 2\n", d);
  } else if (strcmp(op, "jmp") == 0) {
    xEmit("	j	%s\n", d);
  } else if (strcmp(op, "call") == 0) {
    xEmit("	call %s\n	ld a0, %s\n", p1, d);
  } else if (strcmp(op, "arg") == 0) {
    int argIdx = atoi(x);
    xEmit("	ld a%d %s\n", argIdx, d);
  } else if (strcmp(op, "str") == 0) {
    char literal[SMAX];
    sscanf(_p1, "\"%[^\"]*\"", literal);
    xEmit("	.section .rodata\n\t.align 3\n%s:\n\t.string \"%s\"\n	.text\n", d, literal);
  } else if (strcmp(op, "jz") == 0) {
    xEmit("	ld a4,%s\n	beq	a4,zero,%s\n", p1, d);
  } else if (strcmp(op, "jnz") == 0) {
    xEmit("	ld a4,%s\n	bne	a4,zero,%s\n", p1, d);
  } else if (strcmp(op, "++")==0) {
    char *sizeStr = (strcmp(x, "char*") == 0) ? "1" : "4";
    xCode("+", d, d, sizeStr, x);
  } else if (strcmp(op, "--")==0) {
    char *sizeStr = (strcmp(x, "char*") == 0) ? "1" : "4";
    xCode("-", d, d, sizeStr, x);
  } else if (*op=='=') {
    xEmit("	ld a4,%s\n	sd	a4, %s\n", p1, d);
  } else if (strcmp(op, "function")==0) {
    xEmit("	.text\n\t.align 1\n	.globl	_%s\n	.type	%s, @function\n_%s\n", d, d, d);
    xEmit("	pushl	%%ebp\n	movl	%%esp, %%ebp\n");
    int frameSize = atoi(x);
    xEmit("	addi	sp,sp,%d\n", frameSize*8);
    xEmit("\tsd\tra, %d(sp)\n\tsd\ts0,%d(sp)\naddi\ts0,sp,%d\n", (frameSize-1)*8, (frameSize-2)*8, frameSize*8); 
    // if (strcmp(d, "main") == 0) { xEmit("	call	___main\n"); }
  } else if (strcmp(op, "return") == 0) {
    xEmit("\tld\tra,%d(sp)\n,\tld\ts0,%d(sp)\n\tld a0, %s\n\tjr ra\n", (frameSize-1)*8, (frameSize-2)*8, d);
  } else if (strcmp(op, "-function") == 0) {
    xCode("return");
    xEmit("\t.size\t%s, .-%s\n", d, d);
  } else if (strcmp(op, "param")==0) {
  } else if (strcmp(op, "local") == 0) {
  } else if (strcmp(op, "extern") == 0) {
  } else if (isMember(op, "~ + - * / & | ^ && || [] ptr*")) {
    xEmit("	ld	a4,%s\n", p1);
    switch (*op) {
      // uniary operator
      case '~': xEmit("	not	a4,a4\n"); break;
      case 'p': xEmit("	?? movl	(%%eax), %%eax\n"); break; // ptr*
      // binary operator
      case '+': xEmit("	add	a4, %s\n", p2); break;
      case '-': xEmit("	sub	a4, %s\n", p2); break;
      case '*': xEmit("	mul	a4, %s\n", p2); break;
      case '/': xEmit("	div	a4, %s\n", p2); break; // ??　	movl	_a, %eax	movl	_b, %ecx	cltd	idivl	%ecx	 # D.1567
      case '&': xEmit("	and	a4, %s\n", p2); break;
      case '|': xEmit("	orl	a4, %s\n", p2); break;
      case '^': xEmit("	xor	a4, %s\n", p2); break;
      case '[': 
        xEmit("	add	a4,%s\n", p2);
        if (strcmp("int*", x)==0) xEmit("	shl	3, a4\n"); // int* a[i] => a[i*4] => a[i<<2]
        break;
    }
    xEmit("	sd	a4, %s\n", d);
  } else if (isMember(op, "< > == != >= <=")) {
    xEmit("	ld a4,%s\n	ldl a5, %s\n", p1, p2); // cmpl %%ebx, %%eax 是 eax-ebx
    if (strcmp(op, "==")==0) xEmit("	??sete %%al");
    if (strcmp(op, "!=")==0) xEmit("	??setne %%al");
    if (strcmp(op, "<")==0) xEmit("	??setl %%al");
    if (strcmp(op, "<=")==0) xEmit("	??setle %%al");
    if (strcmp(op, ">")==0) xEmit("	??setg %%al");
    if (strcmp(op, ">=")==0) xEmit("	??setge %%al");
    xEmit("	sd a4,%s\n", d); // 不能用 movzbl, 應該用 movsbl (move and sign extend)
  } else {
    xEmit(" --- not handle yet ! ----\n");
  }
}
