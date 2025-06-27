#include "../include/Parser/ParseTree.hpp"


class AstBuilderVisitor: public ParseTreeVisitor
{
    public:
        AstNode* visit(ParseNode* node)
       {

            if(node->m_value->value=="Program")
            {
                ProgramNode* prg= new ProgramNode();

                prg->stmts=stmsList(node->m_children[0]);
              
                return prg;
            }


            if(node->m_value->value=="Statement")
            {
                return node->m_children[1]->accept(*this);
            }

            if(node->m_value->value=="ifExp")
            {
                AstNode* defaultExp=node->m_children[0]->accept(*this);
                std::vector<std::pair<AstNode*,AstNode*>> exprs_cond=ElifList(node->m_children[2]);
                exprs_cond.insert(exprs_cond.begin(),{node->m_children[5]->accept(*this),node->m_children[3]->accept(*this)});
                
                return new IfExpression(node->m_children[7]->m_token,defaultExp,exprs_cond);
            }

            if(node->m_value->value=="While_loop")
            {

                AstNode* condition=node->m_children[2]->accept(*this);
                AstNode* body=node->m_children[0]->accept(*this);
                return new WhileExpression(condition,body);
            }
           
            if(node->m_value->value=="For_loop")
            {

                AstNode* item=new IdentifierNode(node->m_children[4]->m_token);
                AstNode* iterable=node->m_children[2]->accept(*this);
                AstNode* body=node->m_children[0]->accept(*this);
                return new ForExression(item,iterable,body);
            }


            if(node->m_value->value=="VarDeclaration")
            {
                std::vector<AstNode*> asgs= AssignmentList(node->m_children[2]);
                auto body=node->m_children[0]->accept(*this);
                return new LetExpression(asgs,body);    
            }
            
            if(node->m_value->value=="Expbody")
            {
                return node->m_children[0]->accept(*this);    
            }
            
            if(node->m_value->value=="Block")
            {
                std::vector<AstNode*> exp= ExpList(node->m_children[1]);
                return new BlockNode(exp);    
            }
         
            if(node->m_value->value=="AssignmentDecl")
            {
                auto body=node->m_children[0]->accept(*this);
                auto atrib=new AttributeNode(node->m_children[3]->m_token,body);
                if(!(node->m_children[2]->m_children.empty()))
                {
                    atrib->setType(node->m_children[2]->m_children[0]->m_token.lexeme);
                }
                return   atrib;

            }

            if(node->m_value->value=="Expression")
            {
                if(node->m_children.size()==1)
                    return node->m_children[0]->accept(*this);

                auto left=node->m_children[1]->accept(*this);
                if(node->m_children[0]->m_children.empty()) return left;

                Token op= node->m_children[0]->m_children[1]->m_token;
                auto rhl=node->m_children[0]->m_children[0]->accept(*this);
                return new DestructiveAssignNode(left,op,rhl );
            }


            if(node->m_value->value=="FuncDef")
            {
                Token id= node->m_children[1]->m_token;
                auto [params,body]=MethodSignaturePrime(node->m_children[0]);
                auto meth= new MethodNode(id,params,body);
                if(!node->m_children[0]->m_children[1]->m_children.empty())
            {
                meth->type=node->m_children[0]->m_children[1]->m_children[0]->m_token.lexeme;
            }
                if(!(node->m_children[0]->m_children[1]->m_children.empty()))
                {
                    meth->setType(node->m_children[0]->m_children[1]->m_children[0]->m_token.lexeme);
                }
                return meth;
            
            }
            
            if(node->m_value->value=="TypeDef")
            {
                Token name= node->m_children[5]->m_token;
                std::vector<AstNode*> args={};
                Token parentName;
                std::vector<AstNode*> parent_args;
                if(!(node->m_children[3]->m_children.empty()))
                {
                    parentName= node->m_children[3]->m_children[1]->m_token;
                    if(!node->m_children[3]->m_children[0]->m_children.empty())
                    {
                        parent_args=ArgList(node->m_children[3]->m_children[0]->m_children[1]);
                    }
                }

                if(!(node->m_children[4]->m_children.empty()))
                {
                    args=ParamList(node->m_children[4]->m_children[1]);
                }
                auto [atr,meth]= GetMemberList(node->m_children[1]);
                auto type= new TypeNode(name,atr,meth,args);
                type->parentName=parentName;
                type->parent_args=parent_args;
                return type;
                
            }

            if (node->m_value->value == "RelationalExpression")
            {
                AstNode* left = node->m_children[1]->accept(*this);
                return RelationalExpressionPrime(left, node->m_children[0]);
            }

            if (node->m_value->value == "BooleanExpression")
            {
                AstNode* left = node->m_children[1]->accept(*this);
                return BooleanExpressionPrime(left, node->m_children[0]);
            }

            if (node->m_value->value == "BooleanTerm")
            {
                AstNode* left = node->m_children[1]->accept(*this);
                return BooleanTermPrime(left, node->m_children[0]);
            }

            if (node->m_value->value == "BooleanFactor")
            {
                if(node->m_children.size()==2)
                {
                     return new UnaryExpression(node->m_children[1]->m_token, node->m_children[0]->accept(*this));
                }
                
                else
                {
                    return node->m_children[0]->accept(*this);
                }
            }

            if (node->m_value->value == "TypeInstantiation")
            {
                Token type=node->m_children[1]->m_token;
                std::vector<AstNode*> arguments=ArgList(node->m_children[0]->m_children[1]);
                return new TypeInstantiation(type,arguments);
            }


            if(node->m_value->value=="ExpAditiva")
            {
                AstNode* left=node->m_children[1]->accept(*this);
                return ExpAditivaPrime(left,node->m_children[0]);
            }
            
            if(node->m_value->value=="ExpMultiplicativa")
            {
                AstNode* left=node->m_children[1]->accept(*this);
                return ExpMultiplicativaPrime(left,node->m_children[0]);
            }
            
            if(node->m_value->value=="ExpExpon")
            {
                AstNode* left=node->m_children[1]->accept(*this);
                return ExpExponPrime(left,node->m_children[0]);
            }
            
            if(node->m_value->value=="Primary")
            {
                if(node->m_children.size()==3 && node->m_children[2]->m_value->value=="punc_LeftParen")
                {
                    return node->m_children[1]->accept(*this);
                }
                if(node->m_children.size()==3)
                {
                    ParseNode* identifierNode = node->m_children[2]; // "Identifier"
                    ParseNode* funCallPrime = node->m_children[1];   // "FunCallPrime"
                    ParseNode* memberAccessPrime = node->m_children[0]; // "MemberAccessPrime"
                    
                    // 1. Creamos el nodo base: puede ser identifier o llamada a función
                    AstNode* base;
                    if (!funCallPrime->m_children.empty() &&
                        funCallPrime->m_children[0]->m_children[2]->m_value->value == "punc_LeftParen") {
                        // Es una llamada a función: x(...)
                        std::vector<AstNode*> args = ArgList(funCallPrime->m_children[0]->m_children[1]);
                        base = new FunCallNode(identifierNode->m_token, args);
                    } else {
                        // Solo es un identificador
                        base = new IdentifierNode(identifierNode->m_token);

                    }

                    // 2. Ahora procesamos encadenamiento de miembros: .x.y().z
                    return MemberAccessChain(base, memberAccessPrime);

                }
                
                if(node->m_children[0]->m_value->value=="Number")
                {
                   auto l= new LiteralNode(node->m_children[0]->m_token);
                   l->type="Number";
                    return l;
                }
                if(node->m_children[0]->m_value->value=="String")
                {
                   auto l= new LiteralNode(node->m_children[0]->m_token);
                   l->type="String";
                    return l;
                }
                if(node->m_children[0]->m_value->value=="kw_false_" ||node->m_children[0]->m_value->value=="kw_true_")
                {
                   auto l= new LiteralNode(node->m_children[0]->m_token);
                   l->type="Boolean";
                    return l;
                }
            }

            std::cout<<"Unknow node "<<node->m_value->value<<std::endl;            
            return nullptr;
       }

