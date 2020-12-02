I worked on this project myself. I spent about 2 hours analyzing the problem, 6 hours making the design, and about 15 hours
implementing and testing my solution. I used Hanson's documentation on the sequence library, and a binary <-> decimal calculator as references.

This implementation takes 1.88 seconds for 50,000,000 instructions. To find this I used \time with midmark.um, which has 
85,070,522 commands, and got a result of 3.2 seconds.
    - (50,000,000/85,070,522) * 3.2 = 1.88 seconds.

Segment Representation and Invariants
    - The segmented memory is represented as a sequence, 'm', of sequences.
    - A seperate sequence, 'um', is provided for unmapped segment IDs.
        - A segment is accessed with a Seq_get(<segment sequence>, <32-bit ID>).
            - A memory address within a segment is accessed with Seq_get(<value sequence>, <32-bit ID>).
    - Seq_length(m) == 1 + (<number of calls to map segment> - <number of calls to unmap segment>).
    - Hanson's seqence library helps in converting between 32-bit IDs and 64-bit pointers.
        - Memory segment 'i' = Seq_get(m, i).
        - Element 'i, j' = Seq_get( Seq_get(m, i), j )
        ** i < 0xFFFFFFFF && j < 0xFFFFFFFF && i < Seq_length(m) && j < Seq_length( Seq_get(m, i) ) ** 
 
UM Architecture
    um
    - Checks that file exits and that it is a proper .um file.
        - Prints error and exits if not
    - Calls "load_program" passing it a pointer to a file containing the program.

    load_prog 
    - This module reads 32 bit 'words' from a file placing them into a sequence.
        - The word is read with 4 calls to getc() which returns 8 bits each call.
        - The word is packed into a command variable in big edian byte order.
            - Result from getc() gets left shifted appropriately.
            - Command gets set to "Command bitwise-or Shifted Result". Placing the 8 bits in the command.
    - Once the end of the file is reached, load_prog passes on the 'loaded program' to "UM_Exec".
    - If the sequence does not have any elements after reading the file an error message is sent and the program exits.

    UM_Exec
    - init_seg
        - Takes a sequence and a integer, size, as arguments.
        - Pushes 0 to the sequence 'size' number of times.
    - dup_seg
        - Takes 2 sequences, one to copy from and one to copy to.
        - Loops through 'copy from' sequence placing values in 'copy to'.
    - unpack_value
        - Takes a 32-bit word and the location of a register/operation identifier in that word.
        - Uses bitwise shift operations to return appropriate 3 or 4 bit value.
    - execute_prog
        - Initializes the program
            - 8 general purpose registers  'uint32_t r[8]'
            - mapped memory segment        'Seq_t m'
            - unmapped segment identifiers 'Seq_t um'
            - program counter              'uint32_t ct'
        - Executes program
            - Gets the command from the program sequence
            - increments the program counter
            - unpacks the operation code and register identifiers A, B, and C
            - 14 operations
                - conditional move -> if regC != 0 then regA = regB
                - Segment Load     -> regA = m[regB][regC]
                - Segment Store    -> m[regA][regB] = regC
                - Add              -> regA = regB + regC
                - Multiply         -> regA = regB * regC
                - Divide           -> regA = regB / regC
                - NAND             -> regA = ~(regB & regC)
                - Halt             -> exits program
                - Out              -> prints character from regC
                - In               -> loads character from stdin to regC
                - Map Segment      -> Initializes segment with a size specified by regC to 0
                                        Reuses unmapped segment identifiers if any are available
                                        Places the new segment identifier in regB
                - Unmap Segment    -> Frees the segment specified by regC and stores the
                                        corresponding identifier in the unmapped seg ID sequence
                - Load Program     -> Sets program counter to the value specified in regC
                                        If regB is not 0 the segment identified by regB is
                                        duplicated and replaces the program segment.
                - Load Value       -> Loads a 25 bit value into a specified register.
                                        This register identifier has a lsb of 26.