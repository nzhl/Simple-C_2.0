#include "../io/io.h"
#include "../lex.h"
#include "../word/Word.h"
#include "Function.h"
#include "Variable.h"

#include <stack>

void program();

static void global_declaration();
static void function_definition();
static void function_parameter_list();
static void function_body();
static void block();
static void statement();

static void expression();
static void assign_expression();
static void boolean_expression();
static void relation_expression();
static void plus_minus_expression();
static void multiply_division_expression();
static void unary_expression();
static void postfix_expression();

static void function_argument_list(Function *callee);

static void skip(int token);
static void emit(const char* fmt, ...);


extern char *FilePath;
extern PWord pWord;
extern std::vector<std::string> codes;

static std::map<std::string, Function> function_table;
static PFunction pFunction;

static std::map<std::string, Variable> global_variable_table;
static std::stack<std::map<std::string, Variable>> local_variable_table_stack;
static PVariable pVariable;

static std::map<std::string, std::string> string_table;

static int string_index = 0;
static int label_index = 0;
static int current_variable_position = 0;
static int isArray = 0;

enum Type{ T_INT = 0, T_CHAR, T_PTR };

void program(){
    // program = {global_declaration}


    function_table.insert({"printf", Function()});
    pFunction = &function_table["printf"];
    pFunction->setName("_printf");
    pFunction->getParameters().resize(2);
    pFunction->setReturn_type(T_INT);

    get_next_token();
    while(pWord->token != TK_EOF){
        global_declaration();
    }

    // generate code for string
    for(auto &each : string_table){
        emit("%s:\n\t.asciz\t\"%s\"", each.second.c_str(), each.first.c_str());
    }

    const auto outputPath = std::string(FilePath) + ".s";
    const auto cmd = std::string("gcc -m32 -o ") + outputPath.substr(0, outputPath.size()-4) + " " + outputPath.c_str();
    auto fp = fopen(outputPath.c_str(), "w");
    for(auto each : codes){
#if 0
        printf("%s\n", each.c_str());
#endif
        fprintf(fp, "%s\n", each.c_str());
    }
    fclose(fp);
    printf("%s\n", cmd.c_str());
    system(cmd.c_str());
}

void global_declaration(){
    // global_declaration ::= variable_declaration | function_definition
    // variable_declaration ::= type {'*'} id { ',' {'*'} id } ';'
    // function_definition ::= type {'*'} id '(' parameter_declaration ')' '{' body_declaration '}'


    int type = T_INT;
    if(pWord->token == TK_INT){
        type = T_INT;
        skip(TK_INT);
    } else if(pWord->token == TK_CHAR){
        type = T_CHAR;
        skip(TK_CHAR);
    }

    while (pWord->token != TK_COMMAS){
        while (pWord->token == TK_STAR){
            skip(TK_STAR);
            type += T_PTR;
        }

        if(pWord->token != ID){
            error("identifier expected, but got %s", pWord->rawStr.c_str());
        }

        auto name = pWord->rawStr;
        skip(ID);

        if(pWord->token == TK_LPA){
            // this is a function
            // register the function in the function table
            function_table.insert({name, Function()});
            pFunction = &function_table[name];
            pFunction->setName("_" + name);
            pFunction->setReturn_type(type);

            // fresh the %ebp register
            // fresh the %ebp register
            emit(".global %s", pFunction->getName().c_str());
            emit("%s", (pFunction->getName() + ":").c_str());
            emit("\tpushl %%ebp");
            emit("\tmovl %%esp, %%ebp");
            function_definition();
            return;
        }

        global_variable_table.insert({pWord->rawStr, Variable()});
        pVariable = &global_variable_table[name];
        pVariable->setType(type);
        pVariable->setName(name);
    }
    skip(pWord->token);

}




void function_definition(){
    /*
     *  now deal with those part
     *  (...){...}
     */
    local_variable_table_stack.push(std::map<std::string, Variable>());

    skip(TK_LPA);
    function_parameter_list();
    skip(TK_RPA);

    //{...}
    function_body();

    local_variable_table_stack.pop();
}


