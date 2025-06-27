#include<string>
#include<set>
#include <iostream>
#include <map>
#include "../common/Error.hpp"
#include "../include/Lexer/regexParser.hpp"
#include <cctype>

class Lexer{
private:
    std::vector<pair<TokenType,std::string>>tokens_regExp;
    DFA finaldfa;
    int currentLine=1;
    int currentCol=1;
    

public:
    ErrorHandler errorHandler;

    Lexer( ErrorHandler& errorHandler):errorHandler(errorHandler)
    {
        
        // if(DFA::existsDfa())
        // {
        //     finaldfa=DFA::load_binary();
        //     return;
        // }

        std::vector<std::tuple<TokenType, std::string>> patterns={
            {TokenType::String,      "\\\"(.)*\\\""},
            {TokenType::Number,      "[0-9]+[0-9]*\\.?[0-9]*"}, 
            {TokenType::op_Modulo,       "%"},   
            {TokenType::punc_at,           "@"},   
            {TokenType::punc_doubleAt,     "@@"},  
            {TokenType::kw_extends,      "extends"}, 
            {TokenType::punc_LeftBracket,  "\\["},  
            {TokenType::punc_RightBracket, "\\]"},   
            {TokenType::punc_LeftBrace,    "{"},   
            {TokenType::punc_RightBrace,   "}"},   
            {TokenType::punc_LeftParen,    "\\("},  
            {TokenType::punc_RightParen,   "\\)"},  
            {TokenType::op_Greater,      ">"},   
            {TokenType::op_Less,         "<"},   
            {TokenType::punc_Semicolon,    ";"}, 
            {TokenType::punc_Colon,        ":"},   
            {TokenType::punc_Comma,        ","},   
            {TokenType::kw_type,         "type"}, 
            {TokenType::arrow,        "=>"},     
            {TokenType::Assignment,   "="},   
            {TokenType::kw_if,           "if"},       
            {TokenType::kw_else,         "else"},      
            {TokenType::kw_elif,         "elif"},
            {TokenType::kw_protocol,     "protocol"},
            {TokenType::kw_in,           "in"},
            {TokenType::kw_let,          "let"},
            {TokenType::kw_function,     "function"},
            {TokenType::kw_inherits,     "inherits"},
            {TokenType::kw_extends,      "extends"},   
            {TokenType::kw_while,        "while"},     
            {TokenType::kw_for,          "for"},       
            {TokenType::kw_true_,        "true"},
            {TokenType::kw_false_,       "false"},
            {TokenType::kw_new_,         "new"},
            {TokenType::kw_null_,        "null"},
            {TokenType::kw_is,         "is"},
            {TokenType::kw_as,         "as"},
            {TokenType::op_destruc,    ":="},
            {TokenType::op_LogicalOr,    "\\|\\|"},    
            {TokenType::op_Or,           "\\|"},       
            {TokenType::op_And,          "&"},       
            {TokenType::op_LogicalAnd,    "&&"},     
            {TokenType::op_Equal,        "=="},      
            {TokenType::op_NotEqual,     "!="},      
            {TokenType::op_Not,          "!"},       
            {TokenType::op_GreaterEqual, ">="},      
            {TokenType::op_LessEqual,    "<="},      
            {TokenType::op_Plus,         "\\+"},     
            {TokenType::op_Minus,        "\\-"},     
            {TokenType::op_Multiply,     "\\*"},     
            {TokenType::op_Divide,       "/"},       
            {TokenType::op_Exp,         "^"},         
            {TokenType::punc_Dot,          "\\."},   
            {TokenType::Identifier,      "[a-zA-Z]+[a-zA-Z0-9_]*"},
        };

        std::vector<NFA> nfas={};
    
        for(auto [type,pat]:patterns)
        {
            RegexParser p(pat);
            auto reg=p.parse();
            NFA nfa=reg->ConvertToNFA();
            for(int fs:nfa.m_final_states)
                {
                nfa.m_final_token_types[fs]=type;
                }
            nfas.push_back(nfa);
        }

        NFA nfa=nfas[0];

        for (size_t i=1; i<nfas.size();i++)
        {
            nfa= NFA::UnionRE(nfa,nfas[i]);
        }
        finaldfa=nfa.convertToDFA();
                 
    }

    std::vector<Token> scanTokens(std::string inp)
    {
        
        std::vector<Token> tokens={};
        size_t cr=0;

        while(cr<inp.size())
        {
            if(inp[cr]==' ')
            {
                currentCol+=1;
                cr++;
                continue;
            }
            if(inp[cr]=='\n')
            {
                currentLine++;
                currentCol=1;
                cr++;
                continue;
            }
            if(inp[cr]=='\t')
            {
                currentCol+=4;
                cr++;
                continue;
            }
            if(inp[cr]=='\r')
            {
                currentCol+=1;
                cr++;
                continue;
            }
            

            Token token= scanToken(inp,cr);
            if(token.type==TokenType::Error)
            {
                errorHandler.reportError(token,"Unrecognized token.");
                return tokens;
            }
            else
            {
                cr+=token.lexeme.length();
                currentCol+=token.lexeme.length();
                tokens.push_back(token);
            }
        }
        tokens.push_back(Token("EOF",EOFs,currentLine,currentCol));

        return tokens;
    }


private:

    Token scanToken(std::string inp,size_t cr)
    {
        
        size_t currentpos=cr;
        int currentState=finaldfa.m_start_state;


        bool firstC=false;


        while(currentpos <inp.size())
        {
            char n=inp[currentpos];
            if(n=='"' and !firstC )  firstC=true;
            else if(n=='"' and firstC)
            {
                currentpos++;
                TokenType t=TokenType::String;
                return Token(inp.substr(cr,currentpos-cr),t,currentLine,currentCol);
                break;

            }
            int nextstate=finaldfa.nextState(n,currentState);
            if (nextstate==-1)  break;
            if(nextstate==-2)
            {
                if(n==' '||n=='\n'||n=='\t'||n=='\r') break;  
                    return Token("error",Error,this->currentLine,this->currentCol);  

            }
            
            currentState=nextstate;
            currentpos++;
            
        }
        if (finaldfa.is_final_state(currentState) )
        {
                TokenType t=finaldfa.m_final_token_types[currentState];
                if(t==TokenType::Number && (std::isalpha(inp[currentpos]) ||inp[currentpos]=='_'||inp[currentpos]=='.' ))
                {
                return Token("error",Error,this->currentLine,this->currentCol);  
                }
                return Token(inp.substr(cr,currentpos-cr),t,currentLine,currentCol);
        }
        
        return Token("error",Error,this->currentLine,this->currentCol);
    }

};
