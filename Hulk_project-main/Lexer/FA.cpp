#include "../include/Lexer/NFA.hpp"

   
int DFA::nextState(char c, int state)
   
{
    if(!is_valid(c))  return -2;   // wrong character
    
    auto tr=m_transitions.at(state);
    std::vector<int> states={};
    for (auto t:tr)
    {
        if(t.character==c || t.type=="any") 
            return t.to[0];
        
    }
    return -1;
}
    
bool DFA::is_valid(const char symbol)
{
    if (m_alphabet.find(symbol)==m_alphabet.end())
    {
        
        return false;
    }
    return true;
}

bool DFA::is_final_state(int current_state)
{
    for(int f_s :m_final_states){
        if(current_state==f_s)  return true;
    }
    return false;
}

bool DFA::evualuate(std::string input)
{
        
    int current_state=m_start_state;
    for(char symbol :input)
    {
        current_state=nextState(symbol,current_state);
    }

    return is_final_state(current_state);
}
    
 bool DFA::existsDfa()
    {
        return std::filesystem::exists("dfa.bin");
    }
    

//     static void save_binary(const DFA& dfa, const std::string& filename="dfa.bin") {
//     std::ofstream out(filename, std::ios::binary);

//     out.write(reinterpret_cast<const char*>(&dfa.start_state), sizeof(int));
//     out.write(reinterpret_cast<const char*>(&dfa.total_states), sizeof(int));

//     int final_size = dfa.final_states.size();
//     out.write(reinterpret_cast<const char*>(&final_size), sizeof(int));
//     for (int f : dfa.final_states) {
//         out.write(reinterpret_cast<const char*>(&f), sizeof(int));
//     }

//     int alphabet_size = dfa.alphabet.size();
//     out.write(reinterpret_cast<const char*>(&alphabet_size), sizeof(int));
//     for (char c : dfa.alphabet) {
//         out.write(&c, sizeof(char));
//     }

//     int num_states = dfa.transitions.size();
//     out.write(reinterpret_cast<const char*>(&num_states), sizeof(int));
//     for (const auto& [state, map] : dfa.transitions) {
//         out.write(reinterpret_cast<const char*>(&state), sizeof(int));
//         int num_transitions = map.size();
//         out.write(reinterpret_cast<const char*>(&num_transitions), sizeof(int));
//         for (const auto& [symbol, next] : map) {
//             out.write(&symbol, sizeof(char));
//             out.write(reinterpret_cast<const char*>(&next), sizeof(int));
//         }
//     }
// }

//     static DFA load_binary(const std::string& filename="dfa.bin")
//     {
//         std::ifstream in(filename, std::ios::binary);
//         DFA dfa;

//         in.read(reinterpret_cast<char*>(&dfa.start_state), sizeof(int));
//         in.read(reinterpret_cast<char*>(&dfa.total_states), sizeof(int));

//         int final_size;
//         in.read(reinterpret_cast<char*>(&final_size), sizeof(int));
//         dfa.final_states.resize(final_size);
//         for (int& f : dfa.final_states) {
//             in.read(reinterpret_cast<char*>(&f), sizeof(int));
//         }

//         int alphabet_size;
//         in.read(reinterpret_cast<char*>(&alphabet_size), sizeof(int));
//         for (int i = 0; i < alphabet_size; ++i) {
//             char c;
//             in.read(&c, sizeof(char));
//             dfa.alphabet.insert(c);
//         }

//         int num_states;
//         in.read(reinterpret_cast<char*>(&num_states), sizeof(int));
//         for (int i = 0; i < num_states; ++i) {
//             int state;
//             in.read(reinterpret_cast<char*>(&state), sizeof(int));
//             int num_transitions;
//             in.read(reinterpret_cast<char*>(&num_transitions), sizeof(int));
//             for (int j = 0; j < num_transitions; ++j) {
//                 char sym;
//                 int dest;
//                 in.read(&sym, sizeof(char));
//                 in.read(reinterpret_cast<char*>(&dest), sizeof(int));
//                 dfa.transitions[state][sym] = dest;
//             }
//         }

//         return dfa;
// }


 


bool NFA::is_final_state(int current_state)
{
    for(int f_s :m_final_states){
        if(current_state==f_s)  return true;
    }
    return false;
}
    
std::pair<std::vector<int>,bool> NFA::nextStates(char c, int state,bool useAny) const 
{
    if(m_transitions.find(state)==m_transitions.end())  return{{},false};
    auto tr=m_transitions.at(state);
    std::vector<int> states={};
    bool any=false;
    for (auto t:tr)
    {
        if(t.character==c || (useAny &&  t.type=="any")) 
        {
            if(t.type=="any")   any=true;
                states.insert(states.end(), t.to.begin(), t.to.end());
        }
                        
    }
    return {states,any};
}
    
