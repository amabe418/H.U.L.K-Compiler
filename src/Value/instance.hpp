#pragma once

#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <memory>
#include <string>

// Forward declarations
struct TypeDecl;
struct EnvFrame;

struct Instance
{
    std::shared_ptr<EnvFrame> attrs;
    TypeDecl *typeDef;
    std::shared_ptr<Instance> self; // Referencia a sí mismo

    Instance() : attrs(nullptr), typeDef(nullptr), self(nullptr) {}

    // Constructor with EnvFrame initialization
    Instance(std::shared_ptr<EnvFrame> env) : attrs(env), typeDef(nullptr), self(nullptr) {}
};

#endif // INSTANCE_HPP