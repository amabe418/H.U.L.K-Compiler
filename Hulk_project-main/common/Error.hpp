#pragma once
#include <string>
#include "../include/Lexer/Token.hpp"

using namespace std;

struct CompilerError {
    int line;
    int column;
    std::string message;
    
    CompilerError(int l, int c, const std::string &msg)
        : line(l), column(c), message(msg) { }
};

#include <vector>
#include <iostream>

class ErrorHandler {
private:
    std::vector<CompilerError> errors;
public:
    void reportError(int line, int column, const std::string &msg) {
        errors.push_back(CompilerError(line, column, msg));
    }
    
    void reportError(const Token &token, const std::string &msg) {
        reportError(token.line, token.column, msg);
    }
    
    bool hasErrors() const {
        return !errors.empty();
    }
    
    void printErrors() const {
        for (const CompilerError &error : errors) {
            std::cerr << "Error [line " << error.line << ", col " << error.column << "]: " << error.message << std::endl;
        }
    }
    
    const std::vector<CompilerError>& getErrors() const {
        return errors;
    }
    
    void clear() {
        errors.clear();
    }
};