bool NFA::is_valid(const char symbol)
{
    if (m_alphabet.find(symbol)==m_alphabet.end())
    {
        std::cout<<"el caracter" <<symbol<< " no pertenece a el alfabeto"<<std::endl ;
        return false;
    }
    return true;
}

bool NFA::evaluate(std::string input)
{
    std::vector<int> cs={m_start_state};
    for(char symbol :input)
    {
        std::vector<int> aux={};

        for (size_t i=0;i<cs.size();i++)
        {
            auto [eps,_]=eClousure(cs[i],false);
            for(int i:eps) cs.push_back(i);

            auto s=nextStates(symbol,cs[i],true);
            for(int i:s.first) aux.push_back(i); 
        }
        cs=aux;
    }

    for (size_t i=0;i<cs.size();i++)
    {
        auto [eps,_]=eClousure(cs[i],false);
        for(int i:eps) cs.push_back(i);

        if(is_final_state(cs[i]))    return true;
    }
    return false;
    }

DFA NFA::convertToDFA()
{
    std::map<std::set<int>,int> dfn_states={};
    std::queue<std::set<int>> queue={};
    std::map<int, std::vector<Transition>> trs={};
    int cr=0;
    auto [start,a]=eClousure_set({m_start_state});
    queue.push(start);
    dfn_states[start]=cr;
    cr++;
    while (!queue.empty())
    {
        std::set<int> T=queue.front();
        for (char sym:m_alphabet)
        {
            if (sym=='$' ||sym=='~') continue;
            auto [moveSet,useAny]=move(T,sym);
            auto [u,useAnyE]=eClousure_set(moveSet);
            if(dfn_states.find(u)==dfn_states.end())
            {
                if (u.size()==0)    dfn_states[u]=-1;
                else{
                    dfn_states[u]=cr;
                    cr++;
                }
                
                queue.push(u);
            }

            if(trs.find(dfn_states[T])==trs.end())  trs[dfn_states[T]]={};
            if(!useAny )
                trs[dfn_states[T]].push_back(Transition(sym,{dfn_states[u]},"normal"));
            else
            {
                trs[dfn_states[T]].push_back(Transition(sym,{dfn_states[u]},"any"));
                break;
            }
            
        }
        queue.pop();
    }

    std::map<int, TokenType> dfa_final_tokens;
    std::vector<int> finalStates ={};
    for (auto pair : dfn_states)
    {
        int dfa_state_id = pair.second;
        TokenType token = UNKNOWN; 
        for (int state_nfa: pair.first)
        {
            
            if (is_final_state(state_nfa))
            {
                auto t=m_final_token_types[state_nfa];
                if(morePriority(t,token))    
                {
                    token=t;
                }

                finalStates.push_back(pair.second);
            }
        }

        dfa_final_tokens[dfa_state_id]=token;

    }
    std::set<char> d_al=m_alphabet;
    if(d_al.find('$')==d_al.end())  d_al.erase('$');

    DFA dfa(0,dfn_states.size(),finalStates,m_alphabet,trs);
    dfa.m_final_token_types=dfa_final_tokens;
    return dfa;
}


    // factory for regular expressions
NFA NFA::emptyRE()
{
    return NFA(0,0,{},{},{});
}

NFA NFA::epsilonRE()
{
    std::map<int,std::vector<Transition>> tras={};
    tras[0]={Transition('$',{1},"epsilon")};
    return NFA(0,2,{1},{'$'},tras);
}

NFA NFA::anyRE()
{
    std::map<int,std::vector<Transition>> tras={};
    tras[0]={Transition('~',{1},"any")};
    return NFA(0,2,{1},{'~'},tras);
}

NFA NFA::symbolRE(char symbol) 
{
    std::map<int,std::vector<Transition>> tras;
    tras[0]={Transition(symbol,{1},"normal")};
    std::set<char> alphabet{symbol};
    return NFA(0, 2, {1}, alphabet, tras);
}

NFA NFA::RangeRE(char from,char to)
{
    
    int start = 0;
    int end = to-from+2;
    std::set<char> alpha={};
    std::map<int,std::vector<Transition>> t={};
    int state=1;
    for (char c = from; c <= to; ++c,state++)
    {
        alpha.insert(c);
        t[start].push_back(Transition(c,{state},"normal"));
        t[state].push_back(Transition('$',{end},"epsilon"));

    } 
    return NFA(start,to-from+3,{end},alpha,t);
}

