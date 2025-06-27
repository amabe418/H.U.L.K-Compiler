#include"../include/Semantic/VariableDefindedVisitor.hpp"


void VariableDefindedVisitor::visit(ProgramNode* node) 
{
    for (auto stmt: node->stmts)
    {
        stmt->accept(*this);
    }
}

void VariableDefindedVisitor::visit(TypeNode* node) 
{
    if(context->exist_Type(node->name.lexeme))
    {
        std::string msg="The type "+ node->name.lexeme+" already exists.";
        errorHandler.reportError(node->name,msg);
        return;
    }
    
    auto lastType=currentType;

    context=context->createChildContext();

    Type* parent=context->GetType("Object");
    if(node->parentName.lexeme!="")
    {
        for(auto tp:context->INTERNAL_TYPES)
        {
            if(tp==node->parentName.lexeme)
            {
                std::string msg="The type " + node->name.lexeme + " cannot inherit from inner type like "+tp+".";
                errorHandler.reportError(node->parentName,msg);
                return;
            }
        }

        if(!context->exist_Type(node->parentName.lexeme))
        {
            std::string msg="The type "+ node->parentName.lexeme+" is not defined.";
            errorHandler.reportError(node->parentName,msg);
            return;
        }
        
        parent= context->GetType(node->parentName.lexeme);

    }
    
    currentType=context->CreateType(node->name.lexeme);
    node->type=currentType->name;
    currentType->parent=parent;
    bool hasArg=false;
    for(auto arg:node->args)
    {
        hasArg=true;
        auto id= static_cast<IdentifierNode*>(arg);
        if(currentType->exist_Argument(id->value.lexeme))
        {
            std::string msg="The argument "+ id->value.lexeme+" already exist";
            errorHandler.reportError(id->value,msg);
            return;
        }
        currentType->DefineArgument(id->value.lexeme,id->type);
        context->Define_local_Attribute(id->value.lexeme,id->getType());
    }

    if(node->parentName.lexeme!="")
    {  
        if(hasArg)
        {
            auto currentp=parent;
            bool found=false;
            while(true)
            {
                if( currentp->arguments.size()==0)
                {
                    if(currentp->parent->name=="Object")
                    {
                        if(node->parent_args.size()==0) break;
                        std::string msg="The Parent type should receive "+std::to_string(currentp->arguments.size())+" arguments.";
                        errorHandler.reportError(node->parentName,msg);
                        return;
                    }

                    currentp=currentp->parent;
                    continue;
                }

                if(node->parent_args.size()!=currentp->arguments.size())
                {
                    std::string msg="The Parent type should receive "+std::to_string(currentp->arguments.size())+" arguments.";
                    errorHandler.reportError(node->parentName,msg);
                    return;
                }

                for(auto atr:node->parent_args) 
                    atr->accept(*this);
                
                break;
            }
        }   
        else
        {
            auto currentp=parent;
            while(currentp->parent->name!="Object" && currentp->arguments.size()==0)
            {
                currentp=currentp->parent;
            }
            currentType->parentInstantiation=currentp->name;
        }
    }


    for(auto atribute : node->atributes)
    {
        atribute->accept(*this);
    };

    for(auto atribute : node->atributes)
    {
        auto atr=static_cast<AttributeNode*>(atribute);
        context->Define_local_Attribute(atr->id.lexeme,atr->getType());
    };


    for(auto meth : node->methods)
    {
        meth->accept(*this);
    };

    context =context->RemoveContext();
    auto temp=context->CreateType(node->name.lexeme);
    
    temp->parent=currentType->parent;
    temp->arguments=currentType->arguments;
    temp->atributes=currentType->atributes;
    temp->methods=currentType->methods;
    temp->parentInstantiation=currentType->parentInstantiation;
    currentType=lastType;

};

