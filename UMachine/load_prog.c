#include "./load_prog.h"

void load_prog(FILE *UMprog){
    Seq_T prog = Seq_new(0);
    uint32_t command;
    while( !feof(UMprog) )
    {
        int size = fread(&command, 4, 1, UMprog);
        if( !size ) break;
        Seq_addhi(prog, (void *)(uintptr_t)command);
    }

    if( Seq_length(prog) == 0 ) 
    {
        printf("Incorrect program format.\n");
        printf("UM expects a sequence of 32-bit words.\n");
        exit(2);
    }

    execute_prog( prog );
    Seq_free( &prog );
}