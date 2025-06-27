#include "../include/Semantic/FunctionCollectorVisitor.hpp"
#include"../include/Semantic/VariableDefindedVisitor.hpp"
#include "../common/Error.hpp"


class SemanticAnalizer
{
public:
    ErrorHandler errorHandler;
    

    void check(AstNode* ast)
    {
        FunctionCollectorVisitor vis(errorHandler);
        ast->accept(vis);
        if(vis.errorHandler.hasErrors())
        {
            vis.errorHandler.printErrors();
            return;
        }

        // TypeBuilderVisitor  builder;
        // builder.context=vis.context;
        // ast->accept(builder);

        VariableDefindedVisitor  def(vis.context,errorHandler);
        ast->accept(def);

        if(def.errorHandler.hasErrors())
        {
            def.errorHandler.printErrors();
            errorHandler=def.errorHandler;
            return;
        }

    }


};