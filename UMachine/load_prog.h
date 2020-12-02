#ifndef LOAD_DEFINED
#define LOAD_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <seq.h>
#include <assert.h>
#include "./UM_Exec.h"

typedef uint32_t Um_instruction;

// Reads 32-bit commands from file in big-endian byte order
void load_prog(FILE *UMprog); 

#endif