       std::pair<std::vector<AstNode*> ,std::vector<AstNode*>> GetMemberList(ParseNode*node)
       {
            std::vector<AstNode*> atributes;
            std::vector<AstNode*> methods;
            if(node->m_children.empty() )    return {atributes,methods};
            
            auto [isAtr,member]=GetMember(node->m_children[1]);
            if(isAtr)   atributes.push_back(member);
            else methods.push_back(member);

            auto [atr,meth]=GetMemberList(node->m_children[0]);
             for(auto child:atr)
            {
                atributes.push_back(child);
            }
            for(auto child:meth)
            {
                methods.push_back(child);
            }
            return {atributes,methods};
            
       }

       std::pair<bool,AstNode*> GetMember(ParseNode* node)
       {
            Token id=node->m_children[3]->m_token;
            if(node->m_children[1]->m_children.size()==2)
            {
                AstNode* exp=node->m_children[1]->m_children[0]->accept(*this);
                auto atr=new AttributeNode(id,exp);
                if(!(node->m_children[2]->m_children.empty()))
                {
                    atr->setType(node->m_children[2]->m_children[0]->m_token.lexeme);
                }
                return {true,atr};   
            }

            auto signature=node->m_children[1]->m_children[0];
            auto [params,body]=MethodSignaturePrime(signature);
            auto met=new MethodNode(id,params,body);
            if(!signature->m_children[1]->m_children.empty())
            {
                met->type=signature->m_children[1]->m_children[0]->m_token.lexeme;
            }
            return {false,met};

       }

