#ifndef LANGUAGE_2_VARIABLE_H
#define LANGUAGE_2_VARIABLE_H


#include <string>

class Variable;
using PVariable = Variable *;

class Variable {
private:
    int type;
    std::string name;
    int relative_position;
    int size;
public:
    int getType() const {
        return type;
    }

    void setType(int type) {
        Variable::type = type;
    }

    const std::string &getName() const {
        return name;
    }

    void setName(const std::string &name) {
        Variable::name = name;
    }

    int getRelative_position() const {
        return relative_position;
    }

    void setRelative_position(int ralative_position) {
        Variable::relative_position = ralative_position;
    }

    int getSize() const {
        return size;
    }

    void setSize(int size) {
        Variable::size = size;
    }
};


#endif //LANGUAGE_2_VARIABLE_H
