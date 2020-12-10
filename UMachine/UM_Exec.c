#include "./UM_Exec.h"

static inline unsigned unpack_value(word op, unsigned reg){
    unsigned width = (reg == OP) ? 4 : 3;        //opcodes are 4 bits reg identifiers are 3
    unsigned shiftL = 32 - (width + reg);
    unsigned shiftR = 32 - width;
    return (unsigned)( (op<<shiftL)>>shiftR );
}

void execute_prog(uint32_t* program){
    //Initialize machine
    uint32_t *r = (uint32_t *)calloc(8, sizeof(uint32_t));    // 8 general purpose registers
    uint32_t **m = (uint32_t **)calloc(1, sizeof(uint32_t*)); // Segmented mapped memory
    m[0] = program;                              // Program segment
    uint32_t numSegments = 1;                    // Size of segment array   
    Seq_T um = Seq_new(0);                       // Unmapped memory address'
    address ct = 0;                              // Program counter
    
    //Execute program
    while( TRUE )
    {
        word cmd = m[0][ct++];
        unsigned opcode = unpack_value(cmd, OP);
        unsigned ra = unpack_value(cmd, A);
        unsigned rb = unpack_value(cmd, B);
        unsigned rc = unpack_value(cmd, C);

        switch (opcode) {
        case CMOV: r[ra] = (r[rc] != 0) ? r[rb] : r[ra];
            break;
        case SLOAD: r[ra] = m[r[rb]][r[rc]];   
            break;
        case SSTORE: m[r[ra]][r[rb]] = r[rc]; 
            break;
        case  ADD: r[ra] = (r[rb] + r[rc]);
            break;
        case MULT: r[ra] = (r[rb] * r[rc]);
            break;
        case  DIV: r[ra] = (r[rb] / r[rc]);
            break;
        case NAND: r[ra] = ~(r[rb] & r[rc]);
            break;
        case HALT: exit(0);

        case OUT:
            assert( r[rc] < 266); 
            putchar( r[rc] );
            break;

        case IN: ;
            char c = getchar();
            if (c == EOF) {
                r[rc] = 0;
                r[rc] = ~r[rc];              //If end of input set every bit in r[rc] to 1
            }
            else r[rc] = (uint32_t)c;
            break;
        
        case MSEG: ;
            uint32_t *segment = calloc(r[rc], sizeof(uint32_t));
            if ( Seq_length(um) == 0 ) {    
                r[rb] = numSegments;
                m = realloc(m, sizeof(uint32_t*)*(r[rb]+1));
                m[r[rb]] = segment;
                numSegments++;     
            }
            else {
                address adr = (address)(uintptr_t)Seq_remlo(um);
                r[rb] = adr;
                m[adr] = segment;
            }
            break;

        case UMSEG: ;
            free( m[r[rc]] );
            address uadr = r[rc];
            Seq_addlo(um, (void *)(uintptr_t)uadr);
            break;

        case LPROG: ;
            if (r[rb] != 0) {
                free( m[0] );        // Free current program
                m[0] = m[r[rb]];
            }
            ct = r[rc];
            break;

        case LVAL: ;
            unsigned ral = unpack_value(cmd, AL);
            uint32_t val = (cmd<<7)>>7;      // extract value
            r[ral] = val;                    // load extracted value
            break;
           
        default:                             // If immproper Commands are suppplied notify user
            printf("Command #%u did not execute. opcode: %u\n", (unsigned)ct, opcode);
            printf("Check formatting and operation specs\n");
            break;
        }
    }
}