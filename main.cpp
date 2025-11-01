#include <bits/stdc++.h>
using namespace std;

#define PUSH 0
#define SWAP 1
#define N 2020

map<string,int> mp;
string var[N];
queue<bool> input, output;

struct ins{
    int op, x, y;
    void print(){
        if(op == PUSH) printf("PUSH %s %s", var[x].c_str(), var[y].c_str());
        else           printf("SWAP %s", var[x].c_str());
    }
};

struct node{
    ins i;
    int p0, p1;
};

struct program{
    node d[N];
    int S, I, O0, O1, n, size;

    void step(){
        if(n == I){
            // assumes input is non-empty when visiting I
            bool x = input.front();
            input.pop();
            n = x ? d[I].p1 : d[I].p0;
            return;
        }else if(n == O0){
            output.push(0);
        }else if(n == O1){
            output.push(1);
        }else{
            ins cur_i = d[n].i;
            if(cur_i.op == PUSH){
                d[cur_i.x].p0 = d[cur_i.x].p1;
                d[cur_i.x].p1 = cur_i.y;
            }else{ // SWAP
                swap(d[cur_i.x].p0, d[cur_i.x].p1);
            }
        }
        n = d[n].p0;
    }

    void print(bool print_machine){
        if(print_machine){
            for(int i = 0; i < size; i++){
                printf("NODE %s(", var[i].c_str());
                d[i].i.print();
                printf("), p0 = %s, p1 = %s \n", var[d[i].p0].c_str(), var[d[i].p1].c_str());
            }
            printf("END\n");
        }
        printf("CUR = %s \n", var[n].c_str());
        printf("-------------------------\n");
    }

    void load(){
        ifstream in("input.glf");
        if(!in){ perror("input.glf"); return; }

        // reset state
        while(!input.empty()) input.pop();
        while(!output.empty()) output.pop();
        mp.clear();
        size = 0;

        auto trim = [](string &s){
            size_t a = s.find_first_not_of(" \t\r\n");
            size_t b = s.find_last_not_of(" \t\r\n");
            if (a == string::npos) { s.clear(); return; }
            s = s.substr(a, b - a + 1);
        };

        // Create/get id for a name; default to safe node (self-loops, SWAP self)
        auto getId = [&](const string& s)->int{
            auto it = mp.find(s);
            if(it != mp.end()) return it->second;
            int id = size++;
            mp[s] = id;
            var[id] = s;
            d[id].p0 = d[id].p1 = id; // self-loop
            d[id].i.op = SWAP;
            d[id].i.x  = id;          // SWAP self
            d[id].i.y  = 0;
            return id;
        };
        S = getId("S");
        I = getId("I");
        O0 = getId("O0");
        O1 = getId("O1");
        string line;
        // ---- Read NODE lines until END
        while (std::getline(in, line)) {
            string t = line; trim(t);
            if (t.empty()) continue;
            if (t == "END") break;
            if (t.rfind("NODE ", 0) != 0) continue;

            // Format: NODE <name>(<op...>) <p0> <p1>
            size_t pos  = 5; // after "NODE "
            size_t lpar = t.find('(', pos);
            size_t rpar = (lpar == string::npos) ? string::npos : t.find(')', lpar+1);
            if (lpar == string::npos || rpar == string::npos) continue;

            string name = t.substr(pos, lpar - pos); trim(name);
            int idx = getId(name);

            string inside = t.substr(lpar+1, rpar - lpar - 1); trim(inside);

            string rest = t.substr(rpar+1); trim(rest);
            string p0s, p1s;
            { stringstream rs(rest); rs >> p0s >> p1s; }
            int p0id = p0s.empty() ? idx : getId(p0s);
            int p1id = p1s.empty() ? idx : getId(p1s);

            // parse instruction
            ins insn{};
            {
                stringstream is(inside);
                string op; is >> op;
                if (op == "PUSH") {
                    string xs, ys; is >> xs >> ys;
                    insn.op = PUSH; insn.x = getId(xs); insn.y = getId(ys);
                } else if (op == "SWAP") {
                    string xs; is >> xs;
                    insn.op = SWAP; insn.x = getId(xs); insn.y = 0;
                } else {
                    // unknown => SWAP self as a safe default
                    insn.op = SWAP; insn.x = idx; insn.y = 0;
                }
            }

            d[idx].i  = insn;
            d[idx].p0 = p0id;
            d[idx].p1 = p1id;
        }

        // ---- After END: read CUR and INPUT until EOF (order-insensitive)
        while (std::getline(in, line)) {
            string t = line; trim(t);
            if (t.empty()) continue;

            if (t.rfind("CUR", 0) == 0) {
                size_t eq = t.find('=');
                if (eq != string::npos) {
                    string cur = t.substr(eq+1); trim(cur);
                    n = getId(cur);
                }
            } else if (t.rfind("INPUT", 0) == 0) {
                size_t eq = t.find('=');
                if (eq != string::npos) {
                    string bits = t.substr(eq+1); trim(bits);
                    for (char c : bits) {
                        if (c == '0') input.push(false);
                        else if (c == '1') input.push(true);
                    }
                }
            }
        }

        // Fill conventional special nodes if present
        if (mp.count("S"))  S  = mp["S"];
        if (mp.count("I"))  I  = mp["I"];
        if (mp.count("O0")) O0 = mp["O0"];
        if (mp.count("O1")) O1 = mp["O1"];
    }

    void init(){ n = S; }
} p;

int main(){
    p.load();
    p.init();
    p.print(true);

    // demo: step a bit and show CUR only
    for(int i = 0; i < 32; i++){
        p.step();
        p.print(true);
    }
    return 0;
}
