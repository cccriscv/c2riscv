#ifndef __X86_H__
#define __X86_H__

#include <stdlib.h>
#include "../util/strTable.h"
#include "../util/util.h"
// #include "vm.h"


// extern int argIdx;

extern FILE *sFile;

#define xEmit(...) do { fprintf(sFile, __VA_ARGS__); } while (0)

extern void xCode(char *op, char *_d, char *_p1, char *_p2, char *x);

#endif