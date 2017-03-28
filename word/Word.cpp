#include "Word.h"

const std::map<std::string, int> Word::preservedTable = {
        {std::string("int"), TK_INT},
        {std::string("void"), TK_VOID},
        {std::string("if"), TK_IF},
        {std::string("else"), TK_ELSE},
        {std::string("continue"), TK_CONTINUE},
        {std::string("break"), TK_BREAK},
        {std::string("return"), TK_RETURN},
};
