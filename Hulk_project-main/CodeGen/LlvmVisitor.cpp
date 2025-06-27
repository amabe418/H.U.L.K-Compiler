#include "../include/CodeGen/LlvmVisitor.hpp"
#include "llvm/IR/IRBuilder.h"


void LlvmVisitor::visit(ProgramNode* node)          
 {
    for(auto stmt:node->stmts)
    {
        stmt->accept(*this);
    }


 };

void LlvmVisitor::visit(TypeNode* node) {
    
    std::vector<llvm::Type*> attrTypes;
   
    if(node->parentName.lexeme!="")
    {
        attrTypes.push_back(cgContext.namedStruct[node->parentName.lexeme]);
    }

    for (AstNode* attr : node->atributes) // tipo de los atributos
    {
        std::string attrTypeName = attr->getType();

        
        if ( cgContext.getInteralType(attrTypeName)) {
            llvm::Type* type = cgContext.getInteralType(attrTypeName);
            attrTypes.push_back(type);
           
        }
        else
        {
            llvm::Type* type = cgContext.namedStruct[attrTypeName];
            attrTypes.push_back(type->getPointerTo());
        
        }
    }
 
    if (attrTypes.empty()) 
    {
        attrTypes.push_back(llvm::Type::getInt8Ty(cgContext.llvmContext));
    }
  
    llvm::StructType* structType = llvm::StructType::create(cgContext.llvmContext, attrTypes, node->name.lexeme);
    cgContext.namedStruct[node->name.lexeme] = structType;
    cgContext.namedTypeNode[node->name.lexeme]=node;
    
    auto lasttype=cgContext.currentType;
    cgContext.currentType=node->name.lexeme;

    std::vector<llvm::Type*> ParamTypes;

    for (AstNode* arg : node->args)  // tipo de los argumetnsp para el const
    {
        std::string argTypeName = arg->getType();
         if ( cgContext.getInteralType(argTypeName)) {
            llvm::Type* type = cgContext.getInteralType(argTypeName);
            ParamTypes.push_back(type);
           
        }
        else
        {
            auto st=cgContext.namedStruct[argTypeName];

            ParamTypes.push_back(st->getPointerTo());
        }
    }

    TypeNode* parent=nullptr;

    if(node->parentName.lexeme!="")
    {
        parent=cgContext.namedTypeNode[node->parentName.lexeme];
    }

    std::string constructorName = "new_" + node->name.lexeme;
    bool useParentConst=false;
    if(ParamTypes.empty() && parent)// usar parametros del padre
    {
        useParentConst=true;
        llvm::Function* parentConst= cgContext.llvmModule.getFunction("new_"+parent->name.lexeme);
        auto parentArgs=parentConst->args();
        for(auto &arg:parentArgs)
        {
            ParamTypes.push_back(arg.getType());
        }

    }

    llvm::FunctionType* constructorType = llvm::FunctionType::get(
        llvm::PointerType::get(structType, 0),
        ParamTypes,
        false
    );
    llvm::Function* constructorFunc = llvm::Function::Create(
        constructorType,
        llvm::Function::ExternalLinkage,
        constructorName,
        cgContext.llvmModule
    );

    // 6. Construir el cuerpo del constructor.
    auto prevInsertPoint = cgContext.irBuilder.GetInsertBlock();
    llvm::BasicBlock* constructorBB = llvm::BasicBlock::Create(cgContext.llvmContext, "entry", constructorFunc);
    cgContext.irBuilder.SetInsertPoint(constructorBB);
    
    llvm::Value* structSize = llvm::ConstantExpr::getSizeOf(structType);
    llvm::Function* mallocFunc = cgContext.llvmModule.getFunction("malloc");

    if (!mallocFunc) {
        llvm::FunctionType* mallocType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(cgContext.llvmContext), 0),
            { llvm::Type::getInt64Ty(cgContext.llvmContext) },
            false
        );
        mallocFunc = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", cgContext.llvmModule);
    }
    
    llvm::Value* rawPtr = cgContext.irBuilder.CreateCall(mallocFunc, { structSize });  
    llvm::Value* typedPtr = cgContext.irBuilder.CreateBitCast(rawPtr, llvm::PointerType::get(structType, 0)); 

  //  llvm::AllocaInst* selfAlloca = cgContext.createEntryBlockAlloca(constructorFunc, typedPtr->getType(), "self");
  //  constructorBuilder.CreateStore(typedPtr, selfAlloca);
  //  cgContext.addLocalVariable("self", selfAlloca);
 

     //agrego los argumento de la clase como varibales locales del construc para usarlos en la inicializacion de los atributos
    cgContext.pushLocalScope();

    unsigned argIndex = 0;
    if(!node->args.empty())
    {
        for (auto& arg : constructorFunc->args()) {
        arg.setName(dynamic_cast<IdentifierNode*>(node->args[argIndex])->value.lexeme);

        llvm::AllocaInst* alloca = cgContext.createEntryBlockAlloca(constructorFunc, arg.getType(), arg.getName().str());
        cgContext.irBuilder.CreateStore(&arg, alloca);
        cgContext.addLocalVariable(arg.getName().str(), alloca);
        ++argIndex;
        }
    }
   
    if(useParentConst)
    {
        std::vector<llvm::Value*> parentArgValues;
        auto structParent=cgContext.namedStruct[node->parentName.lexeme];
        auto parentConst=cgContext.llvmModule.getFunction("new_"+node->parentName.lexeme);
        for (auto argsIter = constructorFunc->arg_begin(); argsIter < constructorFunc->arg_end(); argsIter++)
        {
            parentArgValues.push_back(argsIter);
        }
        
        auto parentValue=cgContext.irBuilder.CreateCall(parentConst,parentArgValues,node->parentName.lexeme);
        llvm::Value* parentPtr= cgContext.irBuilder.CreateStructGEP(structType,typedPtr,0,"base");

        llvm::Value* dest=cgContext.irBuilder.CreateBitCast(parentPtr,llvm::PointerType::get(structParent, 0));
        llvm::Value* src=cgContext.irBuilder.CreateBitCast(parentValue,llvm::PointerType::get(structParent, 0));

        llvm::StructType* parentStruct = cgContext.namedStruct[parent->name.lexeme];
        llvm::Value* size = llvm::ConstantExpr::getSizeOf(parentStruct);
        cgContext.irBuilder.CreateMemCpy(dest, llvm::MaybeAlign(8), src, llvm::MaybeAlign(8), size);
    }

    if(parent && !useParentConst)
    {
        std::vector<llvm::Value*> parentArgValues;
        auto structParent=cgContext.namedStruct[node->parentName.lexeme];
        auto parentConst=cgContext.llvmModule.getFunction("new_"+node->parentName.lexeme);
        for(auto arg:node->parent_args)
        {
            arg->accept(*this);
            parentArgValues.push_back(lastValue);
        }
        lastValue=nullptr;
        auto parentValue=cgContext.irBuilder.CreateCall(parentConst,parentArgValues,node->parentName.lexeme);
        llvm::Value* parentPtr= cgContext.irBuilder.CreateStructGEP(structType,typedPtr,0,"base");

        llvm::Value* dest=cgContext.irBuilder.CreateBitCast(parentPtr,llvm::PointerType::get(structParent, 0));
        llvm::Value* src=cgContext.irBuilder.CreateBitCast(parentValue,llvm::PointerType::get(structParent, 0));

        llvm::StructType* parentStruct = cgContext.namedStruct[parent->name.lexeme];
        llvm::Value* size = llvm::ConstantExpr::getSizeOf(parentStruct);
        cgContext.irBuilder.CreateMemCpy(dest, llvm::MaybeAlign(8), src, llvm::MaybeAlign(8), size);
    }


    unsigned memberIndex = 0;
    if(parent) memberIndex=1;
    lastValue = nullptr;
    
    for (AstNode* attr : node->atributes) 
    {
        auto atr=static_cast<AttributeNode*>(attr);
        
        attr->accept(*this);  
        
        llvm::Value* initValue = lastValue;
        if (!initValue) {
            std::cerr << "Error: atributo sin valor de inicialización.\n";
            initValue = llvm::ConstantFP::get(cgContext.llvmContext, llvm::APFloat(0.0));
        }

        llvm::Value* memberPtr =cgContext.irBuilder.CreateStructGEP(structType,typedPtr,memberIndex,atr->id.lexeme);
        cgContext.irBuilder.CreateStore(initValue,memberPtr);

       
        ++memberIndex;
    }
    cgContext.irBuilder.CreateRet(typedPtr);
    
    cgContext.irBuilder.SetInsertPoint(prevInsertPoint);

   
    
    for (AstNode* method : node->methods) {
        if (auto methodNode = dynamic_cast<MethodNode*>(method)) {
            std::string methName = methodNode->id.lexeme;
           
            auto selfP=new IdentifierNode(Token("self",TokenType::Identifier,0,0));
            methodNode->params.insert(methodNode->params.begin(),selfP);
            

            methodNode->accept(*this);

        }
    }

    cgContext.popLocalScope(); 
    cgContext.namedStruct[node->name.lexeme]=structType;
    cgContext.currentType=lasttype;
    lastValue = nullptr;
}


