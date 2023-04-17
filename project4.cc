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

InstructionNode* current;

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
        //relop 
        condition.push_back(lexer->GetToken());
        //primary
        condition.push_back(lexer->GetToken());
    }
    return condition;
    
}

std::vector<Token> parseExpression() {
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
    
    // ID EQUAL expr SEMICOLON
    // ID EQUAL primary SEMICOLON
    std::vector<Token> assignment;
    
    if (lexer->peek(1).token_type == ID){
        if (lexer->peek(2).token_type == EQUAL){
            if (isPrimary(3)){
                //the ID
                assignment.push_back(lexer->GetToken());
                //the equal
                assignment.push_back(lexer->GetToken());
                //primary (ID or NUM)
                assignment.push_back(lexer->GetToken());
            }
            else if (isExpression(3)){
                //the ID 0
                assignment.push_back(lexer->GetToken());
                //the equal 1
                assignment.push_back(lexer->GetToken());
                //the expression 2 3 4
                std::vector<Token> expression = parseExpression();
                for (int i = 0; i < expression.size(); i++){
                    assignment.push_back(expression[i]);
                }
            }
        }
    }
    //get semicolon
    lexer->GetToken();

    //make instruction node
    InstructionNode* assignInstruction = new InstructionNode();
    assignInstruction->type = ASSIGN;
    //if size is 3, then the rhs is primary
    if (assignment.size() == 3){
        int lhsIndex = indexOfToken(assignment[0].lexeme);
        int op1Index = indexOfToken(assignment[2].lexeme);
        assignInstruction->assign_inst.left_hand_side_index = lhsIndex;
        assignInstruction->assign_inst.operand1_index = op1Index;
        assignInstruction->assign_inst.op = OPERATOR_NONE;
    }
    else{ //else is expression
        int lhsIndex = indexOfToken(assignment[0].lexeme);
        int op1Index = indexOfToken(assignment[2].lexeme);
        int op2Index = indexOfToken(assignment[4].lexeme);
        //int opInstIndex = indexOfToken(assignment[3].lexeme);

        assignInstruction->assign_inst.left_hand_side_index = lhsIndex;
        assignInstruction->assign_inst.operand1_index = op1Index;
        assignInstruction->assign_inst.operand2_index = op2Index;

        ArithmeticOperatorType aop;
        //get conditional operation
        if(assignment[3].lexeme == "+") {
            aop = OPERATOR_PLUS;
        }
        else if(assignment[3].lexeme == "-") {
            aop = OPERATOR_MINUS;
        }
        else if(assignment[3].lexeme == "*") {
            aop = OPERATOR_MULT;
        }
        else if(assignment[3].lexeme == "/") {
            aop = OPERATOR_DIV;
        }
        assignInstruction->assign_inst.op = aop;
    }

    //link 
    current->next = assignInstruction;
    current = assignInstruction;
    
    return assignment;
}

void parseOutput() {
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
    //consume WHILE
    if(lexer->peek(1).token_type == WHILE) {
        lexer->GetToken();
    
        //consume LPARENTHESIS
        lexer->GetToken();

        std::vector<Token> condition = parseCondition();

        if(condition.size() != 0) {
            //need to make a CJUMP instruction
            InstructionNode* cJump = new InstructionNode();
            cJump->type = CJMP;
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
    if(lexer->peek(1).token_type == IF) {
        //consume the IF
        lexer->GetToken();

        //consume the LPARENTHESIS
        lexer->GetToken();

        //get the condition involved
        std::vector<Token> condition = parseCondition();

        if(condition.size() != 0) {
            InstructionNode* cJump = new InstructionNode();
            cJump->type = CJMP;
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

// std::vector<Token> parseCase() {
//     std::vector<Token> cases;

//     if (lexer->peek(1).token_type == CASE){
//         if (lexer->peek(2).token_type == NUM){
//             if (lexer->peek(3).token_type == COLON){
                
//                 //get case
//                 cases.push_back(lexer->GetToken());

//                 //get num
//                 cases.push_back(lexer->GetToken());

//                 //get colon
//                 lexer->GetToken();
                
//                 parseBody();
//             }
//         }
//     }

//     return cases;

// }

void parseCaseList(Token switchId) {
    InstructionNode* endSwitch = new InstructionNode();
    endSwitch->type = NOOP;
    
    do {
        //consume CASE
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
            //consume COLON
            lexer->GetToken();
            //make IF instruction 
            InstructionNode* cJump = new InstructionNode();
            cJump->type = CJMP;
            cJump->cjmp_inst.operand1_index = indexOfToken(switchId.lexeme);
            cJump->cjmp_inst.operand2_index = indexOfToken(op2.lexeme);
            cJump->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

            current->next = cJump;
            current = cJump;

            parseBody();

            current->next = endSwitch;

            cJump->cjmp_inst.target = cJump->next;
            
            InstructionNode* noop = new InstructionNode();
            noop->type = NOOP;
            cJump->next = noop;
            current = noop;

        }
    }while(lexer->peek(1).token_type != RBRACE);

    current = endSwitch;

}

//translate it to a sequence of IF statements
void parseSwitch() {
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

ConditionalOperatorType condOpType(std::string op) {
    if(op == ">") {
        return CONDITION_GREATER;
    }
    if(op == "<") {
        return CONDITION_LESS;
    }
    return CONDITION_NOTEQUAL;
}

//translate it to a WHILE loop
void parseFor() {
    if(lexer->peek(1).token_type == FOR) {
        //consume FOR
        lexer->GetToken();
        //consume LPARENTHESIS
        lexer->GetToken();

        parseAssignment();
        //keep track of the first assignment 
        InstructionNode* firstAssignment = current;

        std::vector<Token> condition = parseCondition();

        //consume SEMICOLON
        lexer->GetToken();

        parseAssignment();
        InstructionNode* secondAssignment = current;

        //make a CJ instruction
        InstructionNode* cJump = new InstructionNode();
        cJump->type = CJMP;
        cJump->cjmp_inst.operand1_index = indexOfToken(condition[0].lexeme);
        cJump->cjmp_inst.condition_op = condOpType(condition[1].lexeme);
        cJump->cjmp_inst.operand2_index = indexOfToken(condition[2].lexeme);

        firstAssignment->next = cJump;

        current = cJump;

        //consume the RPAREN
        lexer->GetToken();

        parseBody();

        current->next = secondAssignment;
        current = current->next;

        current->next = cJump;

        //create NOOP
        InstructionNode* noop = new InstructionNode();
        noop->type = NOOP;
        noop->next = NULL;

        cJump->cjmp_inst.target = noop;
        current = noop;

        
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
    current = new InstructionNode;
    current->type = NOOP;

    parseVarSection();

    parseBody();

    parseInputs();
}

struct InstructionNode * parse_generate_intermediate_representation() {

    

}
