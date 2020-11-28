#ifndef UMEXEC_DEFINED
#define UMEXEC_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <seq.h>
#include <bitpack.h>
#include <assert.h>

typedef uint32_t address;
typedef uint32_t word;

enum reg { C=0, B=3, A=6, AL=25, OP=28 };                   //LSB in specified register
enum TF {FALSE=0, TRUE};
enum opcode { CMOV=0, SLOAD, SSTORE, ADD, MULT, DIV, NAND,  //operation code values
              HALT, MSEG, UMSEG, OUT, IN, LPROG, LVAL};

void execute_prog(Seq_T program);                           //Initializes machine then executes program

#endif