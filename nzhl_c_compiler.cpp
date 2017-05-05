//  file related
#include "code_style_specification.h"

//  C header
#include <cstdio>
#include <cstdlib>

//  C++ header
#include <string>

//  third-party


//  project related
#include "lex.h"


//---------------------------------------------

FILE *OpenInputFile(std::string file_name){
    FILE *file_pointer = fopen(file_name.c_str(), "r");
    if(file_pointer == nullptr){
        printf("Can't open the file %s !\n", file_name.c_str());
        exit(0);
    }
    return file_pointer;
}


int main(int argc, char* argv[]) {
    //TODO : parse the parameter, set flag.
    std::string input_file_name;
    FILE *input_file_pointer = nullptr;

    input_file_name = argv[1];

    input_file_pointer = OpenInputFile(input_file_name);
    // pass

    // pre_process();

    lex(input_file_pointer);

    //  parse();

    //  code_generate();

}