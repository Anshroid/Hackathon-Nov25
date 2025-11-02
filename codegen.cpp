#include<bits/stdc++.h>
using namespace std;
#define N 1000010
struct node{
    int op,x,y,p0,p1;
}d[N];
string names[N];
int dcnt = 5;
int S = 1, I = 2, O0 = 3, O1 = 4;
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
struct Byte{
    Bit bd[8];
    void init0(){
        for(int i=0;i<8;i++)bd[i].init0();
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
    struct DuplicateGate{
        int s,t;
        void init(Bit x,Bit y, Bit z){// The r1&f of y&z are occupied, meaning they aren't writable.
            s = newnode(NIL,x.b,-1);
            connect0(x.bp0,y.r0);
            connect0(x.bp1,y.f);
            t = newnode(NIL,0,-1);
            connect0(y.r1,z.r0);
            connect0(y.f,z.f);
            connect0(z.r1,t);
            connect0(z.f,t);
        }
    };
    struct HalfAdder{
        int s,t;
        Bit i0_d1,i1_d1,i0_d2,i1_d2;
        DuplicateGate d0,d1;
        XorGate x;
        AndGate a;
        void init(Bit i0,Bit i1,Bit o0,Bit o1){
            i0_d1.init0();i1_d1.init0();
            i0_d2.init0();i1_d2.init0();
            d0.init(i0,i0_d1,i0_d2);
            d1.init(i1,i1_d1,i1_d2);
            s = newnode(NIL,d0.s,-1);
            connect0(d0.t,d1.s);
            x.init(i0_d1,i1_d1,o0);
            a.init(i0_d2,i1_d2,o1);
            connect0(d1.t,x.s);
            connect0(x.t,a.s);
            t = newnode(NIL,0,-1);
            connect0(a.t,t);
        }
    };
    struct ByteNot{
        int s,t;
        NotGate ng[8];
        void init(Byte x,Byte y){
            for(int i=0;i<8;i++)ng[i].init(x.bd[i],y.bd[i]);
            s=newnode(NIL,0,0);
            t=newnode(NIL,0,0);
            connect0(s,ng[0].s);
            for(int i=0;i<7;i++){
                connect0(ng[i].t,ng[i+1].s);
            }
            connect0(ng[7].t,t);
        }
    };
};
namespace IO{
    struct Reader{
        int s,t;
        void init(Bit x){
            int c1 = newnode(PUSH,I,x.r0,-1,0);
            int c2 = newnode(PUSH,I,x.f,I,0);
            s = newnode(NIL,c1,0);
            t = newnode(NIL,0,0);
            connect0(c1,c2);
            connect0(x.r1,t);
            connect0(x.f,t);
        }
    };
    struct Writer{
        int s,t;
        void init(Bit x){
            s = newnode(NIL,0,0);
            t = newnode(NIL,0,0);
            int c1 = newnode(PUSH,O0,0,O0,0);
            int c2 = newnode(PUSH,O0,t,c1,0);
            int c3 = newnode(PUSH,O1,0,O1,0);
            int c4 = newnode(PUSH,O1,t,c3,0);
            connect0(s,x.b);
            connect0(x.bp0,c2);
            connect0(x.bp1,c4);
        }
    };
    struct ByteReader{
        Reader rd[8];
        int s,t;
        void init(Byte x){
            s=newnode(NIL,0,0);
            t=newnode(NIL,0,0);
            for(int i=0;i<8;i++){
                rd[i].init(x.bd[i]);
            }
            connect0(s,rd[0].s);
            for(int i=0;i<7;i++)connect0(rd[i].t,rd[i+1].s);
            connect0(rd[7].t,t);
        }
    };
    struct ByteWriter{
        Writer wd[8];
        int s,t;
        void init(Byte x){
            s=newnode(NIL,0,0);
            t=newnode(NIL,0,0);
            for(int i=0;i<8;i++)wd[i].init(x.bd[i]);
            connect0(s,wd[0].s);
            for(int i=0;i<7;i++)connect0(wd[i].t,wd[i+1].s);
            connect0(wd[7].t,t);
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
void Test1(){
    Bit x,y,z,w;
    x.init0();y.init0();z.init0();w.init0();
    IO :: Reader rx,ry;
    rx.init(x);ry.init(y);
    connect0(S,rx.s); connect0(rx.t,ry.s);
    Logic::HalfAdder ha; ha.init(x,y,z,w);
    connect0(ry.t,ha.s);
    IO :: Writer wz,ww;
    wz.init(z);ww.init(w);
    connect0(ha.t,wz.s);
    connect0(wz.t,ww.s);
    connect0(ww.t,0);
    print();
}
int main(){
    names[0]="*";
    names[S]="S";names[I]="I";names[O0]="O0";names[O1]="O1";
    Logic::init();
    Byte x,y;
    x.init0();y.init0();
    IO::ByteReader rx;
    rx.init(x);
    connect0(S,rx.s);
    Logic::ByteNot bn;
    bn.init(x,y);
    connect0(rx.t,bn.s);
    IO::ByteWriter wy;
    wy.init(y);
    connect0(bn.t,wy.s);
    connect0(wy.t,0);
    print();
}