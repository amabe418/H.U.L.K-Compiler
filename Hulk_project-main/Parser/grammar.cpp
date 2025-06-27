#include "../include/Parser/grammar.hpp"

void Grammar::printParsingTable() 
{
    std::cout << "Tabla de Parsing LL(1):\n";
    for (const auto& row : parsing_table) {
        const Symbol& nonTerminal = row.first;
        std::cout << nonTerminal.value << " -> ";

        for (const auto& col : row.second) {
            const Symbol& terminal = col.first;
            int productionIndex = col.second;

            std::cout << "[" << terminal.value << ": " << productionIndex << "] ";
        }
        std::cout << "\n";
    }
}


ParseTree Grammar::parse(std::vector<Token> inpTokens)
{
    ParseNode* root= new ParseNode(&startSymbol);
    ParseNode* endf= new ParseNode (new Symbol("EOFs",SymbolType::Terminal));
    ParseTree tree(root);
    std::stack<ParseNode*> stack;
    int index=0;
    stack.push(endf);
    stack.push(root);
    
    while(!stack.empty())
    {
        auto X=stack.top();
        
        if(X->m_value->type==SymbolType::NonTerminal)
        {
            Symbol lookahead(getStringOfToken(inpTokens[index].type),SymbolType::Terminal);
            if(parsing_table[*(X->m_value)].find(lookahead)==parsing_table[*(X->m_value)].end())
            {
                
                auto expected=first[*(X->m_value)];
                std::string msg="Unexpected token '"+ inpTokens[index].lexeme;
                errorHandler.reportError(inpTokens[index],msg);
                return tree;

                // throw std::runtime_error(msg);
            }

            int in=parsing_table[*X->m_value][lookahead];
            stack.pop();
            
            Production p=productions[in];
            
            for(int i=p.right.size()-1; i>=0; i--)
            {
                auto childNode = new ParseNode (&(productions[in].right[i]));
                childNode->m_parent=X;
                X->m_children.push_back(childNode);
                stack.push(childNode);

            } 
            // if(p.right.size()==0)
            // {
            //     auto parent=X->m_parent;
            //     X->m_parent=nullptr;
            //     parent->m_children.erase(parent->m_children.begin());

            // }
        }
        else if(X->m_value->type==SymbolType::Terminal)
        {
            if(X->m_value->value==getStringOfToken(inpTokens[index].type))
            {
                X->m_token = inpTokens[index];

                stack.pop();
                index++;
            }
            else
            {
                
                std::string msg="Unexpected token '"+ inpTokens[index].lexeme+"' expected "+ X->m_value->value;
                errorHandler.reportError(inpTokens[index],msg);
                return tree;

                //throw std::runtime_error("Parser error , no matched ");
            }
        }

        // epsilon
        else
        {

            stack.pop();
        }
        

    }
    return tree;

}  


