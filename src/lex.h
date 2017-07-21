#ifndef LEX_H
#define LEX_H

typedef struct {
    int kind;
    File *file;
    int line;
    int column;
    // true if the token has a leading space
    bool leading_space;
    // true if the token is at the beginning of a line
    bool beginning_of_line;
    // token number in a file, counting from 0.
    int count;
    // used by the preprocessor for macro expansion
    Set *hideset;
    union {
        // TKEYWORD
        int id;
        // TSTRING or TCHAR
        struct {
            char *sval;
            int slen;
            int c;
            int enc;
        };
        // TMACRO_PARAM
        struct {
            bool is_vararg;
            int position;
        };
    };
} Token;

#endif
