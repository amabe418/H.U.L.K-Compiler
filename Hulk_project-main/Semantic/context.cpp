#include "../include/Semantic/Context.hpp"



void Type::DefineAttribute(std::string name,std::string type)
{
    atributes.emplace(name, new Attribute(name,type));
}

void Type::DefineArgument(std::string name,std::string type)
{
    arguments.push_back({name, new Attribute(name,type)});
}


bool Type::exist_Attribute(const std::string& name)
{
    return atributes.find(name)!=atributes.end();
}
    
bool Type::exist_Argument(const std::string& name)
{
    for(auto[argname,_] :arguments)
    {

        if(argname==name)   return true;

    }
    return false;
}


bool Type::exist_Method(const std::string& name)
{
    return methods.find(name)!=methods.end();
}


void Type::DefineMethod(std::string name,std::string type,std::vector<Attribute> paramaters)
{
    
    methods.emplace(name,new Method(name,paramaters,type));
    
}

Method* Type::GetMethod(const std::string& name)
{
    if(methods.find(name)!=methods.end())   return methods[name];
    if(parent==nullptr) return nullptr;
    return parent->GetMethod(name);
    
}



bool Type::isDescendantOf(const Type* other) const 
{
    const Type* current = this;
    while (current != nullptr) {
    if (current == other)
        return true;
    current = current->parent;
    }
    return false;
}

bool Type::operator==(const Type &other) const {
    return name == other.name;
}

bool Type::operator<(const Type &other) const {
    // Si son iguales, no se considera menor.
    if (*this == other)
        return false;

    // Si este tipo es descendiente de 'other', se considera menor.
    if (this->isDescendantOf(&other)) 
        return true;
    
    return false;
}



        
Type* Context::GetType(const std::string& name) 
{
    if(types.find(name)!=types.end())   return types[name];
    if(parent==nullptr) return nullptr;
    return parent->GetType(name);
}

Type* Context::GetTypeOfAtribute(const std::string& name) 
{
    if(atributes.find(name)!=atributes.end())   return atributes[name];
    if(parent==nullptr) return nullptr;
    return parent->GetTypeOfAtribute(name);
}


// Type GetType_Method(std::string symbol)
// {
//     auto type= methods[symbol].returnType;
//     return types[type];

// }

Type* Context::CreateType(const std::string& name)
{
    Type* t= new Type(name);
    types.emplace(name,t);
    return  t;
}

bool Context::exist_Type(const std::string& name)
{
    if(types.find(name)!=types.end())   return true;
    if(parent==nullptr) return false;
    return parent->exist_Type(name);
}

bool Context::exist_Atribute(const std::string& name) //global
{
    if(atributes.find(name)!=atributes.end())   return true;
    if(parent==nullptr) return false;
    return parent->exist_Atribute(name);
}

bool Context::exist_local_Attribute(const std::string& name) 
{
    return atributes.find(name)!=atributes.end();

}


bool Context::Define_local_Attribute(const std::string& name,const std::string& typeName)  ////// 
{
    if(!exist_Type(typeName)) return false;/////// inneceisario?
    Type* type =GetType(typeName);
    atributes.emplace(name,type);
    return true;
}


bool Context::exist_Method(const std::string& name)
{
    if(methods.find(name)!=methods.end())   return true;
    if(parent==nullptr) return false;
    return parent->exist_Method(name);
    
}

Method* Context::GetMethod(const std::string& name)
{
    if(methods.find(name)!=methods.end())   return methods[name];
    if(parent==nullptr) return nullptr;
    return parent->GetMethod(name);
    
}


bool Context::DefineMethod(const std::string& name,const std::string& type,std::vector<Attribute> paramaters)
{
    if(methods.find(name)!=methods.end()) return false;

    methods.emplace(name,new Method(name,paramaters,type));
    return true;
}

Context* Context::createChildContext()
{
    child= new Context(this);
    return child;
}

Context* Context::RemoveContext()
{
    return parent;
}

void Context::loadInternalTypeAndMethod()
{
    Type* object= CreateType(INTERNAL_TYPES[0]);
    for(size_t i=1 ; i< INTERNAL_TYPES.size();i++)
    {
        auto t=CreateType(INTERNAL_TYPES[i]);
        t->parent=object;
    }

    DefineMethod("print","Object",{Attribute("value","Object")});

}

std::string Context::getLeastCommonAncestor(std::vector<std::string>exp_types)
{
    Type* current=GetType(exp_types[0]);


    for(size_t i=1 ;i<exp_types.size();i++)
    {
        current=LeastCommonAncestors(current,GetType(exp_types[i]));
    }
    return current->name;
    

}

Type* Context::LeastCommonAncestors(Type*first, Type* second ) {

    if(first==second) return first; 

    Type* a = first;
    Type* b = second;

    int depthA = 0, depthB = 0;
    for(const Type* curr = a; curr != nullptr; curr = curr->parent)
        depthA++;
    for(const Type* curr = b; curr != nullptr; curr = curr->parent)
        depthB++;

    // Igualamos las profundidades moviendo el nodo más profundo hacia arriba.
    while(depthA > depthB) {
        a = a->parent;
        depthA--;
    }
    while(depthB > depthA) {
        b = b->parent;
        depthB--;
    }

    while(a != b) {
        a = a->parent;
        b = b->parent;
    }

    // a y b son iguales en este punto (o nullptr) y serán el LCA.
    return a;
}

