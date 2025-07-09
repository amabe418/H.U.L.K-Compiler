#pragma once
#include <string>
#include <vector>
#include <iostream>

/**
 * @brief Types of semantic errors
 */
enum class ErrorType
{
    GENERAL_ERROR,
    TYPE_ERROR,
    UNDEFINED_VARIABLE,
    UNDEFINED_FUNCTION,
    UNDEFINED_TYPE,
    UNDEFINED_ATTRIBUTE,
    UNDEFINED_METHOD,
    REDEFINED_FUNCTION,
    REDEFINED_TYPE,
    REDEFINED_VARIABLE,
    REDEFINED_ATTRIBUTE,
    REDEFINED_METHOD,
    ARGUMENT_COUNT_MISMATCH,
    INVALID_SELF,
    INVALID_BASE,
    INVALID_SELF_ASSIGNMENT,
    INVALID_BASE_TYPE
};

/**
 * @brief Represents a semantic error with detailed location and context information
 */
struct SemanticError
{
    ErrorType type;
    std::string message;
    int line;
    int column;
    std::string context;
    std::string source;

    SemanticError(ErrorType t, const std::string &msg, int ln, int col,
                  const std::string &ctx, const std::string &src)
        : type(t), message(msg), line(ln), column(col), context(ctx), source(src) {}

    std::string format() const
    {
        std::string result = "Error Semántico en línea " + std::to_string(line);
        if (column > 0)
        {
            result += ", columna " + std::to_string(column);
        }

        if (!source.empty())
        {
            result += " (" + source + ")";
        }

        result += ": " + message;

        if (!context.empty())
        {
            result += " (en " + context + ")";
        }

        return result;
    }

    std::string getTypeString() const
    {
        switch (type)
        {
        case ErrorType::TYPE_ERROR:
            return "Error de tipos";
        case ErrorType::UNDEFINED_VARIABLE:
            return "Variable no definida";
        case ErrorType::UNDEFINED_FUNCTION:
            return "Función no definida";
        case ErrorType::UNDEFINED_TYPE:
            return "Tipo no definido";
        case ErrorType::UNDEFINED_ATTRIBUTE:
            return "Atributo no definido";
        case ErrorType::UNDEFINED_METHOD:
            return "Método no definido";
        case ErrorType::REDEFINED_FUNCTION:
            return "Función redefinida";
        case ErrorType::REDEFINED_TYPE:
            return "Tipo redefinido";
        case ErrorType::REDEFINED_VARIABLE:
            return "Variable redefinida";
        case ErrorType::REDEFINED_ATTRIBUTE:
            return "Atributo redefinido";
        case ErrorType::REDEFINED_METHOD:
            return "Método redefinido";
        case ErrorType::ARGUMENT_COUNT_MISMATCH:
            return "Número de argumentos inválido";
        case ErrorType::INVALID_SELF:
            return "Acceso a miembro inválido";
        case ErrorType::INVALID_BASE:
            return "Llamada a método inválida";
        case ErrorType::INVALID_SELF_ASSIGNMENT:
            return "Asignación inválida a self";
        case ErrorType::INVALID_BASE_TYPE:
            return "Herencia inválida de tipos";
        default:
            return "Error general";
        }
    }
};

/**
 * @brief Manager for collecting and reporting semantic errors
 */
class ErrorManager
{
private:
    std::vector<SemanticError> errors_;
    bool error_limit_reached_ = false;
    static const size_t MAX_ERRORS = 20; // Limit to prevent excessive output

public:
    void reportError(ErrorType type, const std::string &message, int line, int column = 0,
                     const std::string &context = "", const std::string &source = "")
    {
        if (errors_.size() >= MAX_ERRORS)
        {
            if (!error_limit_reached_)
            {
                errors_.emplace_back(ErrorType::GENERAL_ERROR,
                                     "Demasiados errores encontrados. Se omiten errores adicionales.",
                                     line, column, "", "ErrorManager");
                error_limit_reached_ = true;
            }
            return;
        }

        errors_.emplace_back(type, message, line, column, context, source);
    }

    bool hasErrors() const
    {
        return !errors_.empty();
    }

    const std::vector<SemanticError> &getErrors() const
    {
        return errors_;
    }

    void clear()
    {
        errors_.clear();
        error_limit_reached_ = false;
    }

    void printErrors(std::ostream &out = std::cerr) const
    {
        for (const auto &error : errors_)
        {
            out << error.format() << std::endl;
        }

        if (hasErrors())
        {
            out << "\nTotal de errores encontrados: " << errors_.size() << std::endl;
        }
    }

    size_t getErrorCount() const
    {
        return errors_.size();
    }
};
