#ifndef UMEXEC_DEFINED
#define UMEXEC_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <seq.h>
#include <bitpack.h>

typedef uint32_t address;

void init_seg(Seq_T segment, int size);
void execute_prog(Seq_T program);

#endif