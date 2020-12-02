#include "./UM_Exec.h"

enum reg { C=0, B=3, A=6, AL=25, OP=28 }; 
enum TF {FALSE=0, TRUE};
enum opcode { CMOV=0, SLOAD, SSTORE, ADD, MULT, DIV, NAND,
              HAULT, MSEG, UMSEG, OUT, IN, LPROG, LVAL};

static inline void init_seg(Seq_T segment, uint32_t size){
    uint32_t init_val = 0;
    for(uint32_t i = 0; i < size; i++){
        Seq_addhi(segment, (void *)(uintptr_t)init_val);
    }
}

static inline unsigned unpack_value(word op, unsigned reg){
    unsigned width = (reg == OP) ? 4 : 3;
    return (unsigned)Bitpack_getu(op, width, reg);
}

static inline uint32_t three_registers(unsigned opcode, uint32_t rb, uint32_t rc) {
    uint64_t mod = 1; mod = mod<<32;             // Mod = 2^32
    switch (opcode) {
    case ADD: return (rb + rc) % mod;
    case MULT: return (rb * rc) % mod;
    case DIV: return (rb / rc);
    case NAND: return ~(rb & rc);
    }
    printf("Improper use of function 'three_registers'.\n");
    printf("Expects 3 <= opcode <= 6. Got: %u\n", (opcode));
    exit(3);
}

void execute_prog(Seq_T program){
    //Initialize machine
    uint32_t *r;
    r = (uint32_t *)calloc(8, sizeof(uint32_t)); //8 general purpose registers
    Seq_T m = Seq_new(0);                        //Segmented mapped memory
    Seq_addhi(m, program); 
    Seq_T um = Seq_new(0);                       //Unmapped memory address'
    address ct = 0;                              //Program counter
    (void)um;

    //Execute program
    while( TRUE )
    {
        word cmd = (word)(uintptr_t)Seq_get(program, ct++);
        unsigned opcode = unpack_value(cmd, OP);
        unsigned ra = unpack_value(cmd, A);
        unsigned rb = unpack_value(cmd, B);
        unsigned rc = unpack_value(cmd, C);
        switch (opcode) {
        case CMOV:
        case NAND:
            switch (opcode) {
            case CMOV:
                r[ra] = (r[rc] != 0) ? r[rb] : r[ra];
                break;
            case SLOAD:
                r[ra] = (uint32_t)(uintptr_t)Seq_get((Seq_T)Seq_get(m, r[rb]), r[rc]);
                break;
            case SSTORE:
                Seq_put(
                    (Seq_T)Seq_get(m, r[ra]),    //Segment identified by register A
                    r[rb],                       //Location in segment identified by reg A
                    (void *)(uintptr_t)r[rc]     //Value to store
                );
                break;
            case ADD:
            case NAND:
                r[ra] = three_registers(opcode, r[rb], r[rc]);
                break;
            }
            break;

        case HAULT: exit(0);

        case OUT: 
            putchar( r[unpack_value(cmd, C)] );
            break;
        
        case MSEG: ;
            Seq_T seg = Seq_new(0);
            init_seg(seg, r[rc]);
            if ( Seq_length(um) == 0 ) {
                Seq_addhi(m, seg);
            }
            else { 
                address adr = (address)(uintptr_t)Seq_remlo(um);
                Seq_put(m, adr, seg);
            }
            break;

        case LVAL: ;
            unsigned ral = unpack_value(cmd, AL);
            cmd = (cmd<<7)>>7;                            //CHANGE
            r[ral] = cmd;
            break;
           
        default:
            printf("Command #%u did not execute\n", (unsigned)ct);
            printf("Check formatting and machine specs\n");
            break;
        }
    }
    
}