void function_parameter_list(){
    int type, parameter_index = 0;

    while (pWord->token != TK_RPA){
        type = T_INT;
        if (pWord->token == TK_INT) {
            type = T_INT;
            skip(TK_INT);
        } else if (pWord->token == TK_CHAR) {
            type = T_CHAR;
            skip(TK_CHAR);
        }
        while (pWord->token == TK_STAR){
            skip(TK_STAR);
            type += T_PTR;
        }

        if(pWord->token != ID){
            error("identifier expected, but got %s", pWord->rawStr.c_str());
        }

        local_variable_table_stack.top().insert({pWord->rawStr, Variable()});
        pVariable = &local_variable_table_stack.top()[pWord->rawStr];
        pVariable->setType(type);
        pVariable->setName(pWord->rawStr);
        pVariable->setRelative_position(parameter_index++);

        pFunction->getParameters().push_back(type);
        skip(ID);

        if(pWord->token == TK_COMMAS){
            skip(TK_COMMAS);
            continue;
        }
    }
}


void function_body(){
    // now is this part
    // {...}

    skip(TK_BEGIN);

    int basic_type, pos = -4;

    // ... {
    // 1. local declarations
    // 2. statements
    // }
    while (pWord->token == TK_INT || pWord->token == TK_CHAR){
        basic_type = pWord->token == TK_INT ? T_INT : T_CHAR;
        skip(pWord->token);

        while (pWord->token != TK_SEMICOLON) {
            int type = basic_type;
            while (pWord->token == TK_STAR){
                skip(TK_STAR);
                type += T_PTR;
            }
            if (pWord->token != ID) {
                error("identifier expected, but got %s", pWord->rawStr.c_str());
            }

            local_variable_table_stack.top().insert({pWord->rawStr, Variable()});
            pVariable = &local_variable_table_stack.top()[pWord->rawStr];
            pVariable->setType(type);
            pVariable->setName(pWord->rawStr);
            pVariable->setRelative_position(pos);
            skip(ID);

            pos -= 4;

            if(pWord->token == TK_LSB){
                skip(TK_LSB);
                //ARRAY
                pVariable->setSize(pWord->convert2Number());
                pVariable->setType(pVariable->getType() + T_PTR);
                skip(TK_NUMBER);
                skip(TK_RSB);
                pos -= (pVariable->getSize() - 1) * 4;
                pVariable->setRelative_position(pos + 4);
            }

            if (pWord->token == TK_COMMAS) {
                skip(TK_COMMAS);
                continue;
            }
        }
        skip(TK_SEMICOLON);
    }

    pFunction->setAllocated_size(-pos-4);

    // allocate enough space for the variable
    emit("\tsubl $%d, %%esp", pFunction->getAllocated_size());

    while (pWord->token != TK_END){
        statement();
    }
    skip(TK_END);

    // free the space and return
    emit("\tleave");
    emit("\tret");
}

void block(){
    skip(TK_BEGIN);
    while (pWord->token != TK_END){
        statement();
    }
    skip(TK_END);
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

    if(pWord->token == TK_IF){
        skip(TK_IF);
        skip(TK_LPA);
        expression();
        skip(TK_RPA);


        // set the zero flag, if expression value is 0, then the flag is set.
        emit("\tpopl %%eax");
        emit("\ttestl %%eax, %%eax");

        auto start_else_label = label_index++;
        auto end_else_label = label_index++;

        // use the flag to jump
        emit("\tje if_l%d", start_else_label);

        block();

        // set the no condition jump after if statement
        emit("\tjmp if_l%d", end_else_label);
        emit("if_l%d:", start_else_label);
        if(pWord->token == TK_ELSE){
            skip(TK_ELSE);
            block();
        }
        emit("if_l%d:", end_else_label);
    } else if(pWord->token == TK_WHILE){
        emit("while_l%d:", label_index);
        auto start_while_label = label_index++;
        auto end_while_label = label_index++;
        skip(TK_WHILE);
        skip(TK_LPA);
        expression();
        skip(TK_RPA);

        // set the zero flag, if expression value is 0, then the flag is set.
        emit("\tpopl %%eax");
        emit("\ttestl %%eax, %%eax");
        emit("\tje while_l%d", end_while_label);
        block();
        emit("\tjmp while_l%d", start_while_label);
        emit("while_l%d:", end_while_label);
    } else if(pWord->token == TK_RETURN){
        skip(TK_RETURN);
        expression();
        // put return value in %eax then return
        emit("\tpopl %%eax");
        emit("\tleave");
        emit("\tret");
        skip(TK_SEMICOLON);
    } else if(pWord->token == TK_BEGIN){
        block();
    } else if(pWord->token == TK_SEMICOLON){
        skip(TK_SEMICOLON);
    } else{
        // expression
        expression();
        // drop the value of expression
        emit("\tpopl %%eax");
        skip(TK_SEMICOLON);
    }
}