       std::pair<std::vector<AstNode*> ,AstNode*> MethodSignaturePrime(ParseNode* node)
       {
            AstNode* body=nullptr;
            std::vector<AstNode*> params= ParamList(node->m_children[3]);
            if(!(node->m_children[0]->m_children.empty()))
                body= node->m_children[0]->m_children[0]->accept(*this);
            
            return {params,body};

       }

       std::vector<std::pair<AstNode*,AstNode*>> ElifList(ParseNode* node)
       {
            std::vector<std::pair<AstNode*,AstNode*>> list;
            if(node->m_children.empty())    return list;
            list.push_back({node->m_children[3]->accept(*this),node->m_children[1]->accept(*this)});
            auto asglist= ElifList(node->m_children[0]);
            for(auto child:asglist)
            {
                list.push_back(child);
            }
            return list;

       }

       std::vector<AstNode*> ParamList(ParseNode* node)
       {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;
            auto id =new IdentifierNode(node->m_children[2]->m_token);
            if(!(node->m_children[1]->m_children.empty()))
            {
                id->setType(node->m_children[1]->m_children[0]->m_token.lexeme);
            }
            list.push_back(id);
            auto parmlist2=ParamListTail(node->m_children[0]);
             for(auto child:parmlist2)
            {
                list.push_back(child);
            }
            return list;

       }

        std::vector<AstNode*> ParamListTail(ParseNode* node)
        {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;
            auto id =new IdentifierNode(node->m_children[2]->m_token);
            if(!(node->m_children[1]->m_children.empty()))
            {
                id->setType(node->m_children[1]->m_children[0]->m_token.lexeme);
            }
            list.push_back(id);
            auto asglist= ParamListTail(node->m_children[0]);
            for(auto child:asglist)
            {
                list.push_back(child);
            }
            return list;
        }

       std::vector<AstNode*> ExpList(ParseNode* node)
       {
            std::vector<AstNode*> list;
            list.push_back(node->m_children[2]->accept(*this));
            auto asglist2=ExpListTail(node->m_children[0]);
             for(auto child:asglist2)
            {
                list.push_back(child);
            }
            return list;

       }

        std::vector<AstNode*> ExpListTail(ParseNode* node)
        {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;
            list.push_back(node->m_children[2]->accept(*this));
            auto asglist= ExpListTail(node->m_children[0]);
            for(auto child:asglist)
            {
                list.push_back(child);
            }
            return list;
        }

        std::vector<AstNode*> AssignmentList(ParseNode* node)
        {
            std::vector<AstNode*> list;
            list.push_back(node->m_children[1]->accept(*this));
            auto asglist2=AssignmentListTail(node->m_children[0]);
             for(auto child:asglist2)
            {
                list.push_back(child);
            }
            return list;

        }

        std::vector<AstNode*> AssignmentListTail(ParseNode* node)
        {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;

            list.push_back(node->m_children[1]->accept(*this));
            
            auto asglist= AssignmentListTail(node->m_children[0]);
            for(auto child:asglist)
            {
                list.push_back(child);
            }
            return list;
        }
       
        std::vector<AstNode*> stmsList(ParseNode* node)
       {
            std::vector<AstNode*> list;
            if(node->m_children.empty())
                return list;
            
            list.push_back(node->m_children[1]->accept(*this));
            
            auto stmsl=stmsList(node->m_children[0]);
            for(auto child:stmsl)
            {
                list.push_back(child);
            }
            return list;
       }

        AstNode* ExpAditivaPrime(AstNode* inherited,ParseNode* node)
        {
            if(node->m_children.empty())    return inherited;

            AstNode* right=node->m_children[1]->accept(*this);
            Token op=node->m_children[2]->m_token;
            AstNode* temp = new BinaryExpression(inherited,op,right);
            return ExpAditivaPrime(temp,node->m_children[0]);
        }
       
