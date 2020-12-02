#ifndef UMEXEC_DEFINED
#define UMEXEC_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <seq.h>
#include <bitpack.h>

typedef uint32_t address;
typedef uint32_t word;

//initializes segment the with size elements equal to 0
//void init_seg(Seq_T segment, uint32_t size); 

//unpack words
// unsigned unpack_value(word op, unsigned reg);

// uint32_t three_registers(unsigned opcode, uint32_t rb, uint32_t rc);
void execute_prog(Seq_T program); //Initializes machine then executes program

#endif