Grammar Grammar::loadGrammar(std::string filename)
{

    ifstream infile(filename);
    std::string line;
    bool readingProductions = false;
    std::vector<string> productionLines;
    
    Symbol startSymbol;
    std::set<Symbol> terminals;
    std::set<Symbol> non_terminals;
    std::vector<Production> productions;


    while(getline(infile, line)) 
    {
        line = trim(line);
        if(line.empty()) continue;
        
        // Si la línea comienza con "Terminals:"
        if(line.find("Terminals:") == 0) {
            size_t colonPos = line.find(":");
            string termsStr = trim(line.substr(colonPos + 1));
            istringstream iss(termsStr);
            string name;
            // Dividimos por comas
            while(getline(iss, name, ',')) {
                name = trim(name);
                if(!name.empty())
                    terminals.insert(Symbol(name, SymbolType::Terminal));
            }
        }
        // Si la línea comienza con "NonTerminals:"
        else if(line.find("NonTerminals:") == 0) {
            size_t colonPos = line.find(":");
            string ntsStr = trim(line.substr(colonPos + 1));
            istringstream iss(ntsStr);
            string name;
            while(getline(iss, name, ',')) {
                name = trim(name);
                if(!name.empty())
                    non_terminals.insert(Symbol(name, SymbolType::NonTerminal));
            }
        }
        // Si la línea comienza con "Productions:" se activa el modo de lecturas de producciones.
        else if(line.find("Productions:") == 0) {
            readingProductions = true;
        }
        else if(readingProductions) {
            // Cada línea leída en esta sección se considera una producción.
            productionLines.push_back(line);
        }
    }
    infile.close();

    // Se asigna el símbolo inicial con el LHS de la primera producción (si existe)
    if (!productionLines.empty()) {
        size_t arrowPos = productionLines[0].find("->");
        string startLHS = trim(productionLines[0].substr(0, arrowPos));
        startSymbol = Symbol(startLHS, SymbolType::NonTerminal);
    }
    
    // Procesar cada producción
    for(const auto &prodLine : productionLines) {
        // Se espera el formato: LHS -> RHS
        size_t arrowPos = prodLine.find("->");
        if(arrowPos == string::npos) continue;
        
        string lhsStr = trim(prodLine.substr(0, arrowPos));
        Symbol lhsSymbol(lhsStr, SymbolType::NonTerminal);  // Se asume que LHS es no terminal
        string rhsStr = trim(prodLine.substr(arrowPos + 2));
        
        // Dividir el RHS en posibles alternativas separadas por '|'
        vector<string> alternatives;
        istringstream altStream(rhsStr);
        string alt;
        while(getline(altStream, alt, '|')) {
            alternatives.push_back(trim(alt));
        }
        
        // Para cada alternativa, se crea una producción
        for(auto &altStr : alternatives) {
            vector<Symbol> rhsSymbols;
            if(altStr == "ε" || altStr.empty()) {
                // Producción vacía: dejamos el vector de símbolos vacío.
            } else {
                // Tokenizamos la alternativa por espacios
                istringstream tokenStream(altStr);
                string token;
                while(tokenStream >> token) {
                    // Si el token es ε se omite (o se podría tratar de forma especial)
                    if(token == "ε")
                        continue;
                    
                    // Se determina el tipo del token:
                    // Si el token se encuentra en el conjunto de no terminales, es no terminal.
                    SymbolType type = SymbolType::Terminal;
                    if(non_terminals.find(Symbol(token, SymbolType::NonTerminal)) != non_terminals.end())
                        type = SymbolType::NonTerminal;
                    
                    rhsSymbols.push_back(Symbol(token, type));
                }
            }
            Production prod(lhsSymbol, rhsSymbols);
            productions.push_back(Production(lhsSymbol, rhsSymbols));
        }
    }

    // (Opcional) Asegurar que en el conjunto de símbolos terminales y no terminales
    // incluyamos también los que aparecen en las producciones.
    // Por cada producción se actualizan ambos conjuntos.
    // for (const auto &prod : productions) {
    //     non_terminals.insert(prod.left);
    //     for (const auto &sym : prod.right) {
    //         if(sym.type == SymbolType::NonTerminal)
    //             non_terminals.insert(sym);
    //         else if(sym.type == SymbolType::Terminal)
    //             terminals.insert(sym);
    //     }
    // }

    return  Grammar(startSymbol,terminals,non_terminals,productions);

}




