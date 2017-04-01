#ifndef LANGUAGE_1_WORD_H
#define LANGUAGE_1_WORD_H

#include <string>
#include <cstdlib>
#include <map>

enum Token{
    //operator
    TK_STAR, TK_DIVIDE, TK_MOD,
    TK_PLUS, TK_MINUS,
    TK_EQ, TK_NEQ, TK_LT, TK_LEQ, TK_GT, TK_GEQ,
    TK_ASSIGN,

    // left/right parenthesis ()
    TK_LPA, TK_RPA,
    // left/right square bracket : []
    TK_LSB, TK_RSB,
    // {}
    TK_BEGIN, TK_END,

    TK_COMMAS, TK_SEMICOLON, TK_EOF,

    // literals
    TK_NUMBER, TK_STR,

    // keywords
    TK_INT, TK_CHAR, TK_VOID, TK_IF, TK_ELSE, TK_WHILE, TK_CONTINUE, TK_BREAK, TK_RETURN,

    //identifier
    ID
};


class Word;
using PWord = Word*;

class Word {
private:
    static Word temp;

    Word(int token = -1, const std::string &rawStr = "")
            : token(token), rawStr(rawStr){}

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
};

#endif //LANGUAGE_1_WORD_H
