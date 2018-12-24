#ifndef __RISCV_H__
#define __RISCV_H__

#include <stdlib.h>
#include "../util/strTable.h"
#include "../util/util.h"

extern FILE *sFile;

#define vEmit(...) do { fprintf(sFile, __VA_ARGS__); } while (0)

extern void vCode(char *op, char *_d, char *_p1, char *_p2, char *x);

#endif