void LlvmVisitor::visit(BlockNode* node) 
{
    llvm::Value* value=nullptr;

    for(auto exp:node->exprs)
    {
        exp->accept(*this);
        
        value=lastValue;
    }

    lastValue=value;

    
};

void LlvmVisitor::visit(BinaryExpression* node)     
    {
        node->left->accept(*this);
        llvm::Value* leftvalue=lastValue;
        node->right->accept(*this);
        llvm::Value* rightvalue=lastValue;

        if(node->op.lexeme=="+")
            lastValue=cgContext.irBuilder.CreateFAdd(leftvalue,rightvalue,"addtmp");
        else if(node->op.lexeme=="-")
            lastValue=cgContext.irBuilder.CreateFSub(leftvalue,rightvalue,"subtmp");
        else if(node->op.lexeme=="*")
            lastValue=cgContext.irBuilder.CreateFMul(leftvalue,rightvalue,"multmp");
        else if(node->op.lexeme=="/")
            lastValue=cgContext.irBuilder.CreateFDiv(leftvalue,rightvalue,"divtmp");
        else if(node->op.lexeme=="%")
            lastValue=cgContext.irBuilder.CreateFRem(leftvalue,rightvalue,"modtmp");
        else if(node->op.lexeme=="^")
        {
             llvm::Function *PowFunc = llvm::Intrinsic::getDeclaration(&(cgContext.llvmModule), llvm::Intrinsic::pow,
                                                                { llvm::Type::getDoubleTy(cgContext.llvmContext) });

            lastValue=cgContext.irBuilder.CreateCall(PowFunc, {leftvalue, rightvalue}, "powtmp");
        }
        else if(node->op.lexeme==">")
            lastValue = cgContext.irBuilder.CreateFCmpUGT(leftvalue, rightvalue, "gttmp");
        else if(node->op.lexeme==">=")
            lastValue = cgContext.irBuilder.CreateFCmpUGE(leftvalue, rightvalue, "gttmp");
        else if(node->op.lexeme=="<")
            lastValue = cgContext.irBuilder.CreateFCmpULT(leftvalue, rightvalue, "gttmp");
        else if(node->op.lexeme=="<=")
            lastValue = cgContext.irBuilder.CreateFCmpULE(leftvalue, rightvalue, "gttmp");
       
        else if(node->op.lexeme=="=="||node->op.lexeme=="!=" )
        {
            
            if(node->left->getType()=="Number")
            {
                if(node->op.lexeme=="==")
                    lastValue=cgContext.irBuilder.CreateFCmpUEQ(leftvalue, rightvalue, "eqtmp");
                else
                    lastValue=cgContext.irBuilder.CreateFCmpUNE(leftvalue, rightvalue, "netmp");
            }
             if(node->left->getType()=="Boolean")
            {
                if(node->op.lexeme=="==")
                    lastValue=cgContext.irBuilder.CreateICmpEQ(leftvalue, rightvalue, "beqtmp");
                else
                    lastValue=cgContext.irBuilder.CreateICmpNE(leftvalue, rightvalue, "bnetmp");
            }
        }
        else if(node->op.lexeme=="&&" )
            lastValue=cgContext.irBuilder.CreateAnd(leftvalue,rightvalue,"andtmp");
        else if(node->op.lexeme=="||" )
            lastValue=cgContext.irBuilder.CreateOr(leftvalue,rightvalue,"ortmp");



        else
        {
            llvm::errs() << "Operador binario no soportado: " << node->op.lexeme << "\n";
                lastValue = nullptr;   
        }

        

    };

