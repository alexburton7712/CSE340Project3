//Alexander Burton and Naomi Sano finished the parsing functions together before 
// the announcement to work solo. The debugging of the code was done individually.



#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include "compiler.h"
#include "lexer.h"
using namespace std;


LexicalAnalyzer* lexer = new LexicalAnalyzer();

std::vector<InstructionNode*> instructionList;

std::string address[1000];

InstructionNode* start;

InstructionNode* current;


int indexOfToken(std::string str);
bool isPrimary(int index);
bool isRelop(int index);
bool isOp(int index);
bool isCondition(int index);
bool isExpression(int index);
std::vector<Token> parseCondition();
std::vector<Token> parseExpression();
std::vector<Token> parseAssignment();
void parseOutput();
void parseInput();
void parseWhile();
void parseIf();
void parseStatementList();
void parseStatement();
void parseBody();
void parseCaseList(Token switchId);
void parseSwitch();
ConditionalOperatorType condOpType(std::string op);
void parseFor();
void parseInputs();
void parseIdList();
void parseVarSection();
void parseProgram();




//returns index of lexeme in address array
int indexOfToken(std::string str) {
    for(int i = 0; i < 1000; i++) {
        if(address[i] == str) {
            return i;
        }
    }
    return -1;
}

int indexOfTokenMem(int target) {
    for(int i = 0; i < 1000; i++) {
        if(mem[i] == target) {
            return i;
        }
    }
    return -1;
}

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
    //std::cout << "parseCondition" << endl;
    //primary relop primary
    std::vector<Token> condition;

    //primary
    if (isCondition(1)){
        //primary
        condition.push_back(lexer->GetToken());
        //relop 
        condition.push_back(lexer->GetToken());
        //primary
        condition.push_back(lexer->GetToken());
    }
    return condition;
    
}

std::vector<Token> parseExpression() {
    //std::cout << "parseExpression" << endl;
    //primary op primary
    std::vector<Token> expression;

    if (isExpression(1)){

        //primary
        expression.push_back(lexer->GetToken());
        //op
        expression.push_back(lexer->GetToken());
        //primary
        expression.push_back(lexer->GetToken());
    }
    return expression;
}

//parsers for each rule in the grammar
std::vector<Token> parseAssignment() {
    //std::cout << "parseAssignment" << endl;
    // ID EQUAL expr SEMICOLON
    // ID EQUAL primary SEMICOLON
    std::vector<Token> assignment;

    //make instruction node
    InstructionNode* assignInstruction = new InstructionNode();
    assignInstruction->type = ASSIGN;

    //the ID
    assignment.push_back(lexer->GetToken());
    //the equal
    assignment.push_back(lexer->GetToken());
    //primary (ID or NUM)
    assignment.push_back(lexer->GetToken());

    //      get lhs index
    int lhsIndex = indexOfToken(assignment[0].lexeme);
    assignInstruction->assign_inst.left_hand_side_index = lhsIndex;

    //      get rhs op1 index
    int op1Index;
    if (assignment[2].token_type != ID){//if id, then op1 is index of token
        //if constant, add to mem
        int op1Val = std::stoi(assignment[2].lexeme);
        
        mem[next_available] = op1Val;
        address[next_available] = assignment[2].lexeme;
        next_available++;

        //then find here
        op1Index = indexOfToken(assignment[2].lexeme);
    }
    else{
        //else is an ID find index
        op1Index = indexOfToken(assignment[2].lexeme);
    }

    //std::cout<< "op1Index: " << assignment[2].lexeme << " = " << op1Index << endl;
    
    //then find index (which should just be next_available--)
    //else is an ID find index

    assignInstruction->assign_inst.operand1_index = op1Index;

    //if expression next is not semicolon, it is expression, add to instruction list 
    if (lexer->peek(1).token_type != SEMICOLON){
        //op
        assignment.push_back(lexer->GetToken());
        //second primary
        assignment.push_back(lexer->GetToken());

        //----get op----
        ArithmeticOperatorType aop;
        //get conditional operation
        if(assignment[3].token_type == PLUS) {
            aop = OPERATOR_PLUS;
        }
        else if(assignment[3].token_type == MINUS) {
            aop = OPERATOR_MINUS;
        }
        else if(assignment[3].token_type == MULT) {
            aop = OPERATOR_MULT;
        }
        else if(assignment[3].token_type == DIV) {
            aop = OPERATOR_DIV;
        }

        //----get op2----
        //if id, then op2 is index of token
        int op2Index;
        if (assignment[4].token_type != ID){
            //if constant, add to mem
            int op2Val = std::stoi(assignment[4].lexeme);
            
            mem[next_available] = op2Val;
            address[next_available] = assignment[4].lexeme;
            next_available++;

            //then find index (which should just be next_available--)
            op2Index = indexOfToken(assignment[4].lexeme);
        }
        else{
            //else is an ID find index
            op2Index = indexOfToken(assignment[4].lexeme);
        }

        //std::cout<< "op2Index: " << assignment[4].lexeme << " = " << op2Index << endl;
        
        assignInstruction->assign_inst.op = aop;
        assignInstruction->assign_inst.operand2_index = op2Index;

    }
    else{
        assignInstruction->assign_inst.op = OPERATOR_NONE;
    }


    //get semicolon
    lexer->GetToken();

    //link 
    current->next = assignInstruction;
    assignInstruction->next = NULL;
    current = assignInstruction;
    
    return assignment;
}

