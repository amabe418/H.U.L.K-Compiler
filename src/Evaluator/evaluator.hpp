// evaluator.hpp
#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <stdio.h>

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include "../AST/ast.hpp"
#include "../Value/enumerable.hpp"
#include "../Value/iterable.hpp"
#include "../Value/value.hpp"
#include "env_frame.hpp"
struct Instance;

struct Instance
{
    std::shared_ptr<EnvFrame> attrs;
    TypeDecl *typeDef;
    std::shared_ptr<Instance> self; // Referencia a sí mismo

    Instance() : attrs(std::make_shared<EnvFrame>(nullptr)), typeDef(nullptr), self(nullptr) {}
};

struct EvaluatorVisitor : StmtVisitor, ExprVisitor
{
    Value lastValue{0.0};
    std::shared_ptr<Instance> currentSelf;

    std::shared_ptr<EnvFrame> env;
    std::unordered_map<std::string, FunctionDecl *> functions;
    std::unordered_map<std::string, TypeDecl *> types;
    std::shared_ptr<EnvFrame> globalEnv;
    std::string currentMethodName;
    TypeDecl *currentType = nullptr; // Para seguimiento en métodos

    EvaluatorVisitor()
    {
        globalEnv = env = std::make_shared<EnvFrame>(nullptr);
        types["Object"] = new TypeDecl("Object", {}, {}, {}, {}, "Object", {});

        // Agregar constantes matemáticas al entorno global
        globalEnv->locals["PI"] = Value(M_PI);
        globalEnv->locals["E"] = Value(M_E);
    }

    // Programa: recorre stmt a stmt
    void
    visit(Program *p) override
    {
        // Primero registrar TODAS las funciones
        for (auto &s : p->stmts)
        {
            if (auto *fd = dynamic_cast<FunctionDecl *>(s.get()))
            {
                fd->accept(this); // esto registra la función en el mapa
            }
        }

        // Luego ejecutar todo (incluyendo funciones si hay recursión)
        for (auto &s : p->stmts)
        {
            if (!dynamic_cast<FunctionDecl *>(s.get()))
            {
                s->accept(this);
            }
        }
    }

    // StmtVisitor:

    void
    visit(ExprStmt *e) override
    {
        e->expr->accept(this);
    }

    // ExprVisitor:
    void
    visit(NumberExpr *e) override
    {
        lastValue = Value(e->value);
    }

    void
    visit(StringExpr *e) override
    {
        lastValue = Value(e->value);
    }

    void
    visit(BooleanExpr *expr) override
    {
        lastValue = Value(expr->value);
    }

    void
    visit(UnaryExpr *e) override
    {
        e->operand->accept(this);
        lastValue = Value(-lastValue.asNumber());
    }

