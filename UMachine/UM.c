#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"

#include "load_prog.h"

int main(int argc, char *argv[]){
    assert(argc == 2);

    char *ext = strrchr(argv[1], '.');
    FILE *fp = fopen(argv[1], "r");
    if ( fp == NULL ) 
    {
        printf("Could not open file: %s\n", argv[1]);
        exit(1);
    }

    else if ( strncmp(ext, ".um", 2) != 0 ) // Ensure proper um file is provided
    { 
        printf("Expected a valid .um file. Got: %s\n", argv[1]); 
        exit(1);
    }

    load_prog( fp ); // If the .um file exists load and execute the program
    return 0;
}