void VariableDefindedVisitor::visit(AttributeNode* node)
{
    if(  currentType!=nullptr &&!comingFromLet )
    {
        if(currentType->exist_Attribute(node->id.lexeme))
        {
            std::string msg="The attribute "+ node->id.lexeme+" is already defined in this type";
            errorHandler.reportError(node->id,msg);
            return;
        }
        

        node->expression->accept(*this);
        if(node->type!="")
        {
            if(!context->exist_Type(node->type))
            {
                std::string msg="The type "+ node->type+" is not defined.";
                errorHandler.reportError(node->id,msg);
                return;                        
            }
            if(node->type!=node->expression->type)
            {
                std::string msg="The attribute type '"+node->type+"' does not match the initialization type '"+node->expression->type +"'.";
                errorHandler.reportError(node->id,msg);
                return;
            }
        }
        else    node->type=node->expression->type;
        
        currentType->DefineAttribute(node->id.lexeme,node->expression->getType()); 
        return;
    }

    if(context->exist_local_Attribute(node->id.lexeme))
    {
        std::string msg="The attribute "+ node->id.lexeme+" is already defined in this context";
        errorHandler.reportError(node->id,msg);
        return;
    }
        
    node->expression->accept(*this);
    if(node->type!="")
    {
        if(!context->exist_Type(node->type))
        {
            std::string msg="The type "+ node->type+" is not defined.";
            errorHandler.reportError(node->id,msg);
            return;                        
        }
        if(node->type!=node->expression->type)
        {
            std::string msg="The attribute type '"+node->type+"' does not match the initialization type '"+node->expression->type +"'.";
            errorHandler.reportError(node->id,msg);
            return;
        }
    }
    else    node->type=node->expression->type;
    
    bool ok=context->Define_local_Attribute(node->id.lexeme,node->type);    
};


void VariableDefindedVisitor::visit(MethodNode* node)         
{   
    if(currentType!=nullptr )
    {
        if(currentType->exist_Method(node->id.lexeme))
        {
            std::string msg="The method "+ node->id.lexeme +" is already defined in this type";
            errorHandler.reportError(node->id,msg);
            return;
        }
    }
    context= context->createChildContext();           
    std::vector<Attribute> args;

    for(auto param:node->params)
    {
        if (IdentifierNode* p = dynamic_cast<IdentifierNode*>(param)) 
        {
            if(context->exist_local_Attribute(p->value.lexeme))
            {
                
                std::string msg="The Param name "+ node->id.lexeme+ "already exits.";
                errorHandler.reportError(node->id,msg);
                return;
                
            }
            if(p->type=="")
            {
                std::string msg="Unknown type of Parameter "+p->value.lexeme;
                errorHandler.reportError(p->value,msg);
                return;
            }
            else
            {
                if(!context->exist_Type(p->type))
                {
                    std::string msg="The type "+ p->type +" is not defined.";
                    errorHandler.reportError(p->value,msg);
                    return;                        
                }

            }
            context->Define_local_Attribute(p->value.lexeme,p->type); // todo: inferir tipo de los parametros
            args.push_back(Attribute(p->value.lexeme,p->type));

        }
        else
        {
            std::string msg="Unexpected error in builder method  "+ node->id.lexeme;
            errorHandler.reportError(node->id,msg);
            return;
        }
    }
    
    auto lastMethod=currentMethod;
    if(currentType!=nullptr)
    {
        currentType->DefineMethod(node->id.lexeme,node->type,args);
        currentMethod=currentType->GetMethod(node->id.lexeme);

    }

    
    node->body->accept(*this);
    if(node->type!="")
    {
        if(!context->exist_Type(node->type))
        {
            std::string msg="The type "+ node->type+" is not defined.";
            errorHandler.reportError(node->id,msg);
            return;                        
        }
        if(node->type!=node->body->type)
        {
            std::string msg="The return type '"+node->type+"' does not match the body type '"+node->body->type +"'.";
            errorHandler.reportError(node->id,msg);
            return;
        }
    }
    currentMethod=lastMethod;
    context= context->RemoveContext();
    node->type=node->body->type;

if(currentType!=nullptr)
{
    currentType->methods[node->id.lexeme]->m_returnType=node->type;
    return;
}
    auto metho=context->GetMethod(node->id.lexeme);
    metho->m_returnType=node->type;
    
};

void VariableDefindedVisitor::visit(LetExpression* node)
{
    context= context->createChildContext();
    comingFromLet=true;
    for (auto decl: node->assignments)
    {
        decl->accept(*this);
    }
    node->body->accept(*this);
    comingFromLet=false;
    node->type=node->body->type;

    context=context->RemoveContext();
    
};

