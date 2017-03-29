#include "Word.h"

std::map<std::string, Word> Word::table = {
        {std::string("int"), Word(INT, "int")},
        {std::string("char"), Word(CHAR, "char")},
        {std::string("void"), Word(VOID, "void")},
        {std::string("if"), Word(IF, "if")},
        {std::string("else"), Word(ELSE, "else")},
        {std::string("while"), Word(WHILE, "while")},
        {std::string("continue"), Word(CONTINUE, "continue")},
        {std::string("break"), Word(BREAK, "break")},
        {std::string("return"), Word(RETURN, "return")},
};

Word Word::temp;