//------------------------- expression part ---------------------------------

// expression basic translation rule :
// 1. finish operation
// 2. put the expression value on the top of stack

void expression(){
    isArray = 0;
    assign_expression();
}



void assign_expression(){
    boolean_expression();

    while (pWord->token == TK_ASSIGN){
        skip(TK_ASSIGN);
        emit("\tpopl %%eax");
        auto variable_position = current_variable_position;
        assign_expression();

        emit("\tmovl (%%esp), %%eax");
        if(isArray){
            //array
            isArray = 0;
            emit("\tmovl %%eax, %d(%%ebp, %%ecx, 4)", variable_position);
        }
        else{
            emit("\tmovl %%eax, %d(%%ebp)", variable_position);
        }
    }
}


void boolean_expression(){
    //   boolean_expression = boolean_expression { ("==" | "!=") relation_expression) }
    relation_expression();

    while(pWord->token == TK_EQ || pWord->token == TK_NEQ){
        auto current_operator = pWord->token;
        skip(pWord->token);
        relation_expression();

        /*
         *  left operand value : %esp + 4
         *  right operand value : %esp
         */

        emit("\tpopl %%eax");
        emit("\tcmpl %%eax, (%%esp)");

        if( current_operator == TK_EQ){
            emit("\tsete (%%esp)");
        } else{
            emit("\tsetne (%%esp)");
        }
    }
}

void relation_expression(){
    plus_minus_expression();

    while(pWord->token == TK_GT || pWord->token == TK_GEQ || pWord->token == TK_LT || pWord->token == TK_LEQ){
        auto current_operator = pWord->token;
        skip(pWord->token);
        plus_minus_expression();

        emit("\tpopl %%eax");
        emit("\tcmpl %%eax, (%%esp)");

        if( current_operator == TK_GT){
            emit("\tsetg (%%esp)");
        } else if (current_operator == TK_GEQ){
            emit("\tsetge (%%esp)");
        } else if ( current_operator == TK_LT){
            emit("\tsetl (%%esp)");
        } else{
            emit("\tsetle (%%esp)");
        }
    }
}

void plus_minus_expression(){
 //     plus_minus_expression = plus_minus_expression { ("+" | "-") multiply_division_expression) }
 //
 //                             Left Recursive Elimination
 //
 //     plus_minus_expression = multiply_division_expression helper
 //     helper = ("+" | "-") multiply_division_expression helper | E

    multiply_division_expression();

    while(pWord->token == TK_PLUS || pWord->token == TK_MINUS){
        auto current_operator = pWord->token;
        skip(pWord->token);
        multiply_division_expression();

        /*
         *  left operand value : %esp + 4
         *  right operand value : %esp
         */

        // move right operand into eax
        emit("\tpopl %%eax");

        if( current_operator == TK_PLUS){
            emit("\taddl %%eax, (%%esp)");
        } else{
            emit("\tsubl %%eax, (%%esp)");
        }
    }
}

void multiply_division_expression(){
    unary_expression();

    while(pWord->token == TK_STAR || pWord->token == TK_DIVIDE){
        auto current_operator = pWord->token;
        skip(pWord->token);
        unary_expression();

        // move **left value** to eax
        emit("\tmovl 4(%%esp), %%eax");

        if( current_operator == TK_STAR){
            emit("\timull (%%esp)");
        } else{
            emit("\tcltd ");
            emit("\tidivl (%%esp)");
        }

        emit("\tsubl $8, %%esp");
        emit("\tpushl %%eax");
    }
}

