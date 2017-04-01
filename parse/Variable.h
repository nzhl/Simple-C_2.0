#ifndef LANGUAGE_2_VARIABLE_H
#define LANGUAGE_2_VARIABLE_H


#include <string>

class Variable;
using PVariable = Variable *;

class Variable {
private:
    int type;
    std::string name;
    int ralative_position;

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

    int getRalative_position() const {
        return ralative_position;
    }

    void setRalative_position(int ralative_position) {
        Variable::ralative_position = ralative_position;
    }
};


#endif //LANGUAGE_2_VARIABLE_H
