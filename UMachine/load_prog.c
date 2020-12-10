#include "./load_prog.h"

void load_prog(FILE *UMprog){
    uint32_t *prog = (uint32_t*)calloc(1, sizeof(uint32_t));
    uint32_t command;
    uint32_t index = 0;
    while( !feof(UMprog) )
    {
        prog = (index == 0) ? prog : realloc(prog, (index + 1)*sizeof(uint32_t));
        command = 0;
        command |= getc(UMprog) << 24;               //
        command |= getc(UMprog) << 16;               // Pack command in Big-endian byte order
        command |= getc(UMprog) << 8;                //
        command |= getc(UMprog);
        prog[index] = command;
        index++;
        
    }

    if( index == 0 ) 
    {
        printf("Incorrect program format.\n");
        printf("UM expects a sequence of 32-bit words.\n");
        exit(2);
    }

    execute_prog( prog );
    free(prog);
}