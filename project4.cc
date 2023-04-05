#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include "compiler.h"
#include "lexer.h"

LexicalAnalyzer lexer;



struct InstructionNode * parse_generate_intermediate_representation() {
    //assign each variable and constant a memory address and store that address in a variable
    // Assigning location for variable "a"
    // int address_a = next_available;
    // mem[next_available] = 0;
    // next_available++;

    //input instruction for variable a
    // i1->type = IN;                                      
    // i1->input_inst.var_index = address_a;
    // i1->next = i2;

    
}