    void
    visit(BinaryExpr *e) override
    {
        e->left->accept(this);
        Value l = lastValue;
        e->right->accept(this);
        Value r = lastValue;

        // Debug: imprimir los valores de la expresión binaria
        std::cout << "Debug: BinaryExpr " << l.toString() << " " << e->op << " " << r.toString()
                  << std::endl;

        if (l.isInstance() || r.isInstance())
        {
            throw std::runtime_error("no compararemos instancias aun.");
        }
        switch (e->op)
        {
        case BinaryExpr::OP_ADD:
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error("ambos miembros en una suma deben ser numeros");
            }
            lastValue = Value(l.asNumber() + r.asNumber());
            break;
        case BinaryExpr::OP_SUB:
            std::cout << "[DEBUG] BinaryExpr: OP_SUB" << std::endl;
            std::cout << "[DEBUG] l: " << l.toString() << std::endl;
            std::cout << "[DEBUG] r: " << r.toString() << std::endl;
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error("ambos miembros en una resta deben ser numeros");
            }
            lastValue = Value(l.asNumber() - r.asNumber());
            break;
        case BinaryExpr::OP_MUL:
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error(
                    "ambos miembros en una multiplicacion deben ser numeros");
            }
            lastValue = Value(l.asNumber() * r.asNumber());
            break;
        case BinaryExpr::OP_DIV:
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error("ambos miembros en una division deben ser numeros");
            }
            lastValue = Value(l.asNumber() / r.asNumber());
            break;
        case BinaryExpr::OP_MOD:
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error(
                    "ambos miembros en una operacion de resto deben ser numeros");
            }
            lastValue = Value(fmod(l.asNumber(), r.asNumber()));
            break;
        case BinaryExpr::OP_POW:
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error("ambos miembros en una potencia deben ser numeros");
            }
            lastValue = Value(pow(l.asNumber(), r.asNumber()));
            break;
        case BinaryExpr::OP_LT:
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error("ambos miembros en una comparacion deben ser numeros");
            }
            lastValue = Value(l.asNumber() < r.asNumber() ? true : false);
            break;
        case BinaryExpr::OP_GT:
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error("ambos miembros en una comparacion deben ser numeros");
            }
            lastValue = Value(l.asNumber() > r.asNumber() ? true : false);
            break;
        case BinaryExpr::OP_LE:
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error("ambos miembros en una comparacion deben ser numeros");
            }
            lastValue = Value(l.asNumber() <= r.asNumber() ? true : false);
            break;
        case BinaryExpr::OP_GE:
            if (!l.isNumber() || !r.isNumber())
            {
                throw std::runtime_error("ambos miembros en una comparacion deben ser numeros");
            }
            lastValue = Value(l.asNumber() >= r.asNumber() ? true : false);
            break;
        case BinaryExpr::OP_EQ:
            if (l.isNumber() && r.isNumber())
            {
                lastValue = Value(l.asNumber() == r.asNumber() ? true : false);
            }
            else if (l.isString() && r.isString())
            {
                lastValue = Value(l.asString() == r.asString() ? true : false);
            }
            else if (l.isBool() && r.isBool())
            {
                lastValue = Value(l.asBool() == r.asBool() ? true : false);
            }
            else
            {
                throw std::runtime_error(
                    "ambos miembros en una comparacion deben ser del mismo tipo");
            }
            break;
        case BinaryExpr::OP_NEQ:
            if (l.isNumber() && r.isNumber())
            {
                lastValue = Value(l.asNumber() != r.asNumber() ? true : false);
            }
            else if (l.isString() && r.isString())
            {
                lastValue = Value(l.asString() != r.asString() ? true : false);
            }
            else if (l.isBool() && r.isBool())
            {
                lastValue = Value(l.asBool() != r.asBool() ? true : false);
            }
            else
            {
                throw std::runtime_error(
                    "ambos miembros en una comparacion deben ser del mismo tipo");
            }
            break;
        case BinaryExpr::OP_OR:
            if (!l.isBool() || !r.isBool())
                throw std::runtime_error("or requiere booleanos");
            lastValue = Value(l.asBool() || r.asBool() ? true : false);
            break;
        case BinaryExpr::OP_AND:
            if (!l.isBool() || !r.isBool())
                throw std::runtime_error("and requiere booleanos");
            lastValue = Value(l.asBool() && r.asBool() ? true : false);
            break;
        case BinaryExpr::OP_CONCAT:
        {
            std::string ls = l.toString();
            std::string rs = r.toString();
            lastValue = Value(ls + rs);
        }
        break;
        case BinaryExpr::OP_CONCAT_WS:
        {
            std::string ls = l.toString();
            std::string rs = r.toString();
            lastValue = Value(ls + " " + rs);
        }
        break;
        default:
            throw std::runtime_error("Operador desconocido");
        }
    }

    void
    visit(CallExpr *e) override
    {
        // std::cout << "Entrando a CallExpr: " << e->callee << std::endl;
        std::vector<Value> args;
        for (auto &arg : e->args)
        {
            arg->accept(this);
            args.push_back(lastValue);
        }

        // Funciones definidas por el usuario
        auto it = functions.find(e->callee);
        if (it != functions.end())
        {
            FunctionDecl *f = it->second;

            if (f->params.size() != args.size())
            {
                throw std::runtime_error("Número incorrecto de argumentos para función: " +
                                         f->name);
            }

            // Guardar entorno actual
            auto oldEnv = env;
            env = std::make_shared<EnvFrame>(oldEnv);

            // Asignar parámetros
            for (size_t i = 0; i < f->params.size(); ++i)
            {
                env->locals[f->params[i]] = args[i];
            }

            // Evaluar cuerpo
            f->body->accept(this);

            // Restaurar entorno
            env = std::move(oldEnv);

            return;
        }

        // Funciones nativas del lenguaje
        if (e->callee == "range")
        {
            if (args.size() != 2 || !args[0].isNumber() || !args[1].isNumber())
            {
                throw std::runtime_error("range() espera 2 argumentos numéricos");
            }
            double start = args[0].asNumber();
            double end = args[1].asNumber();
            auto rv = std::make_shared<RangeValue>(start, end);
            lastValue = Value(rv);
            return;
        }
        else if (e->callee == "iter")
        {
            if (args.size() != 1)
            {
                throw std::runtime_error("iter() espera 1 argumento");
            }
            if (args[0].isRange())
            {
                auto rv = args[0].asRange();
                auto itr = rv->iter();
                lastValue = Value(itr);
                return;
            }
            throw std::runtime_error("iter(): el argumento no es Enumerable");
        }
        else if (e->callee == "next")
        {
            if (args.size() != 1 || !args[0].isIterable())
            {
                throw std::runtime_error("next() espera 1 argumento Iterable");
            }
            auto itr = args[0].asIterable();
            bool hay = itr->next();
            lastValue = Value(hay);
            return;
        }
        else if (e->callee == "current")
        {
            if (args.size() != 1 || !args[0].isIterable())
            {
                throw std::runtime_error("current() espera 1 argumento Iterable");
            }
            auto itr = args[0].asIterable();
            lastValue = itr->current();
            return;
        }
        else if (e->callee == "print")
        {
            if (args.empty())
                throw std::runtime_error("print espera al menos 1 argumento");
            std::cout << args[0] << std::endl;
            lastValue = args[0]; // Asegurarnos de que print retorne el valor que imprime
            return;
        }
        else if (e->callee == "sqrt")
        {
            if (args.size() != 1)
                throw std::runtime_error("sqrt() espera 1 argumento");
            lastValue = Value(std::sqrt(args[0].asNumber()));
            return;
        }
        else if (e->callee == "log")
        {
            if (args.size() == 1)
            {
                lastValue = Value(std::log(args[0].asNumber()));
                return;
            }
            else if (args.size() == 2)
            {
                double base = args[0].asNumber();
                double x = args[1].asNumber();
                if (base <= 0 || base == 1)
                    throw std::runtime_error("Base inválida para log()");
                if (x <= 0)
                    throw std::runtime_error("Argumento inválido para log()");
                lastValue = Value(std::log(x) / std::log(base));
                return;
            }
            else
            {
                throw std::runtime_error("log() espera 1 o 2 argumentos");
            }
        }
        else if (e->callee == "sin")
        {
            if (args.size() != 1)
                throw std::runtime_error("sin() espera 1 argumento");
            lastValue = Value(std::sin(args[0].asNumber()));
            return;
        }
        else if (e->callee == "cos")
        {
            if (args.size() != 1)
                throw std::runtime_error("cos() espera 1 argumento");
            lastValue = Value(std::cos(args[0].asNumber()));
            return;
        }
        else if (e->callee == "rand")
        {
            lastValue = Value(static_cast<double>(rand()) / RAND_MAX);
            return;
        }
        else
        {
            // Intentar buscar la constante en el entorno global
            try
            {
                lastValue = globalEnv->get(e->callee);
                return;
            }
            catch (const std::runtime_error &)
            {
                throw std::runtime_error("Función o constante desconocida: " + e->callee);
            }
        }
    }

    // for variable declarations
    void
    visit(VariableExpr *expr) override
    {
        // get() buscará en este frame y en los padres
        lastValue = env->get(expr->name);
    }

    // let in expressions
    void
    visit(LetExpr *expr) override
    {
        // 1) Evaluar la expresión del inicializador
        expr->initializer->accept(this);
        Value initVal = lastValue;

        // 2) Abrir un nuevo frame (scope hijo)
        auto oldEnv = env; // guardar el frame padre
        env = std::make_shared<EnvFrame>(oldEnv);

        // 3) Insertar la variable en el mapa local
        env->locals[expr->name] = initVal;

        // 4) Evaluar el cuerpo (es un Stmt)
        expr->body->accept(static_cast<StmtVisitor *>(this));
        Value result = lastValue;

        // 5) Al salir, restaurar el frame anterior
        env = std::move(oldEnv);

        // 6) El valor resultante de la expresión let es el valor devuelto
        lastValue = result;
    }

    // destructive assignment
    void
    visit(AssignExpr *expr) override
    {
        // Antes de asignar, evaluamos la expresión de la derecha:
        expr->value->accept(this);
        Value newVal = lastValue;

        // Verificar que exista en alguna parte (no crear nuevas automáticamente):
        if (!env->existsInChain(expr->name))
        {
            throw std::runtime_error("No se puede asignar a variable no declarada: " + expr->name);
        }
        // Llamamos a set() para que reasigne en el frame correspondiente:
        env->set(expr->name, newVal);
        lastValue = newVal; // Devolvemos el nuevo valor asignado
    }

    // functions declaration
    void
    visit(FunctionDecl *f) override
    {
        if (functions.count(f->name))
            throw std::runtime_error("Funcion ya definida: " + f->name);
        functions[f->name] = f;
    }

    // if-else
    void
    visit(IfExpr *e) override
    {
        e->condition->accept(this);
        if (!lastValue.isBool())
        {
            throw std::runtime_error("La condición de un if debe ser booleana");
        }

        if (lastValue.asBool())
        {
            e->thenBranch->accept(this);
        }
        else
        {
            e->elseBranch->accept(this);
        }
    }

    void
    visit(ExprBlock *b) override
    {
        // std::cout << "Entrando a ExprBlock" << std::endl;
        // 1) Abrir un nuevo frame (scope hijo) antes de entrar al bloque
        auto oldEnv = env;
        env = std::make_shared<EnvFrame>(oldEnv);

        // 2) Evaluar cada sentencia dentro del bloque con este nuevo frame
        for (auto &stmt : b->stmts)
        {
            // std::cout << "Evaluando statement en bloque" << std::endl;
            stmt->accept(this);
        }

        // 3) Restaurar el frame anterior al salir del bloque
        env = std::move(oldEnv);
        // std::cout << "Saliendo de ExprBlock" << std::endl;
        // Mantenemos el último valor evaluado
    }

    void
    visit(WhileExpr *expr) override
    {
        Value result;
        while (true)
        {
            expr->condition->accept(this);
            if (!lastValue.isBool())
                throw std::runtime_error("La condición de un while debe ser booleana");
            if (!lastValue.asBool())
                break;

            expr->body->accept(this);
            result = lastValue; // Guardamos el último valor evaluado
        }
        lastValue = result; // Devolvemos el último valor en lugar de void
    }

    void
    visit(ForExpr *expr) override
    {
        // TODO: Implementar evaluación del ciclo for
        // Por ahora, dejamos vacío para que compile
        // El for se puede implementar transpilando a while como se mencionó en la documentación
    }

    void
    visit(TypeDecl *t) override
    {
        if (types.count(t->name))
        {
            throw std::runtime_error("Tipo redefinido: " + t->name);
        }
        types[t->name] = t;
    }

    void
    visit(MethodDecl *m) override
    {
        // Los métodos se registran en el tipo, no en el entorno
    }

    void
    visit(NewExpr *e) override
    {
        auto instance = std::make_shared<Instance>();
        TypeDecl *type = types.at(e->typeName);
        instance->typeDef = type;
        instance->attrs = std::make_shared<EnvFrame>(globalEnv);
        instance->self = instance; // Establecer la referencia a sí mismo

        // 1. Evaluar argumentos de construcción
        {
            const auto &paramNames = type->getParams();
            if (paramNames.size() != e->args.size())
                throw std::runtime_error("Número de argumentos inválido para " + e->typeName);

            env = instance->attrs;
            for (size_t i = 0; i < paramNames.size(); ++i)
            {
                e->args[i]->accept(this);
                env->locals[paramNames[i]] = lastValue;
            }
        }

        // 2. Si hereda de otro tipo, instanciar atributos del padre
        if (type->baseType != "Object")
        {
            TypeDecl *base = types.at(type->baseType);
            auto baseEnv = std::make_shared<EnvFrame>(instance->attrs);

            // Verificar que cantidad de argumentos coincida
            if (base->getParams().size() != type->baseArgs.size())
                throw std::runtime_error("Número incorrecto de argumentos en llamada a base: " +
                                         type->baseType);

            // Evaluar argumentos del base usando el entorno actual
            env = baseEnv;
            for (size_t i = 0; i < base->getParams().size(); ++i)
            {
                type->baseArgs[i]->accept(this);
                env->locals[base->getParams()[i]] = lastValue;
            }

            // Inicializar atributos del padre
            for (auto &attr : base->attributes)
            {
                attr->initializer->accept(this);
                instance->attrs->locals[attr->name] = lastValue;
            }
        }

        // 3. Inicializar atributos del tipo actual
        env = instance->attrs;
        for (auto &attr : type->attributes)
        {
            attr->initializer->accept(this);
            env->locals[attr->name] = lastValue;
        }

        lastValue = Value(instance);
    }

    void
    visit(SelfExpr *expr) override
    {
        if (!currentSelf)
        {
            throw std::runtime_error("Cannot use self outside of class methods");
        }
        lastValue = Value(currentSelf);
    }

    void
    visit(GetAttrExpr *e) override
    {
        e->object->accept(this);
        auto object = lastValue;
        if (auto instance = std::dynamic_pointer_cast<Instance>(object.asInstance()))
        {
            try
            {
                auto val = instance->attrs->get(e->attrName);
                std::cout << "[DEBUG] GetAttrExpr: instance attr '" << e->attrName
                          << "' = " << val.toString() << std::endl;
                lastValue = val;
            }
            catch (const std::runtime_error &)
            {
                throw std::runtime_error("Undefined attribute: " + e->attrName);
            }
        }
        else if (auto selfExpr = dynamic_cast<SelfExpr *>(e->object.get()))
        {
            if (!currentSelf)
                throw std::runtime_error("Cannot use self outside of class methods");
            try
            {
                auto val = currentSelf->attrs->get(e->attrName);
                std::cout << "[DEBUG] GetAttrExpr: self attr '" << e->attrName
                          << "' = " << val.toString() << std::endl;
                lastValue = val;
            }
            catch (const std::runtime_error &)
            {
                throw std::runtime_error("Undefined attribute: " + e->attrName);
            }
        }
        else
        {
            throw std::runtime_error("Cannot access attributes on non-class instance");
        }
    }

    void
    visit(SetAttrExpr *e) override
    {
        // Evaluar el objeto primero y GUARDAR el resultado
        e->object->accept(this);
        auto instanceVal = lastValue;
        auto instance = instanceVal.asInstance(); // sin dynamic cast

        // Evaluar el valor a asignar
        e->value->accept(this);
        auto newValue = lastValue;

        // Validar y asignar
        if (!instance)
            throw std::runtime_error("SetAttrExpr: objeto no es una instancia válida");

        auto currentValue = instance->attrs->get(e->attrName);
        std::cout << "[DEBUG] SetAttrExpr: instance attr '" << e->attrName
                  << "' before = " << currentValue.toString() << std::endl;

        if (newValue.isNumber())
        {
            instance->attrs->set(e->attrName, newValue);
            std::cout << "[DEBUG] SetAttrExpr: instance attr '" << e->attrName
                      << "' after = " << newValue.toString() << std::endl;
            lastValue = newValue;
        }
        else
        {
            throw std::runtime_error("Cannot assign non-numeric value to attribute");
        }
    }

    void
    visit(BaseCallExpr *e) override
    {
        std::cout << "[DEBUG] BaseCallExpr: currentMethodName = '" << currentMethodName << "'" << std::endl;

        auto selfVal = env->get("self");
        if (!selfVal.isInstance())
            throw std::runtime_error("base() requiere instancia");

        auto instance = selfVal.asInstance();
        if (currentMethodName.empty())
            throw std::runtime_error("base() fuera de contexto de método");

        // Buscar el método base en la cadena de herencia (excluyendo el tipo actual)
        TypeDecl *current = instance->typeDef;
        MethodDecl *method = nullptr;
        if (current->baseType != "Object")
        {
            auto it = types.find(current->baseType);
            if (it != types.end())
            {
                method = findMethod(it->second, currentMethodName, types);
            }
        }
        if (!method)
            throw std::runtime_error("base(): método base no encontrado");

        // Guardar entorno y self anteriores
        auto oldEnv = env;
        auto oldSelf = currentSelf;

        // Establecer entorno del método base y currentSelf
        env = std::make_shared<EnvFrame>(instance->attrs);
        currentSelf = instance;
        env->locals["self"] = Value(instance);

        // Inyectar argumentos como variables locales (usando los valores actuales en env)
        for (const auto &param : method->params)
        {
            if (env->locals.find(param) == env->locals.end() && oldEnv->locals.find(param) != oldEnv->locals.end())
            {
                env->locals[param] = oldEnv->locals[param];
            }
        }

        // Ejecutar el cuerpo del método base
        method->body->accept(this);
        Value result = lastValue;

        // Restaurar entorno y self anteriores
        env = oldEnv;
        currentSelf = oldSelf;

        lastValue = result;
    }

    // Búsqueda recursiva de métodos en la cadena de herencia
    MethodDecl *findMethod(TypeDecl *type, const std::string &methodName, const std::unordered_map<std::string, TypeDecl *> &types)
    {
        // First, search in the current type (most specific)
        for (const auto &m : type->methods)
        {
            if (m->name == methodName)
                return m.get();
        }

        // If not found in current type, search in inheritance chain
        if (type->baseType != "Object")
        {
            auto it = types.find(type->baseType);
            if (it != types.end())
                return findMethod(it->second, methodName, types);
        }
        return nullptr;
    }

    void
    visit(MethodCallExpr *e) override
    {
        std::cout << "[DEBUG] MethodCallExpr: calling method '" << e->methodName << "'" << std::endl;

        // Evaluar el objeto de la llamada (ej: a.setx() → 'a')
        e->object->accept(this);
        auto object = lastValue;

        // Verifica que es una instancia
        auto instance = object.asInstance();
        if (!instance)
        {
            throw std::runtime_error("Cannot call methods on non-class instance");
        }

        // Busca el método en el tipo correspondiente (usando búsqueda recursiva)
        MethodDecl *method = findMethod(instance->typeDef, e->methodName, types);

        if (!method)
        {
            throw std::runtime_error("Undefined method: " + e->methodName);
        }

        // Guardar entorno y self anteriores
        auto oldEnv = env;
        auto oldSelf = currentSelf;
        auto oldMethodName = currentMethodName;

        // Establecer entorno del método y currentSelf
        env = std::make_shared<EnvFrame>(instance->attrs);
        currentSelf = instance;
        currentMethodName = e->methodName;     // Set current method name
        env->locals["self"] = Value(instance); // Add self to environment

        std::cout << "[DEBUG] MethodCallExpr: set currentMethodName to '" << currentMethodName << "'" << std::endl;

        // ⚠️ Evalúa los argumentos DESPUÉS de establecer el entorno y currentSelf
        std::vector<Value> args;
        for (const auto &arg : e->args)
        {
            arg->accept(this);
            args.push_back(lastValue);
        }

        // Inyectar argumentos como variables locales
        for (size_t i = 0; i < method->params.size(); ++i)
        {
            if (i < args.size())
            {
                env->locals[method->params[i]] = args[i];
            }
        }

        // Ejecutar el cuerpo del método
        method->body->accept(this);
        Value result = lastValue;

        // Restaurar entorno y self anteriores
        env = oldEnv;
        currentSelf = oldSelf;
        currentMethodName = oldMethodName;

        lastValue = result;
    }
    void visit(AttributeDecl *attr) override
    {
        // Evaluar el inicializador del atributo
        attr->initializer->accept(this);
        // El valor del atributo será el valor del inicializador
        // No necesitamos hacer nada más aquí ya que los atributos se manejan en NewExpr
    }
};
#endif