std::map<Symbol, std::set<Symbol>> Grammar::calculateFirst()
{
    std::map<Symbol, std::set<Symbol>> first_set;
    
    Symbol eps("ε", SymbolType::Epsilon);
    first_set[eps].insert(eps);

    // Para cada terminal, FIRST(terminal) es el terminal mismo.
    for (const auto &terminal : terminals) {
        first_set[terminal].insert(terminal);
    }

    // Para cada no terminal, inicializamos FIRST con un conjunto vacío.
    for (const auto &non_terminal : non_terminals) {
        first_set[non_terminal] = std::set<Symbol>();
    }
    
    bool changed = true;
    while (changed) 
    {
        changed = false;
        // Recorremos todas las producciones A -> X1 X2 ... Xn
        for (const auto &prod : productions)
        {
            const Symbol &A = prod.left; 
            
            // if (prod.right.empty()) {
            //     if (first[A].find(EPSILON) == first[A].end()) {
            //         first[A].insert(EPSILON);
            //         changed = true;
            //     }
            //     continue;
            // }
            
            bool allEpsilon = true;
            // Iteramos sobre los símbolos X1, X2, …, Xn de la producción.
            for (const auto &X : prod.right) {
                size_t beforeSize = first_set[A].size();
                // Agregamos a FIRST(A) todos los terminales de FIRST(X) excepto EPSILON.
                for (const auto &sym : first_set[X]) {  // first[X] puede usarse si X ya fue inicializado.
                    if (sym.type!=SymbolType::Epsilon) {
                        first_set[A].insert(sym);
                    }
                }
                if (first_set[A].size() > beforeSize)
                    changed = true;
                
                // Si FIRST(X) no contiene ε, la cadena derivada no puede continuar derivando ε.
                // if (first.at(X).find(EPSILON) == first.at(X).end()) {
                //     allEpsilon = false;
                //     break;
                // }
                bool eps=false;
                for(auto &s:first_set[X])
                {
                    if(s.type==SymbolType::Epsilon)
                    {
                        eps=true;
                    }
                }
                if(!eps)
                {
                    allEpsilon = false;
                    break;   
                }
            }
            // Si todos los símbolos X1 ... Xn tienen ε en su FIRST, entonces ε está en FIRST(A).
            if (allEpsilon) {
                // if (first[A].find(EPSILON) == first[A].end()) {
                //     first[A].insert(EPSILON);
                //     changed = true;
                // }
                bool eps=false;
                for(auto &s:first_set[A])
                {
                    if(s.type==SymbolType::Epsilon)
                    {
                        eps=true;
                    }
                }
                if(!eps)
                {
                    first_set[A].insert(Symbol("ε",SymbolType::Epsilon));
                    changed = true;
                }
            }
        }
    }
    
    return first_set;
}

std::set<Symbol> Grammar::list_first(std::vector<Symbol> list)
{
    std::set<Symbol> result;
    bool eps=true;
    for (size_t i=0; i<list.size();i++)
    {
        if(eps)
        {
            auto first_i=first[list[i]];
            eps=false;
            for(auto terminal:first_i)
            {
    
                if(terminal.type!=SymbolType::Epsilon)
                {
                    result.insert(terminal);

                }
                else    eps=true;   
            }
        }
    }
    if (eps)
    {
        result.insert(Symbol("ε",SymbolType::Epsilon));
    }
    return result;
}

std::map<Symbol, std::set<Symbol>> Grammar::calculateFollow()
{
    std::map<Symbol, std::set<Symbol>> follow;
    for (const auto &non_terminal : non_terminals) {
        follow[non_terminal] = std::set<Symbol>();
    }


    follow[startSymbol].insert(Symbol("EOFs",SymbolType::Terminal));

    bool changed = true;
    while (changed) 
    {
        changed = false;
        // Para cada producción A -> α.
        for (const auto &prod : productions) {
            const Symbol &A = prod.left;
            const std::vector<Symbol> &alpha = prod.right;
            // Recorremos cada símbolo en la parte derecha.
            for (size_t i = 0; i < alpha.size(); i++) 
            {
                const Symbol &B = alpha[i];
                // Solo los no terminales tienen conjunto FOLLOW.
                if (B.type!=NonTerminal)
                    continue;

                // Calculamos FIRST(β) donde β es la secuencia que sigue al símbolo B.
                std::set<Symbol> firstBeta;
                bool betaCanDeriveEpsilon = true;
                for (size_t j = i + 1; j < alpha.size(); j++) {
                    const Symbol &X = alpha[j];
                    // Agregamos FIRST(X) excepto ε.
                    for (const auto &sym : first.at(X)) {
                        if (sym.type==SymbolType::Epsilon)
                            firstBeta.insert(sym);
                    }
                    // Si FIRST(X) NO contiene ε, no evaluamos símbolos posteriores.
                    // if (firstMap.at(X).find(EPSILON) == firstMap.at(X).end()) {
                    //     betaCanDeriveEpsilon = false;
                    //     break;
                    // }

                    bool eps=false;
                    for(auto &s:first.at(X))
                    {
                        if(s.type==SymbolType::Epsilon)
                        {
                            eps=true;
                        }
                    }
                    if(!eps)
                    {
                        betaCanDeriveEpsilon = false;
                        break;
                    }
                }

                // Tomamos el tamaño actual del FOLLOW(B) para ver si hay cambios.
                size_t beforeSize = follow[B].size();
                // Se agregan todos los símbolos de FIRST(β) (excluyendo ε) a FOLLOW(B).
                follow[B].insert(firstBeta.begin(), firstBeta.end());
                // Si β es vacío o puede derivar ε, se agrega FOLLOW(A) a FOLLOW(B).
                if (betaCanDeriveEpsilon || i == alpha.size() - 1) {
                    follow[B].insert(follow[A].begin(), follow[A].end());
                }
                if (follow[B].size() > beforeSize)
                    changed = true;
            }
        }
    }

    return follow;
}

