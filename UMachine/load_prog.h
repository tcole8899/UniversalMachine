#ifndef LOAD_DEFINED
#define LOAD_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <seq.h>
#include <assert.h>

typedef uint32_t Um_instruction;

void add_inst(Seq_T program, Um_instruction inst);
void launch_prog(Seq_T program); // Calls UM_Exec to run with the program
void load_prog(FILE *UMprog);

#endif
