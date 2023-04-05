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


void parseCondition() {
    //primary relop primary
}

void parseExpression() {
    //primary op primart
}

//parsers for each rule in the grammar
void parseAssignment() {
    // ID EQUAL primary SEMICOLON

    // ID EQUAL expr SEMICOLON
}

void parseOutput() {
    //consume OUTPUT token
    
    //make output instruction for the variable

    //consume SEMICOLON
}

void parseInput() {
    //consume OUTPUT token
    
    //make input instruction for the variable

    //consume SEMICOLON
}

void parseWhile() {
    //consume WHILE

    parseCondition();

    parseBody();
}

void parseIf() {

}

void parseCase() {

}

void parseCaseList() {
    //repeatedly call parseCase for all cases in the switch statement
}

//translate it to a sequence of IF statements
void parseSwitch() {
    //take care of switch statements with and without default case
}

//translate it to a WHILE loop
void parseFor() {

}

void parseInputs() {
    //go thorugh the int list at the end of the input file and add each one in order to input vector
}

void parseBody() {
    //consume LBRACE

    parseStatementList();

    //consume RBRACE
}

//assigns each variable a space in mem using next_available as index
void parseIdList() {
    next_available = 0;
}

void parseVarSection() {
    parseIdList();

    //consume SEMICOLON
}

void parseStatement() {
    //assignment

    //while

    //if

    //switch

    //for

    //input

    //output
}

void parseStatementList() {
    //call parseStatement() for all statements in the list
}

void parseProgram() {
    parseVarSection();

    parseBody();

    parseInputs();
}

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

    //output
    //  int var_index, index in mem of the variable to print out

    //assignment
    //     int left_hand_side_index;
    //     int operand1_index;
    //     int operand2_index;
    //     ArithmeticOperatorType op; // operator, If op == OPERATOR_NONE then only operand1 is meaningful.
    //     struct AssignmentStatement* next;

    //expression
    //boolean condition
    //if
    //while
    //for, translate to a while loop
    //switch, translate to a sequence of if statements followed by a goto label

}
