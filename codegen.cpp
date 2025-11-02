#include<bits/stdc++.h>
using namespace std;
#define N 1000010
struct node{
    int op,x,y,p0,p1;
}d[N];
string names[N];
int dcnt = 1;
int newnode(int op,int x,int y,int p0,int p1){
    int ret = dcnt++;
    names[ret]=to_string(ret);
    d[ret]=(node){op,x,y,p0,p1};
    return ret;
}
#define PUSH 0
#define SWAP 1
#define NIL SWAP,0,0
void connect0(int x,int y){
    d[x].p0=y;
}
void connect1(int x,int y){
    d[x].p1=y;
}
struct Bit{
    int f,r0,r1,b,bp0,bp1;
    void init0(){
        bp0 = newnode(NIL,0,-1);
        bp1 = newnode(NIL,0,-1);
        b = newnode(NIL,bp0,bp1);
        r1 = newnode(PUSH,b,bp1,0,-1);
        r0 = newnode(PUSH,b,bp0,r1,-1);
        f = newnode(SWAP,b,0,0,-1);
    }
    void init1(){
        bp0 = newnode(NIL,0,-1);
        bp1 = newnode(NIL,0,-1);
        b = newnode(NIL,bp1,bp0);
        r1 = newnode(PUSH,b,bp1,0,-1);
        r0 = newnode(PUSH,b,bp0,r1,-1);
        f = newnode(SWAP,b,0,0,-1);
    }
};
namespace Logic{
    Bit B0,B1;
    void init(){
        B0.init0();
        B1.init1();
    }
    struct XorGate{
        int s,t;
        int _r1,_r2;
        void init(Bit x, Bit y, Bit z){
            s = newnode(NIL,x.b,-1);
            connect0(x.bp0,y.b);
            _r2 = newnode(SWAP,y.bp0,0,y.b,-1);
            _r1 = newnode(SWAP,y.bp1,0,_r2,-1);
            connect0(x.bp1,_r1);
            //now, set the primary and secondary output of y.bp0 and y.bp1
            connect0(y.bp0,z.r0);
            connect0(y.bp1,z.f);
            connect1(y.bp0,z.f);
            connect1(y.bp1,z.r0);
            t = newnode(NIL,0,-1);
            connect0(z.r1,t);
            connect0(z.f,t);
        }
    };
    struct AndGate{
        int s,t;
        void init(Bit x, Bit y, Bit z){
            s = newnode(NIL,x.b,-1);
            connect0(x.bp0,z.r0);
            connect0(x.bp1,y.b);
            connect0(y.bp0,z.r0);
            connect0(y.bp1,z.f);
            t = newnode(NIL,0,-1);
            connect0(z.r1,t);
            connect0(z.f,t);
        }
    };
    struct NotGate{
        int s,t;
        void init(Bit x,Bit y){
            s = newnode(NIL,x.b,-1);
            connect0(x.bp0,y.f);
            connect0(x.bp1,y.r0);
            t = newnode(NIL,0,-1);
            connect0(y.r1,t);
            connect0(y.f,t);
        }
    };
    struct OrGate{
        int s,t;
        Bit nx,ny,nz;
        NotGate ngx,ngy,ngz;
        AndGate a;
        void init(Bit x,Bit y,Bit z){
            nx.init0();ny.init0();nz.init0();
            ngx.init(x,nx); ngy.init(y,ny);
            a.init(nx,ny,nz);
            ngz.init(nz,z);
            s = newnode(NIL,ngx.s,-1);
            t = newnode(NIL,0,-1);
            connect0(ngx.t,ngy.s);
            connect0(ngy.t,a.s);
            connect0(a.t,ngz.s);
            connect0(ngz.t,t);
        }
    };
};
void print(){
    for(int i=1;i<dcnt;i++){
        printf("NODE %s(",names[i].c_str());
        if(d[i].p1==-1)d[i].p1=0;
        if(d[i].op==SWAP)printf("SWAP %s) ",names[d[i].x].c_str());
        else printf("PUSH %s %s) ",names[d[i].x].c_str(),names[d[i].y].c_str());
        printf("%s %s\n",names[d[i].p0].c_str(),names[d[i].p1].c_str());
    }
}
int main(){
    int S = dcnt++, I = dcnt++, O0 = dcnt++, O1 = dcnt++;
    names[0]="*";
    names[S]="S";names[I]="I";names[O0]="O0";names[O1]="O1";
    Logic::init();
    Bit x,y,z;
    x.init0();y.init0();z.init0();
    int c1 = newnode(PUSH,I,x.r0,-1,0);
    int c2 = newnode(PUSH,I,x.f,I,0);
    connect0(S,c1); connect0(c1,c2);
    int c3 = newnode(PUSH,I,y.r0,-1,0);
    int c4 = newnode(PUSH,I,y.f,I,0);
    connect0(x.f,c3); connect0(x.r1,c3); connect0(c3,c4);
    Logic::OrGate gate; gate.init(x,y,z);
    connect0(y.f,gate.s); connect0(y.r1,gate.s);
    connect0(gate.t,z.b);
    connect0(z.bp0,O0); connect0(z.bp1,O1);
    print();
}