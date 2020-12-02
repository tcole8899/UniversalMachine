#include "./UM_Exec.h"
static inline void init_seg(Seq_T segment, uint32_t size){
    for(uint32_t i = 0; i < size; i++){
        Seq_addhi(segment, (void *)(uintptr_t)(uint32_t)0);
    }
}

static inline void dup_seg(Seq_T progSegment, Seq_T dupSegment){
    unsigned length = Seq_length(dupSegment);
    for(unsigned i = 0; i < length; i++){
        Seq_addhi(progSegment, Seq_get(dupSegment, i));
    }
}

static inline unsigned unpack_value(word op, unsigned reg){
    unsigned width = (reg == OP) ? 4 : 3;        //opcodes are 4 bits reg identifiers are 3
    unsigned shiftL = 32 - (width + reg);
    unsigned shiftR = 32 - width;
    return (unsigned)( (op<<shiftL)>>shiftR );
}

void execute_prog(Seq_T program){
    //Initialize machine
    uint32_t *r;
    r = (uint32_t *)calloc(8, sizeof(uint32_t)); //8 general purpose registers
    for(int i = 0; i < 8; i++) { r[i] = 0; }
    Seq_T m = Seq_new(0);                        //Segmented mapped memory
    Seq_addhi(m, (void *)program); 
    Seq_T um = Seq_new(0);                       //Unmapped memory address'
    address ct = 0;                              //Program counter

    //Execute program
    while( TRUE )
    {
        Seq_T prog = Seq_get(m, 0);
        word cmd = (word)(uintptr_t)Seq_get(prog, ct++);
        unsigned opcode = unpack_value(cmd, OP);
        unsigned ra = unpack_value(cmd, A);
        unsigned rb = unpack_value(cmd, B);
        unsigned rc = unpack_value(cmd, C);

        switch (opcode) {
        case CMOV: r[ra] = (r[rc] != 0) ? r[rb] : r[ra];
            break;

        case SLOAD:                          //loads m[r[rb]][r[rc]] into r[ra]
            r[ra] = (uint32_t)(uintptr_t)Seq_get((Seq_T)Seq_get(m, r[rb]), r[rc]); 
            break;

        case SSTORE:
            Seq_put(
                (Seq_T)Seq_get(m, r[ra]),    //Segment identified by register A
                r[rb],                       //Location in segment 
                (void *)(uintptr_t)r[rc]);   //Value to store
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
            Seq_T seg = Seq_new(0);
            init_seg(seg, r[rc]);            //Initialize segment with r[rc] elements set to 0
            if ( Seq_length(um) == 0 ) {    
                r[rb] = (uint32_t) Seq_length(m);
                Seq_addhi(m, seg);     
            }
            else {
                address adr = (address)(uintptr_t)Seq_remlo(um);
                r[rb] = adr;
                Seq_put(m, adr, seg);
            }
            break;

        case UMSEG: ;
            Seq_T useg = (Seq_T)Seq_get(m, r[rc]);
            Seq_free( &useg );
            address uadr = r[rc];
            Seq_addlo(um, (void *)(uintptr_t)uadr);
            break;

        case LPROG: ;
            if (r[rb] != 0) {
                Seq_T progSeg = (Seq_T)Seq_get(m, 0);
                Seq_free( &progSeg );        // Free current program
                Seq_T dupSeg = Seq_get(m, r[rb]);
                Seq_T newSeg = Seq_new(0);
                dup_seg(newSeg, dupSeg);     // Duplicate program at m[r[rb]]
                Seq_remlo(m); Seq_addlo(m, newSeg);
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