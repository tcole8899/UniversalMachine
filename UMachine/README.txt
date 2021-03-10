Segment Representation and Invariants
    - The segmented memory is represented as an array, 'm', of arrays.
    - A seperate sequence, 'um', is provided for unmapped segment IDs.
        - A segment is accessed with a Seq_get(<segment sequence>, <32-bit ID>).
            - A memory address within a segment is accessed with Seq_get(<value sequence>, <32-bit ID>).
    - length(m) == 1 + (<number of calls to map segment> - <number of calls to unmap segment>).
    - Since this is a virtual 32-bit machine the maximum number of segments and the maximum segment size is 2,147,483,647.
        - Memory segment 'i' = m[i]
        - Element 'i, j' = m[i][j]
        ** i < 0xFFFFFFFF && j < 0xFFFFFFFF && i < lenght(m) && j < length( m[i] ) ** 
 
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