void LlvmVisitor::visit(IdentifierNode* node) 
{
    
    llvm::Value* varAlloca=cgContext.getLocalVariable(node->value.lexeme);
    if(!varAlloca)
    {
        return ; ////// add error here
    }

    llvm::Type* varType = llvm::cast<llvm::AllocaInst>(varAlloca)->getAllocatedType();
    
    lastValue = cgContext.irBuilder.CreateLoad(varType, varAlloca, node->value.lexeme);
    return;
    
          
};


void LlvmVisitor::visit(MethodNode* node) 
{
    std::string methodName=node->id.lexeme;
    if(!cgContext.currentType.empty())
    {
         methodName= methodName+ "_" + cgContext.currentType;
    }
    llvm::BasicBlock* prevBlock = cgContext.irBuilder.GetInsertBlock();
    
    std::vector<llvm::Type*> paramTypes;
    std::vector<std::string> paramNames;
    for (auto* paramNode : node->params) 
    {
        auto id=static_cast<IdentifierNode*>(paramNode);
        std::string typeStr = paramNode->getType();
        llvm::Type* paramType = nullptr;

        if(id->value.lexeme=="self" && !cgContext.currentType.empty())
        {
            auto selfType=cgContext.namedStruct[cgContext.currentType];
            paramType=llvm::PointerType::get(selfType,0);
        }
        
        else if(cgContext.getInteralType(typeStr))
            paramType=cgContext.getInteralType(typeStr);
        else
        {
            auto structtype= cgContext.namedStruct[typeStr];
            paramType=structtype->getPointerTo();
        }

        paramTypes.push_back(paramType);
        paramNames.push_back(id->value.lexeme);

       
    

    }

    llvm::Type* returnType=nullptr;

    std::string returnTypeStr=node->type;
    if(  cgContext.getInteralType(returnTypeStr)!=nullptr)
        {
            returnType=cgContext.getInteralType(returnTypeStr);
        }
    else{
        auto structtype= cgContext.namedStruct[returnTypeStr];
        returnType=structtype->getPointerTo();
    }
       
    if(returnTypeStr=="Object") 
    {
        returnType=llvm::Type::getDoubleTy(cgContext.llvmContext);
    }
    


    llvm::FunctionType* functionType = llvm::FunctionType::get(
        returnType,
        paramTypes,
        false
    );

    llvm::Function* function = llvm::Function::Create(
        functionType,
        llvm::Function::ExternalLinkage,
        methodName,
        cgContext.llvmModule
    );


    // Crear bloque de entrada
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(cgContext.llvmContext, "entry", function);
    cgContext.irBuilder.SetInsertPoint(entry);

    auto lastF=currentFunction;
    currentFunction=function;
    
    currentMethodName=node->id.lexeme;
    cgContext.pushLocalScope();

    // Asignar parámetros a variables locales
    size_t idx = 0;
    for (auto& arg : function->args()) {
       
        std::string paramName = paramNames[idx];
        auto paramType = paramTypes[idx];
        arg.setName(paramName);
    
        llvm::AllocaInst* alloca = cgContext.createEntryBlockAlloca(function, paramType, paramName);
        cgContext.irBuilder.CreateStore(&arg, alloca);
        cgContext.addLocalVariable(paramName, alloca);
        idx++;
    }

    // Generar cuerpo
    node->body->accept(*this);



    // Manejo de retorno
    if (lastValue) {
        cgContext.irBuilder.CreateRet(lastValue);
    } else {
        cgContext.irBuilder.CreateRet(llvm::ConstantFP::get(cgContext.llvmContext, llvm::APFloat(0.0))); // default
    }

    currentFunction=lastF;
    cgContext.popLocalScope(); // restaurar scope anterior

    cgContext.irBuilder.SetInsertPoint(prevBlock);

}


