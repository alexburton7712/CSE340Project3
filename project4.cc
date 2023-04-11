#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include "compiler.h"
#include "lexer.h"

LexicalAnalyzer* lexer = new LexicalAnalyzer();

std::vector<InstructionNode*> instructionList;
std::string address[1000];

InstructionNode* current = instructionList[0];

bool isPrimary(int index){
    // primary --> ID | NUM
    if (lexer->peek(index).token_type == ID | lexer->peek(index).token_type == NUM){
        return true;
    }
    return false;
}

bool isRelop(int index){
    if (lexer->peek(index).token_type == GREATER | 
    lexer->peek(index).token_type == LESS | 
    lexer->peek(index).token_type == NOTEQUAL){
        return true;
    }
    return false;
}

bool isOp(int index){
    if (lexer->peek(index).token_type == PLUS | 
    lexer->peek(index).token_type == MINUS | 
    lexer->peek(index).token_type == MULT | 
    lexer->peek(index).token_type == DIV){
        return true;
    }
    return false;
}

bool isCondition(int index){
    //condition --> primary relop primary
    if (isPrimary(index) && isRelop(index+1) && isPrimary(index+2)){
        return true;
    }
    return false;
}

bool isExpression(int index){
    //expression --> primary op primary
    if (isPrimary(index) && isOp(index+1) && isPrimary(index+2)){
        return true;
    }
    return false;
}

std::vector<Token> parseCondition() {
    //primary relop primary
    std::vector<Token> condition;

    //primary
    if (isCondition(1)){

        //primary
        condition.push_back(lexer->GetToken());
        address[next_available] = condition[next_available].lexeme;
        mem[next_available] = 0;
        next_available++;

        //relop 
        condition.push_back(lexer->GetToken());
        address[next_available] = condition[next_available].lexeme;
        mem[next_available] = 0;
        next_available++;

        //primary
        condition.push_back(lexer->GetToken());
        address[next_available] = condition[next_available].lexeme;
        mem[next_available] = 0;
        next_available++;
    }
    return condition;
    
}

std::vector<Token> parseExpression() {
    //primary op primary
    std::vector<Token> expression;

    if (isExpression(1)){

        //add to address and mem?
        expression.push_back(lexer->GetToken());
        address[next_available] = expression[next_available].lexeme;
        mem[next_available] = 0;
        next_available++;

        expression.push_back(lexer->GetToken());
        address[next_available] = expression[next_available].lexeme;
        mem[next_available] = 0;
        next_available++;

        expression.push_back(lexer->GetToken());
        address[next_available] = expression[next_available].lexeme;
        mem[next_available] = 0;
        next_available++;
    }
    return expression;
}

//parsers for each rule in the grammar
std::vector<Token> parseAssignment() {
    
    // ID EQUAL expr SEMICOLON
    // ID EQUAL primary SEMICOLON
    std::vector<Token> assignment;
    
    if (lexer->peek(1).token_type == ID){
        if (lexer->peek(2).token_type == EQUAL){

            if (isPrimary(3)){
                //the ID
                assignment.push_back(lexer->GetToken());
                address[next_available] = assignment[next_available].lexeme;
                mem[next_available] = 0;
                next_available++;

                //the equal
                assignment.push_back(lexer->GetToken());
                address[next_available] = assignment[next_available].lexeme;
                mem[next_available] = 0;
                next_available++;

                //primary (ID or NUM)
                assignment.push_back(lexer->GetToken());
                address[next_available] = assignment[next_available].lexeme;
                mem[next_available] = 0;
                next_available++;
            }
            else if (isExpression(3)){
                //the ID
                assignment.push_back(lexer->GetToken());
                address[next_available] = assignment[next_available].lexeme;
                mem[next_available] = 0;
                next_available++;

                //the equal
                assignment.push_back(lexer->GetToken());
                address[next_available] = assignment[next_available].lexeme;
                mem[next_available] = 0;
                next_available++;

                //the expression
                std::vector<Token> expression = parseExpression();
                for (int i = 0; i < expression.size(); i++){
                    assignment.push_back(expression[i]);
                }
                address[next_available] = assignment[next_available].lexeme;
                mem[next_available] = 0;
                next_available++;
            }
        }
    }
    lexer->GetToken();
    return assignment;
}

void parseOutput() {
    //output --> output ID SEMICOLON
    //consume OUTPUT token
    lexer->GetToken();
    
    //make output instruction for the variable
    InstructionNode* output = new InstructionNode();
    output->output_inst.var_index = indexOfToken(lexer->GetToken().lexeme);
    current->next = output;
    output->next = NULL;
    current = output;

    //consume SEMICOLON
    lexer->GetToken();
}

void parseInput() {
    //input --> INPUT ID SEMICOLON
    //consume INPUT token
    lexer->GetToken();
    
    //make input instruction for the variable
    InstructionNode* input = new InstructionNode();
    input->input_inst.var_index = indexOfToken(lexer->GetToken().lexeme);
    current->next = input;
    input->next = NULL;
    current = input;
    
    //consume SEMICOLON
    lexer->GetToken();
}

