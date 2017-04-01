
#ifndef LANGUAGE_2_FUNCTION_H
#define LANGUAGE_2_FUNCTION_H


#include <string>
#include <vector>

class Function;
using PFunction = Function *;

class Function {
private:
    int return_type;
    std::string name;
    std::vector<int> parameters;
    int variable_number;

public:
    int getReturn_type() const {
        return return_type;
    }

    void setReturn_type(int return_type) {
        Function::return_type = return_type;
    }

    const std::string &getName() const {
        return name;
    }

    void setName(const std::string &name) {
        Function::name = name;
    }

     std::vector<int, std::allocator<int>> &getParameters() {
        return parameters;
    }

    int getVariable_number() const {
        return variable_number;
    }

    void setVariable_number(int variable_number) {
        Function::variable_number = variable_number;
    }

    int getParameter_number() const {
        return static_cast<int>(parameters.size());
    }
};


#endif //LANGUAGE_2_FUNCTION_H
