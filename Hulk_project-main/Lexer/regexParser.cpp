#include "../include/Lexer/regexParser.hpp"

REPtr RegexParser::parse_union() {
    auto left = parse_concat();
    if (eat('|')) {
        auto right = parse_union();
        return std::make_shared<UnionRE>(left, right);
    }
    return left;
}

REPtr RegexParser::parse_concat() {
    std::vector<REPtr> parts;
    while (true) {
        char c = peek();
        if (!c || c=='|' || c==')') break;
        parts.push_back(parse_closure());
    }
    if (parts.empty()) return std::make_shared<EmptyRE>();
    REPtr res = parts[0];
    for (size_t i=1; i<parts.size(); i++)
        res = std::make_shared<ConcatenationRE>(res, parts[i]);
    return res;
}

REPtr RegexParser::parse_closure() {
    auto atom = parse_atom();
    while (true) {
        char c = peek();
        if (c=='*') { get(); atom = std::make_shared<ClousureRE>(atom); }
        else if (c=='+') { get(); atom = std::make_shared<ConcatenationRE>(atom,
                          std::make_shared<ClousureRE>(atom)); }
        else if (c=='?') { get();
            // Represent ? as (a|ε)
            auto epsilon = std::make_shared<EpsilonRE>();
            atom = std::make_shared<UnionRE>(atom, epsilon);
        }
        else break;
    }
    return atom;
}

REPtr RegexParser::parse_atom(){
    if (eat('(')) {
        auto r = parse_union();
        if (!eat(')')) return nullptr;// throw std::runtime_error("Unclosed paren");
        return r;
    }
    else if (eat('[')) 
    {
        // parse character class
        bool neg = false;
        if (peek()=='^') { neg=true; get(); }
        std::vector<REPtr> ranges;

        while (peek() && peek()!=']') 
        {
            char first = get();
            if (peek()=='-' && input[pos+1]!=']') 
            {
                get();
                char last = get();
                if (first > last)
                       return nullptr; //throw std::runtime_error("Invalid range in character class");

                ranges.push_back(std::make_shared<RangeRE>(first, last));
            } else ranges.push_back(std::make_shared<SymbolRE>(first));
        }
        if (!eat(']'))  return nullptr;//throw std::runtime_error("Unclosed class");
        // create union of all elements
        REPtr result = ranges[0];
        for (size_t i = 1; i < ranges.size(); ++i) 
        {
            result = std::make_shared<UnionRE>(result, ranges[i]);
        }
        return result;
    }

    else if (eat('"')) {
        // Soporte para strings entre comillas dobles con escapes
        std::vector<REPtr> parts;
        while (peek() && peek() != '"') {
            char c = get();
            if (c == '\\') {
                if (pos >= input.size())
                    //throw std::runtime_error("Dangling backslash in string literal");
                     return nullptr;

                char esc = get();
                switch (esc) {
                    case 'n': c = '\n'; break;
                    case 't': c = '\t'; break;
                    case 'r': c = '\r'; break;
                    case '"': c = '"'; break;
                    case '\\': c = '\\'; break;
                    default:
                        //throw std::runtime_error(std::string("Unknown escape sequence: \\") + esc);
                         return nullptr;
                }
            }
            parts.push_back(std::make_shared<SymbolRE>(c));
        }

        if (!eat('"'))  return nullptr;//throw std::runtime_error("Unclosed string literal");

        if (parts.empty()) return std::make_shared<EpsilonRE>();

        REPtr res = parts[0];
        for (size_t i = 1; i < parts.size(); ++i) {
            res = std::make_shared<ConcatenationRE>(res, parts[i]);
        }
        return res;
    }
    else if(eat('.'))
    {
        return std::make_shared<AnyRE>();   
    }

    
    else if (peek())
    {
        char c = get();
        if (c == '\\') {
            if (pos >= input.size())
                // throw std::runtime_error("Dangling backslash in escape sequence");
                return nullptr;

            char next = get();
           return std::make_shared<SymbolRE>(next);
        }
        return std::make_shared<SymbolRE>(c);
    }
    
    //throw std::runtime_error("Unexpected end");
     return nullptr;

}