void VariableDefindedVisitor::visit(BlockNode* node)  // el tipo sera el tipo de la ultima expresion
{
    std::string t="";
    for(auto exp :node->exprs)
    {
        exp->accept(*this);
        t=exp->type;
    }
    node->type=t;
};

void VariableDefindedVisitor::visit(BinaryExpression* node)  
{
    node->left->accept(*this);
    node->right->accept(*this);
    
    std::set<std::string> boolop={"<","<=",">",">=","==","!="};
    std::set<std::string> aritop={"+","-","/","*","^","%"};

    if(node->left->type!=node->right->type)
    {
        std::string msg="Invalid operation between type "+node->left->type +" and type "+node->right->type ;
        errorHandler.reportError(node->op,msg);
        return;
    }
    if(node->left->type=="Number")
    {
        if(aritop.find(node->op.lexeme)==aritop.end())
        {
            std::string msg="Invalid operation between type "+node->left->type +" and type "+node->right->type ;
            errorHandler.reportError(node->op,msg);
            return;
        }
        node->type="Number";
    }
    else if(node->left->type=="Boolean")
    {
        if(boolop.find(node->op.lexeme)==boolop.end())
        {
            std::string msg="Invalid operation between type "+node->left->type +" and type "+node->right->type ;
            errorHandler.reportError(node->op,msg);
            return;
        }
        node->type="Boolean";

    }
    else
    {
        std::string msg="Invalid operation between type "+node->left->type +" and type "+node->right->type ;
        errorHandler.reportError(node->op,msg);
        return;
    }

    
};

void VariableDefindedVisitor::visit(IdentifierNode* node)     
{
    if(node->value.lexeme=="self" )
    {
        if(!currentType)
                errorHandler.reportError(node->value, "'self' used outside of a class");
        else
            {
                node->type=currentType->name;
            }
        
        return;
    }

    if(!context->exist_Atribute(node->value.lexeme))
    {
        std::string msg="The name "+ node->value.lexeme+ " does not exist";
        errorHandler.reportError(node->value,msg);
        return;
    }

    auto type=context->GetTypeOfAtribute(node->value.lexeme);
    node->type=type->name;            

}; 

    
void VariableDefindedVisitor::visit(IfExpression* node)    //todo inferir el tipo    
{
    std::vector<std::string> exp_types;
    for(auto [cond,exp] :node->exprs_cond)
    {
        cond->accept(*this);
        if(cond->getType()!="Boolean")
        {
            std::string msg="The condition ,bust be bool.";
            errorHandler.reportError(node->id,msg);
            return;
        }
        exp->accept(*this);
        exp_types.push_back(exp->getType());
    }

    node->defaultExp->accept(*this);
    exp_types.push_back(node->defaultExp->getType());
    
    //  node->type=context->getLeastCommonAncestor(exp_types);
};

void VariableDefindedVisitor::visit(WhileExpression* node)   
{
    node->condition->accept(*this);
    // if(node->condition->getType()!="Boolean")
    // {
    //     std::string msg="the condition ,bust be bool  ";
    //     errorHandler.reportError(Token(),msg);
    //     return;
    // }
    node->body->accept(*this);

};


void VariableDefindedVisitor::visit(FunCallNode* node) 
{    
    Method* meth=nullptr;
    if(currentType!=nullptr)
    {
        if(node->id.lexeme=="base")
        {
            if(!currentMethod)
            {
                std::string msg="Invalidated use of base call";
                errorHandler.reportError(node->id,msg);
                return;
            }
            if(currentMethod->m_paramaters.size()!=node->arguments.size())
            {
                std::string msg="The function base "+ currentMethod->m_name + " must receive "+std::to_string(currentMethod->m_paramaters.size())+ "args";
                errorHandler.reportError(node->id,msg);
                return;
            }
            node->type=currentMethod->m_returnType;
            return;
        }
        meth=currentType->GetMethod(node->id.lexeme);
        if(!meth)
        {
            std::string msg="The function "+ node->id.lexeme+ " is not defined";
            errorHandler.reportError(node->id,msg);
            return;
        }
    }

    else
    {
        if( !(context->exist_Method(node->id.lexeme)))
    {
        std::string msg="The function "+ node->id.lexeme+ " is not defined";
        errorHandler.reportError(node->id,msg);
        return;
    }

    meth=context->GetMethod(node->id.lexeme);

    }

        if(meth->m_paramaters.size()!=node->arguments.size())
        {
        std::string msg="The function "+ node->id.lexeme + " must receive "+std::to_string(meth->m_paramaters.size())+ "args";
        errorHandler.reportError(node->id,msg);
        return;

        }
    for (size_t i = 0; i < node->arguments.size(); i++)
    {
        auto arg=node->arguments[i];
        arg->accept(*this);

        if(arg->type>meth->m_paramaters[i].type)
        {
            std::string msg="invalid type of argument in function "+ node->id.lexeme;
            errorHandler.reportError(node->id,msg);
            return;
        }
    }
    node->type=meth->m_returnType;

};

