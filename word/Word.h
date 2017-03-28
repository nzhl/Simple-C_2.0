#ifndef LANGUAGE_1_TTKORD_H
#define LANGUAGE_1_TTKORD_H

#include <string>
#include <cstdlib>
#include <map>

enum Token{
    //operator
    TK_PLUS,
    TK_MINUS,
    TK_STAR,
    TK_DIVIDE,
    TK_MOD,
    TK_EQ,
    TK_NEQ,
    TK_LT,
    TK_LEQ,
    TK_GT,
    TK_GEQ,
    TK_ASSIGN,

    // left/right parenthesis ()
    TK_LPA, TK_RPA,
    // left/right square bracket : []
    TK_LSB, TK_RSB,
    // {}
    TK_BEGIN, TK_END,

    TK_COMMA, TK_SEMICOLON, TK_EOF,

    // literals
    LT_NUMBER, LT_STR,

    TK_INT, TK_VOID,

    TK_IF, TK_ELSE, TK_CONTINUE, TK_BREAK, TK_RETURN,

    //identifier
    ID
};


class Word{
private:
    const static std::map<std::string, int> preservedTable;
public:
    int token;
    std::string rawStr;

    /////////////////////
    // used by the parser
    //////////////////////

    // local /  global
    int range;

    // int / char / ptr
    int type;

    int value;

    Word(int token = -1, const std::string &rawStr="")
            : token(token), rawStr(rawStr) {

        if(token == ID && preservedTable.find(rawStr) != preservedTable.end()){
            // preserved words
            this->token = preservedTable.find(rawStr)->second;
        }
    }

    int convert2Number() const {
        return static_cast<int>(atof(rawStr.c_str()));
    }
};

#endif //LANGUAGE_1_TTKORD_H