void LlvmVisitor::visit(IfExpression* node) {
    llvm::Function* function = cgContext.irBuilder.GetInsertBlock()->getParent();
    llvm::BasicBlock* afterBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "ifcont", function);

    std::vector<std::pair<llvm::BasicBlock*, llvm::BasicBlock*>> branches;
    for (size_t i = 0; i < node->exprs_cond.size(); ++i) {
        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "ifcond", function);
        llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "ifbody", function);
        branches.emplace_back(condBlock, bodyBlock);
    }

    llvm::BasicBlock* elseBlock = node->defaultExp
        ? llvm::BasicBlock::Create(cgContext.llvmContext, "elsebody", function)
        : afterBlock;

    // Salto inicial
    if (!branches.empty()) {
        cgContext.irBuilder.CreateBr(branches[0].first);
    } else {
        cgContext.irBuilder.CreateBr(elseBlock);
    }

    std::vector<std::pair<llvm::BasicBlock*, llvm::Value*>> valueBlocks;

    for (size_t i = 0; i < node->exprs_cond.size(); ++i) {
        // Condición
        cgContext.irBuilder.SetInsertPoint(branches[i].first);
        node->exprs_cond[i].first->accept(*this);
        llvm::Value* condValue = lastValue;

        condValue = cgContext.irBuilder.CreateICmpNE(
            condValue,
            llvm::ConstantInt::get(condValue->getType(), 0),
            "ifcondbool"
        );

        llvm::BasicBlock* nextCond = (i + 1 < branches.size()) ? branches[i + 1].first : elseBlock;
        cgContext.irBuilder.CreateCondBr(condValue, branches[i].second, nextCond);

        // Cuerpo
        cgContext.irBuilder.SetInsertPoint(branches[i].second);
        node->exprs_cond[i].second->accept(*this);
        if (lastValue)
            valueBlocks.emplace_back(cgContext.irBuilder.GetInsertBlock(), lastValue);
        cgContext.irBuilder.CreateBr(afterBlock);
    }

    // ELSE
    if (node->defaultExp) {
        cgContext.irBuilder.SetInsertPoint(elseBlock);
        node->defaultExp->accept(*this);
        if (lastValue)
            valueBlocks.emplace_back(cgContext.irBuilder.GetInsertBlock(), lastValue);
        cgContext.irBuilder.CreateBr(afterBlock);
    }

    // AFTER
    cgContext.irBuilder.SetInsertPoint(afterBlock);

    // PHI si hay valores de retorno
    if (!valueBlocks.empty()) {
        llvm::Type* phiType = valueBlocks[0].second->getType();

        // Verificamos si todos tienen el mismo tipo
        bool allSameType = std::all_of(
            valueBlocks.begin(), valueBlocks.end(),
            [&](auto& vb) { return vb.second->getType() == phiType; }
        );

        // Si no, usamos double por defecto
        if (!allSameType) {
            phiType = llvm::Type::getDoubleTy(cgContext.llvmContext);
        }

        llvm::PHINode* phi = cgContext.irBuilder.CreatePHI(phiType, valueBlocks.size(), "iftmp");

        for (auto& [block, val] : valueBlocks) {
            if (val->getType() != phiType) {
                if (val->getType()->isIntegerTy() && phiType->isDoubleTy()) {
                    val = cgContext.irBuilder.CreateSIToFP(val, phiType, "int_to_double");
                } else if (val->getType()->isDoubleTy() && phiType->isIntegerTy()) {
                    val = cgContext.irBuilder.CreateFPToSI(val, phiType, "double_to_int");
                }
            }
            phi->addIncoming(val, block);
        }
        lastValue = phi;
    } else {
        lastValue = nullptr;
    }

    
    
}

