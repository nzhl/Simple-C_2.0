#include "../io/io.h"
#include "../lex/lexer.h"
#include "../word/Word.h"

#include <vector>
#include <stack>

const static int WORD_LENGTH = 4;
/*
    parameter_decl ::= type {'*'} id {',' type {'*'} id}

    body_decl ::= {variable_decl}, {statement}

    statement ::= non_empty_statement | empty_statement

    non_empty_statement ::= if_statement | while_statement | '{' statement '}'
                          | 'return' expression | expression ';'

    if_statement ::= 'if' '(' expression ')' statement ['else' non_empty_statement]

    while_statement ::= 'while' '(' expression ')' non_empty_statement
*/

void program();

static void global_declaration();
static void function_definition();
static void function_parameter_list();
static void function_body();
static void block();
static void statement();
static void expression();

static void assign_expression();
static int boolean_expression();
static int relation_expression();
static int plus_minus_expression();
static int multiply_division_expression();
static int variable();

static void skip(int token);

static void emit(const char* fmt, ...);

extern PWord pWord;
extern std::vector<std::string> codes;

static PWord currentID;

void program(){
    // program ::= {global_declaration}+

    // generate code for string
    int string_index = 0;
    for(auto &each : Word::table){
        if(each.second.token == STR){
            emit("l_str%d:", string_index++);
            emit("\t\"%s\"", each.first.c_str());
            each.second.position = string_index;
        }
    }
    get_next_token();
    while(pWord->token != TK_EOF){
        global_declaration();
    }

#if 1
    for(auto each : codes){
        printf("%s\n", each.c_str());
    }
#endif
}

void global_declaration(){
    // global_declaration ::= variable_declaration | function_definition
    // variable_declaration ::= type {'*'} id { ',' {'*'} id } ';'
    // function_definition ::= type {'*'} id '(' parameter_declaration ')' '{' body_declaration '}'

    //parse the type :
    // 1. int = 0 (default)
    // 2. char = 1
    // 3. ptr = 2
    int type = T_INT;
    if(pWord->token == INT){
        type = T_INT;
        skip(INT);
    } else if(pWord->token == CHAR){
        type = T_CHAR;
        skip(CHAR);
    }

    while (pWord->token != COMMA && pWord->token != END){
        while (pWord->token == STAR){
            skip(STAR);
            type += T_PTR;
        }

        if(pWord->token != ID){
            error("identifier expected, but got %s", pWord->rawStr.c_str());
        }
        if(pWord->range == Global){
            error("duplicate global identifier %s", pWord->rawStr.c_str());
        }
        currentID = pWord;
        skip(ID);

        currentID->range = Global;
        currentID->type = type;
        if(pWord->token == LPA){
            currentID->property = FUNCTION;
            // add the label for the function
            // fresh the %ebp register
            // fresh the %ebp register
            emit(".global _%s", currentID->rawStr.c_str());
            emit("_%s", (currentID->rawStr + ":").c_str());
            emit("\tpushl %%ebp");
            emit("\tmovl %%esp, %%ebp");
            function_definition();
            return;
        }
    }
    skip(pWord->token);

}




void function_definition(){
    /*
     *  now deal with those part
     *  (...){...}
     */

    skip(LPA);
    function_parameter_list();
    skip(RPA);

    //{...}
    function_body();
}

/**
 *   ::= variable_declaration (COMMA variable_declaration)*
 */
int variable_position = 0, parameter_number = 0, label_index = 0;

void function_parameter_list(){
    int type;
    variable_position = 0;
    while (pWord->token != RPA){
        ++variable_position;
        type = T_INT;
        if (pWord->token == INT) {
            type = T_INT;
            skip(INT);
        } else if (pWord->token == CHAR) {
            type = T_CHAR;
            skip(CHAR);
        }

        while (pWord->token == STAR){
            skip(STAR);
            type += T_PTR;
        }

        if(pWord->token != ID){
            error("identifier expected, but got %s", pWord->rawStr.c_str());
        }
        currentID = pWord;
        skip(ID);

        currentID->range = Local;
        currentID->type = type;
        currentID->property = VARIABLE;
        // the part above is exactly like the function one
        // TODO : save the global variable with the same name

        currentID->position = variable_position;
        if(pWord->token == COMMA){
            skip(COMMA);
            continue;
        }
    }

    parameter_number = variable_position;
}


void function_body(){
    // now is this part
    // {...}

    skip(BEGIN);

    int type;

    // ... {
    // 1. local declarations
    // 2. statements
    // }
    while (pWord->token == INT || pWord->token == CHAR){
        type = pWord->token == INT ? T_INT : T_CHAR;
        skip(pWord->token);

        while (pWord->token != SEMICOLON) {
            variable_position++;

            while (pWord->token == STAR){
                skip(STAR);
                type += T_PTR;
            }
            if (pWord->token != ID) {
                error("identifier expected, but got %s", pWord->rawStr.c_str());
            }

            currentID = pWord;
            skip(ID);

            currentID->range = Local;
            currentID->type = type;
            currentID->property = VARIABLE;
            currentID->position = variable_position;
            if (pWord->token == COMMA) {
                skip(COMMA);
                continue;
            }
        }
        skip(SEMICOLON);
    }

    // allocate enough space for the variable
    emit("\tsubl $%d, %%esp", WORD_LENGTH * (variable_position - parameter_number));

    while (pWord->token != END){
        statement();
    }
    skip(END);

    // free the space and return
    emit("\tleave");
    emit("\tret");
}

void block(){
    skip(BEGIN);
    while (pWord->token != END){
        statement();
    }
    skip(END);
}