void parseOutput() {
    //std::cout << "parseOutput" << endl;
    //output --> output ID SEMICOLON
    //consume OUTPUT token
    lexer->GetToken();
    
    //make output instruction for the variable
    InstructionNode* output = new InstructionNode();
    output->type = OUT;
    output->output_inst.var_index = indexOfToken(lexer->GetToken().lexeme);
    current->next = output;
    output->next = NULL;
    current = output;

    //consume SEMICOLON
    lexer->GetToken();
}

void parseInput() {
    //std::cout << "parseInput" << endl;
    //input --> INPUT ID SEMICOLON
    //consume INPUT token
    lexer->GetToken();
    
    //make input instruction for the variable
    InstructionNode* input = new InstructionNode();
    input->type = IN;
    input->input_inst.var_index = indexOfToken(lexer->GetToken().lexeme);
    current->next = input;
    input->next = NULL;
    current = input;
    
    //consume SEMICOLON
    lexer->GetToken();
}


void parseWhile() {
    //std::cout << "parseWhile" << endl;
    //consume WHILE
    if(lexer->peek(1).token_type == WHILE) {
        lexer->GetToken();
    

        std::vector<Token> condition = parseCondition();

        if(condition.size() != 0) {
            //need to make a CJUMP instruction
            InstructionNode* cJump = new InstructionNode();
            cJump->type = CJMP;
            cJump->cjmp_inst.operand1_index = indexOfToken(condition[0].lexeme);
            

            ConditionalOperatorType cop;
            //get conditional operation
            if(condition[1].token_type == GREATER) {
                cop = CONDITION_GREATER;
            }
            else if(condition[1].token_type == LESS) {
                cop = CONDITION_LESS;
            }
            else {
                cop = CONDITION_NOTEQUAL;
            }


            int cop2Index;
            if (condition[2].token_type != ID){//if id, then op1 is index of token
                //if constant, add to mem
                int cop1Val = std::stoi(condition[2].lexeme);
                
                mem[next_available] = cop1Val;
                address[next_available] = condition[2].lexeme;
                next_available++;

                //then find here
                cop2Index = indexOfToken(condition[2].lexeme);
            }
            else{
                //else is an ID find index
                cop2Index = indexOfToken(condition[2].lexeme);
            }

            cJump->cjmp_inst.condition_op = cop;
            cJump->cjmp_inst.operand2_index = cop2Index;

            //set jump to the first instruction of body
            current->next = cJump;
            cJump->next = NULL;
            current = cJump;
            
            parseBody();
            
            current->next = cJump;
            
            //make a NOOP instruction
            InstructionNode* noop = new InstructionNode();
            noop->type = NOOP;

            cJump->cjmp_inst.target = noop;

            noop->next = NULL;

            current = noop;
            
        }
    }

}