void LlvmVisitor::visit(WhileExpression* node) 
{

    llvm::Function* function = cgContext.irBuilder.GetInsertBlock()->getParent();

    // Crear bloques para el ciclo while
    llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "while.cond", function);
    llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(cgContext.llvmContext, "while.body");
    llvm::BasicBlock* endBlock  = llvm::BasicBlock::Create(cgContext.llvmContext, "while.end");

    // Brincar al bloque de condición
    cgContext.irBuilder.CreateBr(condBlock);
    cgContext.irBuilder.SetInsertPoint(condBlock);

    // Generar el código de la condición
    node->condition->accept(*this); // Asume que el resultado está en lastValue
    llvm::Value* condValue = lastValue;

    // Convertir condición a booleano (i1)
    if (condValue->getType()->isIntegerTy(1) == false) {
        condValue = cgContext.irBuilder.CreateICmpNE(
            condValue,
            llvm::ConstantInt::get(condValue->getType(), 0),
            "while.cond.bool"
        );
    }

    // Branch dependiendo del resultado de la condición
    cgContext.irBuilder.CreateCondBr(condValue, bodyBlock, endBlock);

   // Insertar y entrar al cuerpo del while

    bodyBlock->insertInto(function); // Agrega manualmente
    cgContext.irBuilder.SetInsertPoint(bodyBlock);
    node->body->accept(*this);
    cgContext.irBuilder.CreateBr(condBlock); // Loop back

    // Insertar y moverse al bloque final
    endBlock->insertInto(function);
    cgContext.irBuilder.SetInsertPoint(endBlock);

    lastValue = nullptr; 

    
};


void LlvmVisitor::visit(LetExpression* node)      
{
    cgContext.pushLocalScope();
    for(auto asg:node->assignments)
    {
        auto atrib=dynamic_cast<AttributeNode*>(asg);

        atrib->expression->accept(*this);
        llvm::Value* expValue=lastValue;

        llvm::AllocaInst* varAlloca =cgContext.createEntryBlockAlloca(currentFunction,expValue->getType() ,atrib->id.lexeme);

        cgContext.irBuilder.CreateStore(expValue,varAlloca);

        cgContext.addLocalVariable(atrib->id.lexeme,varAlloca);
   
    }
    
    node->body->accept(*this);
    auto bodyvalue=lastValue;
   
    lastValue=bodyvalue;
    cgContext.popLocalScope();

};


