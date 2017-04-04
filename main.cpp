#include "parse/parser.h"

extern char* FilePath;

int main(int argc, char* argv[]) {
    FilePath = argv[1];
    program();
}