void parseIf() {
    
    //consume the IF
    lexer->GetToken();

    //get the condition involved
    std::vector<Token> condition = parseCondition();


    if(condition.size() != 0) {
        InstructionNode* cJump = new InstructionNode();
        cJump->type = CJMP;
        cJump->cjmp_inst.operand1_index = indexOfToken(condition[0].lexeme);

        ConditionalOperatorType cop;
        //get conditional operation
        if(condition[1].token_type == GREATER) {
            cop = CONDITION_GREATER;
        }
        else if(condition[1].token_type == LESS) {
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

        InstructionNode* noop = new InstructionNode();
        noop->type = NOOP;
        noop->next = NULL;
        current->next = noop;
        cJump->cjmp_inst.target = noop;

        current = noop;

    }
    
}


void parseStatementList() {
    //std::cout << "parseStatementList" << endl;
    //call parseStatement() for all statements in the list    
    while(lexer->peek(1).token_type != END_OF_FILE) {
        if(lexer->peek(1).token_type != RBRACE) {
            parseStatement();
        }
        else{
            break;
        }
    }
}

void parseStatement() {
    //std::cout << "parseStatement" << endl;


    //peek ahead and see what keyword it is and call the appropriate parser
    //assignment

    ////std::cout << "\tpeek1: " << lexer->peek(1).lexeme << endl;

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
    else{
        parseAssignment();
    }
}

void parseBody() {
    //std::cout << "parseBody" << endl;
    //consume LBRACE
    lexer->GetToken();
    parseStatementList();
    //consume RBRACE
    lexer->GetToken();
}

void parseCaseList(Token switchId) {
    //std::cout << "parseCaseList" << endl;
    InstructionNode* endSwitch = new InstructionNode();
    endSwitch->type = NOOP;
    
    do {
        //consume DEFAULT
        if(lexer->peek(1).token_type == DEFAULT) {
            //consume DEFAULT
            lexer->GetToken();
            //consume COLON
            lexer->GetToken();
            //make default instruction
            parseBody();
            current->next = endSwitch;
        }
        else {
            //consume CASE
            lexer->GetToken();
            Token op2 = lexer->GetToken();

            int op2Index;
            //is a num add to mem and address
            int op2ValasInt = std::stoi(op2.lexeme);
            mem[next_available] = op2ValasInt;
            address[next_available] = op2.lexeme;
            next_available++;

            //then find here
            op2Index = indexOfToken(op2.lexeme);
            
            //consume COLON
            lexer->GetToken();
            //make IF instruction 
            InstructionNode* cJump = new InstructionNode();
            cJump->type = CJMP;
        
            cJump->cjmp_inst.operand1_index = indexOfToken(switchId.lexeme);
            cJump->cjmp_inst.operand2_index = op2Index;
            cJump->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

            current->next = cJump;
            current = cJump;

            parseBody();
            
            InstructionNode* noop = new InstructionNode();
            noop->type = NOOP;
            noop->next = NULL;

            cJump->cjmp_inst.target = cJump->next;
            cJump->next = noop;
            current->next = endSwitch;
            current = noop;
    
        }
    }while(lexer->peek(1).token_type != RBRACE);

    current = endSwitch;

}

//translate it to a sequence of IF statements
void parseSwitch() {
    //std::cout << "parseSwitch" << endl;
    Token switchId;
    //take care of switch statements with and without default case
    if (lexer->peek(1).token_type == SWITCH){
        if (lexer->peek(2).token_type == ID){
            if (lexer->peek(3).token_type == LBRACE){
                lexer->GetToken(); //get switch
                switchId = lexer->GetToken(); //get id --> this is variable that we check value in case
                lexer->GetToken(); //get lbrace
                parseCaseList(switchId);
                lexer->GetToken(); //get rbrace
            }
        }
    }
}


//translate it to a WHILE loop
void parseFor() {
    //std::cout << "parseFor" << endl;
    //if(lexer->peek(1).token_type == FOR) {
        //consume FOR
        lexer->GetToken();
        //consume LPARENTHESIS
        lexer->GetToken();

        //parse assignment 1
        parseAssignment();
        InstructionNode* firstAssignment = current;

        //parse condition
        std::vector<Token> condition = parseCondition();

        //consume SEMICOLON
        lexer->GetToken();

        //parse assignment 2
        parseAssignment();
        InstructionNode* secondAssignment = current;

        //consume the RPAREN
        lexer->GetToken();


        //make a CJ instruction
        InstructionNode* cJump = new InstructionNode();
        cJump->type = CJMP;
        cJump->cjmp_inst.operand1_index = indexOfToken(condition[0].lexeme);

        ConditionalOperatorType cop;
        //get conditional operation
        if(condition[1].token_type == GREATER) {
            cop = CONDITION_GREATER;
        }
        else if(condition[1].token_type == LESS) {
            cop = CONDITION_LESS;
        }
        else {
            cop = CONDITION_NOTEQUAL;
        }
        cJump->cjmp_inst.condition_op = cop;

        int cop2Index;
        if (condition[2].token_type != ID){//if id, then op1 is index of token
            //if constant, add to mem
            int cop2Val = std::stoi(condition[2].lexeme);
            
            mem[next_available] = cop2Val;
            address[next_available] = condition[2].lexeme;
            next_available++;

            //then find here
            cop2Index = indexOfToken(condition[2].lexeme);
        }
        else{
            //else is an ID find index
            cop2Index = indexOfToken(condition[2].lexeme);
        }
        cJump->cjmp_inst.operand2_index = cop2Index;


        //link before body
        firstAssignment->next = cJump;
        cJump->next = NULL;
        current = cJump;

        parseBody();

        //link after body
        current->next = secondAssignment;
        secondAssignment->next = cJump;

        //create NOOP and link
        InstructionNode* noop = new InstructionNode();
        noop->type = NOOP;
        noop->next = NULL;

        cJump->cjmp_inst.target = noop;
        current = noop;
    //}
}

void parseInputs() {
    //std::cout << "parseInputs" << endl;
    //go thorugh the int list at the end of the input file and add each one in order to input vector
    while(lexer->peek(1).token_type != END_OF_FILE) {
        //std::cout << lexer->peek(1).lexeme << endl;
        inputs.push_back(std::stoi(lexer->GetToken().lexeme));
    }
    lexer->GetToken();
}

//assigns each variable a space in mem using next_available as index
void parseIdList() {
    //std::cout << "parseIdList" << endl;
    next_available = 0;

    while(lexer->peek(1).token_type != SEMICOLON) {
        if(lexer->peek(1).token_type != COMMA) {
            address[next_available] = lexer->GetToken().lexeme;
            mem[next_available] = 0;
            next_available++;
        }
        else {
            lexer->GetToken();
        }
    }
    lexer->GetToken();
}

void parseVarSection() {
    ////std::cout << "parseVarSection" << endl;
    parseIdList();
}

void parseProgram() {
    ////std::cout << "parseProgram" << endl;
    InstructionNode* noop = new InstructionNode;
    noop->type = NOOP;
    current = noop;
    start = current;

    parseVarSection();

    parseBody();

    parseInputs();
}

struct InstructionNode * parse_generate_intermediate_representation() {
    //std::cout << "instructionNode" << endl;
    //parse program
    parseProgram();

    /*std::cout << "mem: " << mem[0] << ", " 
    <<  mem[1] << ", " 
    <<  mem[2] << ", " 
    <<  mem[3] << ", " 
    <<  mem[4] << ", " 
    <<  mem[5] << ", " 
    <<  mem[6] << ", " 
    <<  mem[7] << ", " 
    <<  mem[8] << ", " << endl;

    std::cout << "address: " << address[0] << ", " 
    <<  address[1] << ", " 
    <<  address[2] << ", " 
    <<  address[3] << ", " 
    <<  address[4] << ", " 
    <<  address[5] << ", " 
    <<  address[6] << ", " 
    <<  address[7] << ", " 
    <<  address[8] << ", " << endl;*/

    return start;
}