void LlvmVisitor::visit(FunCallNode* node) {
    // Buscar la función por su nombre
    std::string methname=node->id.lexeme;
    if(methname=="print")
    {
        node->arguments[0]->accept(*this);
        auto val=lastValue;
        llvm::Value* formatStr=nullptr;

        auto printf=cgContext.llvmModule.getFunction("printf");


        if (val->getType()->isPointerTy()) {

        formatStr = cgContext.irBuilder.CreateGlobalStringPtr("%s\n");
    } else if (val->getType()->isIntegerTy(1)) {

        llvm::Value* trueStr = cgContext.irBuilder.CreateGlobalStringPtr("true");
        llvm::Value* falseStr = cgContext.irBuilder.CreateGlobalStringPtr("false");
        
        val = cgContext.irBuilder.CreateSelect(val, trueStr, falseStr);
        formatStr = cgContext.irBuilder.CreateGlobalStringPtr("%s\n");
    } else if (val->getType()->isIntegerTy()) {

        formatStr = cgContext.irBuilder.CreateGlobalStringPtr("%d\n");
    } else if (val->getType()->isDoubleTy()) {

        formatStr = cgContext.irBuilder.CreateGlobalStringPtr("%.6g\n");
    } else {

        formatStr = cgContext.irBuilder.CreateGlobalStringPtr("Unsupported type\n");
    }


    lastValue= cgContext.irBuilder.CreateCall(printf, {formatStr, val});
    return;

    }


    if(cgContext.currentType!="")
    {
        if(node->id.lexeme=="base")
        {
            std::string fname=currentMethodName;
            auto currentTypeNode=cgContext.namedTypeNode[cgContext.currentType];
            auto currentTypeStr=cgContext.namedTypeNode[currentTypeNode->parentName.lexeme]->name.lexeme;
            std::string methodName =fname + "_" + currentTypeStr;
            
            llvm::Function* calleeFunc = cgContext.llvmModule.getFunction(methodName);
       
            while (!calleeFunc && !currentTypeStr.empty()) 
            {
                // Si no se encontró la función, buscar en el padre

                currentTypeNode = cgContext.namedTypeNode[currentTypeStr];
                if (!currentTypeNode || currentTypeNode->parentName.lexeme=="Object") {
                    break; 
                }

                // Obtener el nombre del nuevo método a buscar (en el padre)
                currentTypeStr = currentTypeNode->parentName.lexeme;
                methodName = fname + "_" + currentTypeStr;
                calleeFunc = cgContext.llvmModule.getFunction(methodName);
            }
            methname=methodName;
        }

        else
        {
            methname=methname+"_"+cgContext.currentType;
        }
    }
    llvm::Function* calleeFunc = cgContext.llvmModule.getFunction(methname);
    
    if (!calleeFunc) {
        std::cerr << "Error: función '" << node->id.lexeme << "' no definida.\n";
        lastValue = nullptr;
        return;
    }

   
    // Evaluar argumentos
    std::vector<llvm::Value*> args;
    if(cgContext.currentType!="")
    {
        auto sel=new IdentifierNode(Token("self",TokenType::Identifier,0,0));
        node->arguments.insert(node->arguments.begin(),sel);
    }
    for (AstNode* argNode : node->arguments) {
        argNode->accept(*this);  // llena lastValue
        if (!lastValue) {
            std::cerr << "Error: argumento inválido para llamada a función\n";
            lastValue = nullptr;
            return;
        }
        args.push_back(lastValue);
    }


    // Comprobación de aridad
    if (calleeFunc->arg_size() != args.size()) {
        std::cerr << "Error: número de argumentos no coincide con la función '" << node->id.lexeme << "'\n";
        lastValue = nullptr;
        return;
    }

    // Generar llamada
    lastValue = cgContext.irBuilder.CreateCall(calleeFunc, args, "calltmp");
  
    // Puedes establecer el tipo de retorno si tienes un sistema de tipos
    // node->setType(...);
}


