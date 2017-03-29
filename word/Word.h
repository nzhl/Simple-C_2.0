#ifndef LANGUAGE_1_TTKORD_H
#define LANGUAGE_1_TTKORD_H

#include <string>
#include <cstdlib>
#include <map>

enum Token{
    //operator
    STAR, DIVIDE, MOD,
    PLUS, MINUS,
    EQ, NEQ, LT, LEQ, GT, GEQ,
    ASSIGN,

    // left/right parenthesis ()
    LPA, RPA,
    // left/right square bracket : []
    LSB, RSB,
    // {}
    BEGIN, END,

    COMMA, SEMICOLON, TK_EOF,

    // literals
    NUMBER, STR,

    INT, CHAR, VOID,
    IF, ELSE, WHILE, CONTINUE, BREAK, RETURN,

    //identifier
    ID
};

enum Scope{ Local, Global };

enum Type{ T_INT = 0, T_CHAR, T_PTR };

enum Property{ FUNCTION, VARIABLE };

class Word;
using PWord = Word*;

class Word {
private:
    static Word temp;

    Word(int token = -1, const std::string &rawStr = "")
            : token(token), rawStr(rawStr), range(Local){}

public:
    static PWord GetWord(int token = -1, const std::string &rawStr = "") {
        if (token == ID) {
            if (table.find(rawStr) == table.end()) {
                table.insert({rawStr, Word(token, rawStr)});
            }
            return &(table.find(rawStr)->second);
        }
        temp = Word(token, rawStr);
        return &temp;
    }



    int convert2Number() const {
        return static_cast<int>(atof(rawStr.c_str()));
    }

public:
    static std::map<std::string, Word> table;
    int token;
    std::string rawStr;

    // used by the parser

    int range; // local /  global

    int type; // int / char / ptr

    int property;  // function / variable

    int position;
};

#endif //LANGUAGE_1_TTKORD_H