NFA NFA::UnionRE(NFA a1,NFA a2)
{
    int startstate= a1.m_total_states+a2.m_total_states;
    std::vector<int> finals=a1.m_final_states;
    int totalstates=a1.m_total_states+a2.m_total_states+2;

    std::set<char> alphabet=a1.m_alphabet;
    alphabet.merge(a2.m_alphabet);

    std::map<int, std::vector<Transition>> final_transitions={};
    final_transitions=a1.m_transitions;
    for (auto [state,transition]: a2.m_transitions)
    {
        int new_state=state+a1.m_total_states;
        for (auto t: transition)
        {
            auto new_T=t;
            for (size_t i = 0; i < new_T.to.size(); i++)
            {
                    new_T.to[i]+=a1.m_total_states;
            }
            
            final_transitions[new_state].push_back(new_T);
        }
    }

    if(alphabet.find('$')==alphabet.end())
    {
        alphabet.insert('$');
    }

    final_transitions[startstate]= { Transition( '$',{a1.m_start_state,a2.m_start_state+a1.m_total_states},"epsilon")};

    auto final_token_types=a1.m_final_token_types;
    for (auto pair: a2.m_final_token_types)
    {
        final_token_types[pair.first+a1.m_total_states]=pair.second;
    }

    for (int fs: a2.m_final_states)
    {
        finals.push_back(fs+a1.m_total_states);
    }
    
    NFA res(startstate,totalstates,finals,alphabet,final_transitions);
    res.m_final_token_types=final_token_types;
    return res;
}

NFA NFA::ConcatenationRE(NFA a1,NFA a2)
{
    int startstate= a1.m_start_state;
    int totalstates=a1.m_total_states+a2.m_total_states;

    std::set<char> alphabet=a1.m_alphabet;
    alphabet.merge(a2.m_alphabet);

    std::vector<int>final_states={};
    for (int fs:a2.m_final_states)
    {
        final_states.push_back(fs+a1.m_total_states);
    }

    std::map<int, std::vector<Transition>> final_transitions={};
    final_transitions=a1.m_transitions;

    for (auto [state,a2_transitions]: a2.m_transitions)
    {
        int new_state=state+a1.m_total_states;
        for (auto a2_t: a2_transitions)
        {
            auto  new_T=a2_t;
            for (size_t i = 0; i < new_T.to.size(); i++)
            {
                    new_T.to[i]+=a1.m_total_states;
            }   
            final_transitions[new_state].push_back(new_T);
        }
    }
    if(alphabet.find('$')==alphabet.end())
    {
        alphabet.insert('$');
    }
    for (int fs:a1.m_final_states)
    {
        final_transitions[fs]={Transition('$',{a2.m_start_state+a1.m_total_states},"epsilon")};
    }

    return NFA(startstate,totalstates,final_states,alphabet,final_transitions);
    
}

NFA NFA::ClousureRE(NFA a1)
{

    int startstate=a1.m_total_states;
    int finalstate=a1.m_total_states+1;
    int totalStates=a1.m_total_states+2;

    auto transitions=a1.m_transitions;
    auto alphabet=a1.m_alphabet;

    if(alphabet.find('$')==alphabet.end())
    {
        alphabet.insert('$');
    }


    for (int fs : a1.m_final_states)
    {
        transitions[fs]={Transition('$',{a1.m_start_state,finalstate},"epsilon")};
    }
    transitions[startstate]={Transition('$',{a1.m_start_state,finalstate},"epsilon")};

    return NFA(startstate,totalStates,{finalstate},alphabet,transitions);

}

    
std::pair<std::set<int>,bool> NFA::eClousure(int state,bool initial)
{
    std::set<int> res={};
    std::queue<int> pending={};
    pending.push(state);
    
    bool any=false;
    while(!pending.empty())
    {
        int cr=pending.front();
        pending.pop();

        if(res.find(cr)!=res.end()) continue;

        auto [epst,useAny]=epsilonTransitions(cr);
        if(!any)    any=useAny;
        for(int i:epst)    
        {
            pending.push(i);
        }
        if (!initial && cr==state)   continue;
        res.insert(cr);

    }
    return {res,any};

}

std::pair<std::set<int>,bool> NFA::move(std::set<int>set,char symbol)
{
    std::set<int> res={};

    bool use_any=false;
    for (int state: set)
    {
        auto [s,any]=nextStates(symbol,state,true);
        if(!use_any)    use_any=any;
        for (int i :s){
            res.insert(i);
        }
    }
    return {res,use_any};
}

std::pair<std::set<int>,bool> NFA::eClousure_set(std::set<int> set)
{
    std::set<int> res={};
    bool any=false;
    for (int st: set)
    {
        auto [eclo,useany]=eClousure(st,true);
        if(!any)    any=useany;
        for (int i : eclo)
        {
            res.insert(i);
        }

    }
    return {res,any};
}

std::pair<std::vector<int>,bool> NFA::epsilonTransitions(int state) const 
{
    return nextStates('$',state,false);
}