void VariableDefindedVisitor::visit(MemberCall* node) 
{
    // Primero resolvemos obj
    node->obj->accept(*this);

    // Obtener el tipo del objeto base
    Type* objType = context->GetType(node->obj->getType());

    if (!objType) 
    {
        std::string msg = "Unknown type for object in member call.";
        errorHandler.reportError(0, 0, msg);
        return;
    }

    if (auto id = dynamic_cast<IdentifierNode*>(node->member)) 
    {
        // Atributo
        if (currentType == nullptr || currentType->name!= objType->name) 
        {
            std::string msg = "Attributes of '" + objType->name + "' can only be accessed from within the class using 'self'";
            errorHandler.reportError(0, 0, msg);
            return;
        }

        if (!objType->exist_Attribute(id->value.lexeme)) {
            std::string msg = "The name '" + id->value.lexeme + "' is not an attribute of type " + objType->name;
            errorHandler.reportError(id->value, msg);
            return;
        }
        id->type=objType->atributes[id->value.lexeme]->type;
        node->type = id->type;
    }

    else if (auto meth = dynamic_cast<FunCallNode*>(node->member)) 
    {
        auto current=objType;
        bool found=false;
        while(current->name!="Object")
        {
            if (!current->exist_Method(meth->id.lexeme)) 
            {
                current=current->parent;
                continue;
            }
            found=true;
            break;
        }
        if(!found)
        {
            std::string msg = "The type " + objType->name + " has no method named " + meth->id.lexeme + "'";
            errorHandler.reportError(meth->id, msg);
            return;
        }


        auto lastType=currentType;
        currentType=current;
        meth->accept(*this);
        currentType=lastType;
        // Seteamos el tipo resultante
        node->type = meth->getType();

    } 
    else 
    {
        std::string msg = "Invalid member access.";
        errorHandler.reportError(0, 0, msg);
    }
}


void VariableDefindedVisitor::visit(DestructiveAssignNode* node) 
{
    
    node->lhs->accept(*this);
    node->rhs->accept(*this);

    if(node->lhs->type!=node->rhs->type)
    {
        std::string msg="The right side mus be of type "+ node->lhs->type;
        errorHandler.reportError(node->op_des,msg);
    }
    
}

void VariableDefindedVisitor::visit(TypeInstantiation* node)
{
    if(!context->exist_Type(node->typeName.lexeme))
    {
        std::string msg="The class "+ node->typeName.lexeme+"  is not defined";
        errorHandler.reportError(node->typeName,msg);
        return;
    }

    Type* typeInst= context->GetType(node->typeName.lexeme);
    

    std::string parentInst=typeInst->parentInstantiation;


    if(!parentInst.empty())
    {
        typeInst=context->GetType(parentInst); 
    }


    if(node->arguments.size()!=typeInst->arguments.size())
    {
        std::string msg="Invalid number of arguments , must receive "+ typeInst->arguments.size();
        errorHandler.reportError(node->typeName,msg);
        return;
    }
    
    for (size_t i = 0; i < node->arguments.size(); i++)
    {
        node->arguments[i]->accept(*this);
            
        if(node->arguments[i]->getType()!=typeInst->arguments[i].second->type)
        {
            std::string msg="The type arguments does not match ";
            errorHandler.reportError(node->typeName,msg);
            return;
        }
    }

}    

void VariableDefindedVisitor::visit(ForExression* node)       {};

void VariableDefindedVisitor::visit(UnaryExpression* node)      {};

void VariableDefindedVisitor::visit(LiteralNode* node) {

};


