#include "io.h"
#include "../word/Word.h"

#include <iostream>

extern int current_line;
extern Word word;
extern int ch;

static const char SpaceCharList[] = " \n\t\r";

static void parse_comment_and_space();
static void parse_identifier();
static void parse_number();
static void parse_string();


void get_next_token(){
    parse_comment_and_space();
    if(isalpha(ch) || ch == '_'){
        // id : [a-zA-Z_][a-zA-Z_0-9]*
        parse_identifier();
    } else if(isdigit(ch)){
        parse_number();
    } else if(ch == '+'){
        word = Word(TK_PLUS, "+");
        get_next_char();
    } else if(ch == '-'){
        word = Word(TK_MINUS, "-");
        get_next_char();
    } else if(ch == '*'){
        word = Word(TK_STAR, "*");
        get_next_char();
    } else if(ch == '/'){
        word = Word(TK_DIVIDE, "/");
        get_next_char();
    } else if(ch == '%'){
        word = Word(TK_MOD, "/");
        get_next_char();
    } else if(ch == '='){
        get_next_char();
        if(ch == '='){
            word = Word(TK_EQ, "==");
            get_next_char();
        } else{
            word = Word(TK_ASSIGN, "=");
        }
    } else if(ch == '!'){
        get_next_char();
        if(ch == '='){
            word = Word(TK_NEQ, "!=");
            get_next_char();
        } else{
            error("Do not support `!` yet !");
        }
    } else if(ch == '<'){
        get_next_char();
        if(ch == '='){
            word = Word(TK_LEQ, "<=");
            get_next_char();
        } else {
            word = Word(TK_LT, "<");
        }
    } else if(ch == '>'){
        get_next_char();
        if(ch == '='){
            word = Word(TK_GEQ, ">=");
            get_next_char();
        } else {
            word = Word(TK_GT, ">");
        }
    } else if(ch == ';'){
        word = Word(TK_SEMICOLON, ";");
        get_next_char();
    } else if(ch == '('){
        word = Word(TK_LPA, "(");
        get_next_char();
    } else if(ch == ')'){
        word = Word(TK_RPA, ")");
        get_next_char();
    } else if(ch == '['){
        word = Word(TK_LSB, "[");
        get_next_char();
    } else if(ch == ']'){
        word = Word(TK_RSB, "]");
        get_next_char();
    } else if(ch == '{'){
        word = Word(TK_BEGIN, "{");
        get_next_char();
    } else if(ch == '}') {
        word = Word(TK_END, "}");
        get_next_char();
    } else if(ch == ',') {
        word = Word(TK_COMMA, ",");
        get_next_char();
    } else if(ch == '\"') {
        parse_string();
    } else if(ch == EOF) {
        word = Word(TK_EOF, "EOF");
    } else{
        error("Unknown character : %c !", ch);
    }
}
void init(){
    current_line = 1;
    get_next_token();
}


void lexDebug(){
    init();
    while (word.token != TK_EOF){
        std::cout << word.rawStr << std::endl;
        get_next_token();
    }
}



/**
 *  ignore all the  comment and space,
 *  ch keeps either EOF or a new char that
 *  have't parsed yet when the function return.
 */
void parse_comment_and_space(){
    while (true){
        if(strchr(SpaceCharList, ch)) {
            //space
            if (ch == '\n') ++current_line;
            get_next_char();
        } else if(ch == '/'){
            // comment
            get_next_char();

            if(ch == '/') {
                // one current_line comment
                while (ch != '\n' && ch != EOF){
                    get_next_char();
                }

                if(ch == '\n'){
                    get_next_char();
                    ++current_line;
                }
            } else if(ch == '*'){
                // multi-lines comment
                do{
                    get_next_char();
                    if(ch == '*'){
                        get_next_char();
                        if(ch == '/'){
                            get_next_char();
                            break;
                        } else{
                            return_current_char();
                        }
                    } else if(ch == '\n'){
                        ++current_line;
                    }
                } while (true);
            } else{
                return_current_char();
                ch = '/';
                break;
            }
        } else{
            break;
        }
    }
}


void parse_identifier(){
    std::string string = "";
    string += ch;
    do{
        get_next_char();
        if(isalnum(ch) || ch == '_'){
            string += ch;
        } else{
            break;
        }
    } while (true);
    word = Word(ID, string);
}

void parse_number(){
    std::string string = "";
    string += ch;
    do{
        get_next_char();
        if(isdigit(ch)){
            string += ch;

        } else{
            break;
        }
    } while (true);

    if('.' == ch){
        string += ch;
        do{
            get_next_char();
            if(isdigit(ch)){
                string += ch;

            } else{
                break;
            }
        } while (true);
    }

    word = Word(LT_NUMBER, string);
}

void parse_string(){
    auto startChar = ch;
    std::string string = "";
    do{
        get_next_char();
        if(startChar == ch){
            get_next_char();
            break;
        } else if(ch == '\\'){
            get_next_char();
            switch (ch){
                case '0':
                    string += '\0';
                    break;
                case 't':
                    string += '\t';
                    break;
                case 'n':
                    string += '\n';
                    break;
                case '\'':
                    string += '\'';
                    break;
                case '"':
                    string += '"';
                    break;
                case '\\':
                    string += '\\';
                    break;
                default:
                    string += ch;
                    error("Unknown character: \\%c", ch);
            }
        } else{
            string += ch;
        }
    } while (true);

    word = Word(LT_STR, string);
}