void LlvmVisitor::visit(MemberCall* node) 
{
    
    node->obj->accept(*this);
    llvm::Value* objValue=lastValue;

    if (!objValue) 
      {
        std::cerr << "Error: objeto de MemberCall no válido\n";
        lastValue = nullptr;
        return;
    }
    std::string objTypeStr = node->obj->getType();
    llvm::StructType* objStruct = cgContext.namedStruct[objTypeStr];

    if (!objStruct) {
        std::cerr << "Error: tipo del objeto no encontrado: " << objTypeStr << "\n";
        lastValue = nullptr;
        return;
    }

    
    if(auto attr=dynamic_cast<IdentifierNode*>(node->member))
    {
        // auto selfAlloca=cgContext.getLocalVariable("self");
        // auto self=cgContext.irBuilder.CreateLoad(selfAlloca->getAllocatedType(),selfAlloca,"self");

        // auto selftype=cgContext.currentType;

        // auto typenode=cgContext.namedTypeNode[selftype];
        // auto selfStruct=cgContext.namedStruct[selftype];

        // int index=cgContext.GetIndexOfMember(typenode,attr->value.lexeme);
        // llvm::Value* memberPtr= cgContext.irBuilder.CreateStructGEP(selfStruct,self,index,"mem_ptr");

        // auto memberType=selfStruct->getElementType(index);
        // lastValue=cgContext.irBuilder.CreateLoad(memberType,memberPtr,"mem_val");
        // return;
        auto typenode=cgContext.namedTypeNode[objTypeStr];
        int index = cgContext.GetIndexOfMember(typenode, attr->value.lexeme);
        int structIndex=index;
        if (index < 0) {
            std::cerr << "Error: el atributo '" << attr->value.lexeme << "' no existe en " << objTypeStr << "\n";
            lastValue = nullptr;
            return;

        }
        if( typenode->parentName.lexeme!="" ) structIndex++;

        llvm::Value* gep = cgContext.irBuilder.CreateStructGEP(objStruct, objValue, structIndex, attr->value.lexeme + "_ptr");
        llvm::Type* attrType = objStruct->getElementType(structIndex);
        
        auto temp=cgContext.namedTypeNode[objTypeStr]->atributes[index];
        auto memberTypeStr=static_cast<AttributeNode*>(temp);
        if(cgContext.getInteralType(memberTypeStr->type)==nullptr)
        {
            auto ptr=llvm::PointerType::get(attrType,0);
            lastValue = cgContext.irBuilder.CreateLoad(ptr, gep, attr->value.lexeme);
            return;
        }

        lastValue = cgContext.irBuilder.CreateLoad(attrType, gep, attr->value.lexeme);
        return;
        
    }

    
   
 //  auto objtype=cgContext.namedStruct[node->obj->getType()];
    auto methcall=dynamic_cast<FunCallNode*>(node->member);
    std::string methodName = methcall->id.lexeme+ "_" +node->obj->getType();    
    llvm::Function* calleeFunc = cgContext.llvmModule.getFunction(methodName);
    std::string currentTypeStr = node->obj->getType();
    llvm::Value* currentObj = objValue;
    auto currentStructType=cgContext.namedStruct[node->obj->getType()];

    while (!calleeFunc && !currentTypeStr.empty()) 
    {
        // Si no se encontró la función, buscar en el padre
        auto typeNode = cgContext.namedTypeNode[currentTypeStr];
        if (!typeNode || typeNode->parentName.lexeme=="Object") {
            break; 
        }

        // Obtener el nombre del nuevo método a buscar (en el padre)
        currentTypeStr = typeNode->parentName.lexeme;
        methodName = methcall->id.lexeme + "_" + currentTypeStr;
        calleeFunc = cgContext.llvmModule.getFunction(methodName);

        // // Acceder al campo 0 (padre) del struct actual
        // llvm::StructType* structType = cgContext.namedStruct[node->obj->getType()];
        // llvm::Value* parentPtr = cgContext.irBuilder.CreateStructGEP(structType, currentObj, 0, "parent_ptr");

        // // Bitcast a tipo del padre
        // llvm::PointerType* parentTypePtr = llvm::PointerType::get(cgContext.namedStruct[currentTypeStr], 0);
        // currentObj = cgContext.irBuilder.CreateBitCast(parentPtr, parentTypePtr, "as_parent");

    }

    if (!calleeFunc) {
        std::cerr << "Error: función '" << methodName << "' no definida.\n";
        lastValue = nullptr;
        return;
    }

    std::vector<llvm::Value*> args;
    args.push_back(objValue);

    // Evaluar argumentos
    for (AstNode* argNode : methcall->arguments) {
        argNode->accept(*this);  // llena lastValue
        if (!lastValue) {
            std::cerr << "Error: argumento inválido para llamada a función\n";
            lastValue = nullptr;
            return;
        }
        args.push_back(lastValue);
    }

    // Comprobación de aridad
    if (calleeFunc->arg_size() != args.size()) {
        std::cerr << "Error: número de argumentos no coincide con la función '" << methodName << "'\n";
        lastValue = nullptr;
        return;
    }

    // Generar llamada
    lastValue = cgContext.irBuilder.CreateCall(calleeFunc, args, "calltmp");
  

};