void statement(){
/**
 *  :== if_statement
 *    | while_statement
 *    | continue_statement
 *    | break_statement
 *    | return_statement
 *    | expression ;
 *    | {<statements>}
 */

    if(pWord->token == IF){
        skip(IF);
        skip(LPA);
        expression();
        skip(RPA);


        // set the zero flag, if expression value is 0, then the flag is set.
        emit("\tpopl %%eax");
        emit("\ttestl %%eax, %%eax");

        // use the flag to jump
        emit("\tje if_l%d", label_index);

        block();

        // set the no condition jump after if statement
        emit("\tjmp if_l%d", ++label_index);
        emit("if_l%d:", label_index-1);
        auto end_if_label = label_index++;
        if(pWord->token == ELSE){
            skip(ELSE);
            block();
        }
        emit("if_l%d:", end_if_label);
    } else if(pWord->token == WHILE){
        emit("while_l%d:", label_index);
        auto start_while_label = label_index++;
        auto end_while_label = label_index++;
        skip(WHILE);
        skip(LPA);
        expression();
        skip(RPA);

        // set the zero flag, if expression value is 0, then the flag is set.
        emit("\tpopl %%eax");
        emit("\ttestl %%eax, %%eax");
        emit("\tje while_l%d", end_while_label);
        block();
        emit("\tjmp while_l%d", start_while_label);
        emit("while_l%d:", end_while_label);
    } else if(pWord->token == RETURN){
        skip(RETURN);
        expression();
        // put return value in %eax then return
        emit("\tpopl %%eax");
        emit("\tleave");
        emit("\tret");
        skip(SEMICOLON);
    } else if(pWord->token == BEGIN){
        block();
    } else if(pWord->token == SEMICOLON){
        skip(SEMICOLON);
    } else{
        // expression
        expression();
        skip(SEMICOLON);
    }
}


void expression(){
    assign_expression();
}

std::stack<int> opt, opa;
int isAddress = 0;

// 1. finish assign 2. put the expression value on the top of stack
void assign_expression(){
    opa = opt = std::stack<int>();
    auto l = plus_minus_expression();
    while (pWord->token == ASSIGN){
        if(!isAddress){ error("%s is not a left value", pWord->rawStr.c_str()); }

        skip(ASSIGN);
        assign_expression();
        emit("\tpopl %%eax");
        emit("\tmovl %%eax, %d(%%ebp)", l);
        emit("\tpushl %d(%%ebp)", l);
    }

    // put l in the top of stack
    // if the return value is not address,
    // then is already on the top of the stack
    if(isAddress){
        emit("\tmovl %d(%%ebp), %%eax", l);
        emit("\tpushl %%eax");
    }
}



/**
 *  ::= relation_expression (( EQ | NEQ ) relation_expression)*
 */
int boolean_expression(){ }

/**
 *  ::= plus_minus_expression ( (GT | GE | LT | LE) plus_minus_expression)*
 */
int relation_expression(){ }

/**
 *  ::=  multiply_division_expression (( PLUS / MINUS ) multiply_division_expression)*
 */

int plus_minus_expression(){
    int l = multiply_division_expression();
    opa.push(isAddress); opa.push(l);
    while (!opt.empty() && (opt.top() == STAR || opt.top() == DIVIDE || opt.top() == PLUS || opt.top() == MINUS)){
        int left = opa.top(); opa.pop();
        // put the right value on the top of the stack
        if(opa.top() == 1){
            //is address, put the value in %eax
            emit("\tmovl %d(%%ebp), %%eax", left);
            emit("\tpushl %%eax", left);
        }
        opa.pop();


        // put the left value in %eax
        int right = opa.top(); opa.pop();
        if(opa.top() == 1){
            //is address, put the value in %eax
            emit("\tmovl %d(%%ebp), %%eax", right);
        } else{
            emit("\tpopl %%eax");
        }
        opa.pop();


        if(opt.top() == STAR){
            emit("\timull (%%esp)");
        } else if(opt.top() == DIVIDE){
            emit("\tidivl (%%esp)");
        } else if( opt.top() == PLUS){
            emit("\taddl (%%esp), %%eax");
        } else{
            emit("\tsubl (%%esp), %%eax");
        }
        opt.pop();

        isAddress = 0;
        emit("\tpushl %%eax");
    }
    if(pWord->token == PLUS || pWord->token == MINUS){
        opt.push(pWord->token);
        skip(pWord->token);
        plus_minus_expression();
    }else{
        if(!opt.empty()){
            error("Error operator %s", opt.top());
        }
    }
    return l;
}

/**
 *  ::= (variable | const_value) (( STAR | DIVIDE | MOD ) variable)*
 */
int multiply_division_expression(){
    return variable();
}

int variable(){
    int re = 0;
    if(pWord->token == NUMBER){
        isAddress = 0;
        emit("\tpushl $%d", pWord->convert2Number());
        skip(NUMBER);
    } else if(pWord->token == ID){
        isAddress = 1;
        re = pWord->position;
        if(re <= parameter_number){
            re = (parameter_number - re) * WORD_LENGTH + 8;
        }
        else{
            re = -(re - parameter_number) * WORD_LENGTH;
        }
        skip(ID);
    } else{
        //TODO : function call
    }

    return re;
}

void skip(int token){
    if(pWord->token == token) {
        get_next_token();
    } else{
        error("unexpected token \"%s\"", pWord->rawStr.c_str());
    }
}

void emit(const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    char buffer[1024];
    vsprintf(buffer, fmt, ap);
    codes.push_back(std::string(buffer));
    va_end(ap);
}