        AstNode* ExpMultiplicativaPrime(AstNode* inherited,ParseNode* node)
        {
            if(node->m_children.empty())    return inherited;

            AstNode* right=node->m_children[1]->accept(*this);
            Token op=node->m_children[2]->m_token;
            AstNode* temp = new BinaryExpression(inherited,op,right);
            return ExpMultiplicativaPrime(temp,node->m_children[0]);
        }
        
        AstNode* ExpExponPrime(AstNode* inherited,ParseNode* node)
        {
            if(node->m_children.empty())    return inherited;

            AstNode* right=node->m_children[1]->accept(*this);
            Token op=node->m_children[2]->m_token;
            AstNode* temp = new BinaryExpression(inherited,op,right);
            return ExpExponPrime(temp,node->m_children[0]);
        }
        // (exp) := (exp)
        AstNode* DestructExpPrime(AstNode* inherited,ParseNode* node)
        {
             if(node->m_children.empty())    return inherited;

            AstNode* right=node->m_children[0]->accept(*this);
            Token op=node->m_children[1]->m_token;
            AstNode* temp = new BinaryExpression(inherited,op,right);
            return ExpExponPrime(temp,node->m_children[0]);

        }
        // Procesa la extensión de una RelationalExpression.
        AstNode* RelationalExpressionPrime(AstNode* left, ParseNode* node) {
           if(node->m_children.empty())    return left;

            AstNode* right=node->m_children[1]->accept(*this);
            Token op=node->m_children[2]->m_token;
            AstNode* temp = new BinaryExpression(left,op,right);
            return RelationalExpressionPrime(temp,node->m_children[0]);
        }

        // Para las producciones de BooleanExpression (para el operador 'or'):
        AstNode* BooleanExpressionPrime(AstNode* inherited, ParseNode* node) {
            if(node->m_children.empty())    return inherited;

            AstNode* right=node->m_children[1]->accept(*this);
            Token op=node->m_children[2]->m_token;
            AstNode* temp = new BinaryExpression(inherited,op,right);
            return BooleanExpressionPrime(temp,node->m_children[0]);
        }

        // Para las producciones de BooleanTerm (para el operador 'and'):
        AstNode* BooleanTermPrime(AstNode* left, ParseNode* node) {
            if(node->m_children.empty())    return left;

            AstNode* right=node->m_children[1]->accept(*this);
            Token op=node->m_children[2]->m_token;
            AstNode* temp = new BinaryExpression(left,op,right);
            return BooleanTermPrime(temp,node->m_children[0]);
        }
        

        std::vector<AstNode*> ArgList(ParseNode* node)
       {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;
            auto arg=node->m_children[1]->accept(*this);
            list.push_back(arg);

            auto parmlist2=ArgListTail(node->m_children[0]);
             for(auto child:parmlist2)
            {
                list.push_back(child);
            }
            return list;

       }

        std::vector<AstNode*> ArgListTail(ParseNode* node)
        {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;
            auto arg=node->m_children[1]->accept(*this);
            list.push_back(arg);

            auto asglist= ArgListTail(node->m_children[0]);
            for(auto child:asglist)
            {
                list.push_back(child);
            }
            return list;
        }

        AstNode* MemberAccessChain(AstNode* base, ParseNode* memberAccessPrime) 
        {
            if (memberAccessPrime->m_children.empty()) return base;
            // Esperado: MemberAccessPrime -> dot Identifier FunCallPrime MemberAccessPrime
           
            ParseNode* memberId = memberAccessPrime->m_children[2]; // Identifier
            ParseNode* funCallPrime = memberAccessPrime->m_children[1]; // FunCallPrime
            ParseNode* nextMemberAccess = memberAccessPrime->m_children[0]; // MemberAccessPrime

            AstNode* member;
            if (!funCallPrime->m_children.empty() &&
                funCallPrime->m_children[0]->m_children[2]->m_value->value == "punc_LeftParen") {
                // Es una función miembro: .x()
                std::vector<AstNode*> args = ArgList(funCallPrime->m_children[0]->m_children[1]);
                member = new FunCallNode(memberId->m_token, args);
            } else {
                // Es una propiedad: .x
                member = new IdentifierNode(memberId->m_token);
            }

            
            AstNode* access = new MemberCall(base, member);
            return MemberAccessChain(access, nextMemberAccess); // recursión
}

    };
