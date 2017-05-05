cmake_minimum_required(VERSION 3.6)
project(Language_2)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        lex.cpp
        lex.h
        parse/parser.cpp
        parse/parser.h
        nzhl_c_compiler.cpp
        word/Word.h
        word/Word.cpp
#        vm.h vm.cpp
        io/io.cpp io/io.h parse/Function.h parse/Variable.h code_style_specification.h nzhl_c_compiler.h)

add_executable(Language_2 ${SOURCE_FILES})