void LlvmVisitor::visit(LiteralNode* node)
{
    if(node->type=="Number")
    {
        double numVal = std::stod(node->value.lexeme);
        lastValue = llvm::ConstantFP::get(cgContext.llvmContext, llvm::APFloat(numVal));

    }
    else if(node->type=="String")
    {
        lastValue=cgContext.irBuilder.CreateGlobalString(node->value.lexeme);
    }
    else if(node->type=="Boolean")
    {
        bool b= node->value.lexeme=="true";
        lastValue=llvm::ConstantInt::get(llvm::Type::getInt1Ty(cgContext.llvmContext),b);
    }

}

void LlvmVisitor::visit(TypeInstantiation* node) {
    std::string constructorName = "new_" + node->typeName.lexeme;
    llvm::Function* constructorFunc = cgContext.llvmModule.getFunction(constructorName);
    if (!constructorFunc) {
        std::cerr << "Error: constructor '" << constructorName << "' no definido.\n";
        lastValue = nullptr;
        return;
    }
    
    // Evaluar los argumentos de la instanciación y recolectarlos en un vector.
    std::vector<llvm::Value*> args;
    for (AstNode* arg : node->arguments) {
        arg->accept(*this);
        if (!lastValue) {
            std::cerr << "Error: argumento inválido en instanciación.\n";
            lastValue = nullptr;
            return;
        }
        args.push_back(lastValue);
    }
    
    // Generar la llamada al constructor. Se obtiene un pointer al objeto instanciado.
    auto obj = cgContext.irBuilder.CreateCall(constructorFunc, args, "calltmp");
    
    lastValue = obj;
}


void LlvmVisitor::visit(DestructiveAssignNode* node) 
{
    node->rhs->accept(*this);
    llvm::Value* right=lastValue;

    llvm::Value* ptr = nullptr;


     if (auto idNode = dynamic_cast<IdentifierNode*>(node->lhs)) 
     {
       
        ptr = cgContext.getLocalVariable(idNode->value.lexeme);
        if (!ptr) 
        {
            std::cerr << "Error: variable '" << idNode->value.lexeme << "' no definida\n";
            return;
        }

    }
    else if(auto selfmember = dynamic_cast<MemberCall*>(node->lhs))
    {
        auto attr=dynamic_cast<IdentifierNode*>(selfmember->member);
        auto  selfValue=cgContext.getLocalVariable("self");
        std::string objTypeStr = selfmember->obj->getType();

        llvm::StructType* objStruct = cgContext.namedStruct[objTypeStr];
        int index = cgContext.GetIndexOfMember(cgContext.namedTypeNode[objTypeStr], attr->value.lexeme);
        if (index < 0) {
            std::cerr << "Error: el atributo '" << attr->value.lexeme << "' no existe en " << objTypeStr << "\n";
            lastValue = nullptr;
            return;

        }
        llvm::Value* gep = cgContext.irBuilder.CreateStructGEP(objStruct, selfValue, index, attr->value.lexeme + "_ptr");
        cgContext.irBuilder.CreateStore(right, gep);
        lastValue = right;
        return;

    }
    else {
        std::cerr << "Error: LHS de := no soportado\n";
        return;
    }

    cgContext.irBuilder.CreateStore(right,ptr);

    lastValue=right;
    
}

void LlvmVisitor::visit(AttributeNode* node) {
    if (node->expression) {
        node->expression->accept(*this);  // Esto debería dejar el resultado en `lastValue`
    } else {
        // Default value si no hay inicializador explícito
        llvm::Type* ty = cgContext.getInteralType(node->getType());
        if (ty->isDoubleTy()) {
            lastValue = llvm::ConstantFP::get(ty, 0.0);
        } else if (ty->isIntegerTy()) {
            lastValue = llvm::ConstantInt::get(ty, 0);
        } else {
            lastValue = llvm::Constant::getNullValue(ty);
        }
    }
}

void LlvmVisitor::visit(UnaryExpression* node)       {};
void LlvmVisitor::visit(ForExression* node)          {};