std::map<Symbol, std::set<Symbol>> Grammar::calculateFollow2()
{
    std::map<Symbol, std::set<Symbol>> follow;
    for (const auto &non_terminal : non_terminals) {
        follow[non_terminal] = std::set<Symbol>();
    }


    follow[startSymbol].insert(Symbol("EOFs",SymbolType::Terminal));
    bool changed=true;
    while(changed)
    {
        changed=false;

        for(auto [A,alpha]:productions)
        {
            for (size_t i=0;i<alpha.size();i++)
            {
                if(alpha[i].type!=SymbolType::NonTerminal) continue;

                std::vector<Symbol> sub(alpha.begin()+ i+1,alpha.end()) ; 
                auto lf=list_first(sub) ;
                
                bool eps=false;
                for (auto literal :lf)
                {
                    if(literal.type!=SymbolType::Epsilon)
                    {
                        auto [_,insert] =follow[alpha[i]].emplace(literal);
                        if(insert)   changed=true;
                            
                    }
                    else eps=true;
                }

                if(i==alpha.size()-1 || eps)
                {
                    for(auto literal:follow[A])
                    {
                        auto [_,insert] =follow[alpha[i]].emplace(literal);
                        if(insert)   changed=true;
                    }
                }
            }

        }
    }
    
    return follow;
}


void Grammar::BuildTable()
{
    for (size_t i = 0; i < productions.size(); i++) // for each A -> a
    {
            auto [left,right]= productions[i];
        // if(right[0].type==SymbolType::Epsilon)
        // {
        //     auto follow_i=follow[left];
        //     for (auto terminal : follow_i)
        //     {
        //         if(terminal.type==SymbolType::Epsilon)  continue;
        //         m_parsing_table[left][terminal]=i;
        //     }
        //     continue;
        // }

        // auto first_i=first[left];
        // for (auto terminal : first_i)
        // {
        //     if(terminal.type==SymbolType::Epsilon)  continue;
        //     m_parsing_table[left][terminal]=i;
        // }
        auto lf=list_first(right);
        for (auto terminal : lf)
        {
            if(terminal.type==SymbolType::Epsilon) continue; ;
            if( parsing_table[left][terminal])
            {
                
                errorHandler.reportError(Token(),"Grammar must be ll1");
                return;
            }
            parsing_table[left][terminal]=i;
        }
        if(lf.find(Symbol("ε",SymbolType::Epsilon))!=lf.end())
        {
            auto follow_i=follow[left];
            for (auto terminal : follow_i)
            {
                if(terminal.type==SymbolType::Epsilon)  continue;  ;
                if( parsing_table[left][terminal])
                {
                    errorHandler.reportError(Token(),"Grammar must be ll1");
                    return;
                }
                parsing_table[left][terminal]=i;
            }
            continue;
        }


        // auto first_i=first[left];
        // for (auto terminal : first_i)
        // {
        //     if(terminal.type==SymbolType::Epsilon) continue; ;
        //     m_parsing_table[left][terminal]=i;
        // }
    }
            
}


std::string Grammar::trim(const std::string &s) 
{
    size_t start = s.find_first_not_of(" \t");
    if(start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}