void parseWhile() {
    //consume WHILE
    if(lexer->peek(1).token_type == WHILE) {
        lexer->GetToken();
    
        //consume LPARENTHESIS
        lexer->GetToken();

        std::vector<Token> condition = parseCondition();

        if(condition.size() != 0) {
            //need to make a CJUMP instruction
            InstructionNode* cJump = new InstructionNode();
            cJump->cjmp_inst.operand1_index = indexOfToken(condition[0].lexeme);
            
            ConditionalOperatorType cop;
            //get conditional operation
            if(condition[1].lexeme == ">") {
                cop = CONDITION_GREATER;
            }
            else if(condition[1].lexeme == "<") {
                cop = CONDITION_LESS;
            }
            else {
                cop = CONDITION_NOTEQUAL;
            }
            cJump->cjmp_inst.condition_op = cop;
            cJump->cjmp_inst.operand2_index = indexOfToken(condition[2].lexeme);

            //set jump to the first instruction of body
            cJump->next = NULL;
            current = cJump;
            
            parseBody();

            current->next = cJump;
            cJump->cjmp_inst.target = NULL;
            cJump->next = current->next;

            current = cJump;
            
        }
    }

}

void parseIf() {
    if(lexer->peek(1).token_type == IF) {
        //consume the IF
        lexer->GetToken();

        //consume the LPARENTHESIS
        lexer->GetToken();

        //get the condition involved
        std::vector<Token> condition = parseCondition();

        if(condition.size() != 0) {
            InstructionNode* cJump = new InstructionNode();
            cJump->cjmp_inst.operand1_index = indexOfToken(condition[0].lexeme);

            ConditionalOperatorType cop;
            //get conditional operation
            if(condition[1].lexeme == ">") {
                cop = CONDITION_GREATER;
            }
            else if(condition[1].lexeme == "<") {
                cop = CONDITION_LESS;
            }
            else {
                cop = CONDITION_NOTEQUAL;
            }

            cJump->cjmp_inst.condition_op = cop;
            cJump->cjmp_inst.operand2_index = indexOfToken(condition[2].lexeme);

            cJump->next = NULL;
            current->next = cJump;
            
            current = cJump;

            parseBody();

            cJump->cjmp_inst.target = current->next;

        }
    }
}

void parseCase() {

    if (lexer->peek(1).token_type == CASE){
        if (lexer->peek(2).token_type == NUM){
            if (lexer->peek(3).token_type == COLON){
                //parse body, make sure not empty, and then add to mem/add/etc
                parseBody();
            }
        }
    }

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
    if(lexer->peek(1).token_type == FOR) {
        //consume FOR
        lexer->GetToken();
        //consume LPARENTHESIS
        lexer->GetToken();

        std::vector<Token> assignment = parseAssignment();

        std::vector<Token> condition = parseCondition();

        //create assignment instruction

        //set current to the assignment instruction

        //
    }
}

void parseInputs() {
    //go thorugh the int list at the end of the input file and add each one in order to input vector
    while(lexer->peek(1).token_type != END_OF_FILE) {
        inputs.push_back(stoi(lexer->GetToken().lexeme));
    }
    inputs.push_back(stoi(lexer->GetToken().lexeme));

    lexer->GetToken();
}

void parseBody() {
    //consume LBRACE
    lexer->GetToken();

    parseStatementList();

    //consume RBRACE
    lexer->GetToken();
}

//returns index of lexeme in address array
int indexOfToken(std::string str) {
    for(int i = 0; i < address->length(); i++) {
        if(address[i] == str) {
            return i;
        }
    }
    return -1;
}

//assigns each variable a space in mem using next_available as index
void parseIdList() {
    next_available = 0;

    while(lexer->peek(1).token_type != SEMICOLON) {
        if(lexer->peek(1).token_type == COMMA) {
            lexer->GetToken();
        }
        else {
            address[next_available] = lexer->GetToken().lexeme;
            mem[next_available] = 0;
            next_available++;
        }
    }
}

void parseVarSection() {
    parseIdList();

    //consume the SEMICOLON
    lexer->GetToken();
}

void parseStatement() {
    //peek ahead and see what keyword it is and call the appropriate parser
    //assignment

    //while
    if(lexer->peek(1).token_type == WHILE) {
        parseWhile();
    }
    //if
    else if(lexer->peek(1).token_type == IF) {
        parseIf();
    }

    //switch
    else if(lexer->peek(1).token_type == SWITCH) {
        parseSwitch();
    }

    //for
    else if(lexer->peek(1).token_type == FOR) {
        parseFor();
    }

    //input
    else if(lexer->peek(1).token_type == INPUT) {
        parseInput();
    }

    //output
    else if(lexer->peek(1).token_type == OUTPUT) {
        parseOutput();
    }
}

void parseStatementList() {
    //call parseStatement() for all statements in the list
    while(lexer->peek(1).token_type != END_OF_FILE) {
        parseStatement();
    }
}

void parseProgram() {
    lexer = new LexicalAnalyzer();

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