void unary_expression(){
    if(pWord->token == TK_PLUS){

        // TODO : +3;

    } else if(pWord->token == TK_MINUS){

        // TODO : -x;

    } else if(pWord->token == TK_STAR){

        //TODO *x;

    } else{
        postfix_expression();
    }
}

void postfix_expression(){
    if(pWord->token == TK_NUMBER){
        emit("\tpushl $%d", pWord->convert2Number());
        skip(TK_NUMBER);
    }else if(pWord->token == TK_STR){
        std::string string_label;
        if(string_table.find(pWord->rawStr) == string_table.cend()){
            char buffer[256]; memset(buffer, 0, 256);
            sprintf(buffer, "str%d", string_index++);
            string_table.insert({pWord->rawStr, std::string(buffer)});
        }
        string_label = string_table[pWord->rawStr];
        char buffer[256]; memset(buffer, 0, 256);
        sprintf(buffer, "str_helper%d", label_index++);
        auto string_helper_label = std::string(buffer);

        // find the relative address of the string and put it on the stack
        emit("\tcall %s", string_helper_label.c_str());
        emit("%s:", string_helper_label.c_str());
        emit("\tpopl %%eax");
        emit("\tleal %s-%s(%%eax), %%eax", string_label.c_str(), string_helper_label.c_str());
        emit("\tpushl %%eax");
        skip(TK_STR);
    } else if(pWord->token == ID){
        auto tempWord = pWord; skip(ID);

        if(pWord->token == TK_LPA){
            //function call
            if(function_table.find(tempWord->rawStr) == function_table.cend()){
                error("no matching function %s", tempWord->rawStr.c_str());
            }
            function_argument_list(&function_table[tempWord->rawStr]);
        }
        else{
            // variable
            if(local_variable_table_stack.top().find(tempWord->rawStr) != local_variable_table_stack.top().cend()){
                pVariable = &local_variable_table_stack.top()[tempWord->rawStr];
            }else if(global_variable_table.find(tempWord->rawStr) != global_variable_table.cend()){
                pVariable = &global_variable_table[tempWord->rawStr];
            }else {
                error("undefined variable %s", tempWord->rawStr.c_str());
            }

            if(pVariable->getRelative_position() > 0){
                // function parameter
                current_variable_position = pVariable->getRelative_position() * 4 + 8;
            } else{
                // variable
                current_variable_position = pVariable->getRelative_position();
            }
            if(pWord->token == TK_LSB){
                //array
                skip(TK_LSB);
                auto local_variable_position = current_variable_position;
                expression();
                emit("\tpopl %%ecx");
                emit("\tmovl %d(%%ebp, %%ecx, 4), %%eax", local_variable_position);
                emit("\tpushl %%eax");
                isArray = 1;
                current_variable_position = local_variable_position;
                skip(TK_RSB);
            }
            else{
                emit("\tmovl %d(%%ebp), %%eax", current_variable_position);
                emit("\tpushl %%eax", current_variable_position);
            }
        }
    }
}

static void function_argument_list(Function *callee){
    skip(TK_LPA);

    // allocate space for function call
    if(callee->getName() == "_printf"){
        // mac align conversion : 16 bytes
        emit("\tsubl $%d, %%esp", 8 + 16 - (8 + pFunction->getAllocated_size() + 8) % 16 );
    } else{
        emit("\tsubl $%d, %%esp", callee->getParameter_number() * 4);
    }
    int i = 0;
    while (true){
        expression();
        emit("\tpopl %%eax");
        emit("\tmovl %%eax, %d(%%esp)", i);
        if(pWord->token != TK_COMMAS) break;
        skip(TK_COMMAS);
        i += 4;
    }
    skip(TK_RPA);
    emit("\tcall %s", callee->getName().c_str());
    // free space for function call
    if(callee->getName() == "_printf"){
        emit("\taddl $%d, %%esp", (8 + 16 - (8 + pFunction->getAllocated_size() + 8) % 16 ));
    } else{
        emit("\taddl $%d, %%esp", callee->getParameter_number() * 4);
    }
    emit("\tpushl %%eax");
}


// ----------------------- helper ------------------------------------------


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
