#include "Word.h"

std::map<std::string, Word> Word::table = {
        {std::string("int"), Word(TK_INT, "int")},
        {std::string("char"), Word(TK_CHAR, "char")},
        {std::string("void"), Word(TK_VOID, "void")},
        {std::string("if"), Word(TK_IF, "if")},
        {std::string("else"), Word(TK_ELSE, "else")},
        {std::string("while"), Word(TK_WHILE, "while")},
        {std::string("continue"), Word(TK_CONTINUE, "continue")},
        {std::string("break"), Word(TK_BREAK, "break")},
        {std::string("return"), Word(TK_RETURN, "return")},
};

Word Word::temp;

