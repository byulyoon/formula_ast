#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct InterpData
{
    string expr;
    bool value;
};

class AtomicProps
{
public:
    vector<string> sets;
    AtomicProps(vector<string> s): sets(s) {}
};

class AST 
{
public:
    AST* ptr;
    vector<InterpData> interp;
    AST(AST* p): ptr(p), interp{} {}
    virtual ~AST() {}
    
    virtual void eval() { }
};

class Atom :public AST
{
public:
    string atom;
    Atom(string a): AST(this), atom(a)  { }

    void eval() override {
        InterpData d;
        d.expr = atom;
        d.value = true;
        interp.push_back(d);
        
        d.value = false;
        interp.push_back(d);
    }
};

class Neg :public AST
{
public:
    AST *formula;

    Neg(AST *f): AST(this), formula(f) { }

    void eval() override {
        InterpData d;
        vector<InterpData> t = formula->interp;

        for(int j = 0; j<t.size(); j++) {
            d.expr = "~("+t[j].expr+")";
            d.value = !t[j].value;
            interp.push_back(d);
            cout<<"Neg exp: "<<d.expr<<" , val = "<<d.value<<endl;
        }
    }
};

class And :public AST
{
public:
    AST *left;
    AST *right;
    And(AST *l, AST *r): AST(this), left(l), right(r) { }

    void eval() override {
        InterpData d;

        vector<InterpData> l = left->interp;
        vector<InterpData> r = right->interp;

        for(int j = 0; j<l.size(); j++) {
            for(int k=0; k<r.size(); k++ ){
                d.expr = l[j].expr +"∧"+r[k].expr;
                d.value = l[j].value && r[k].value;
                cout<<"And exp: "<<d.expr<<" , val = "<<d.value<<endl;
                interp.push_back(d);
            }
        }
    }
};

class Or :public AST
{
public:
    AST *left;
    AST *right;
    Or(AST *l, AST *r): AST(this), left(l), right(r) { }

    void eval() override {
        InterpData d;

        vector<InterpData> l = left->interp;
        vector<InterpData> r = right->interp;

        for(int j = 0; j<l.size(); j++) {
            for(int k=0; k<r.size(); k++ ){
                d.expr = l[j].expr +"∨"+r[k].expr;
                d.value = l[j].value || r[k].value;
                cout<<"Or exp: "<<d.expr<<" , val = "<<d.value<<endl;
                interp.push_back(d);
            }
        }
    }
};

AST* pAtom(string s) 
{
    return new Atom(s);
}

AST* pAnd(AST* l, AST* r) 
{
    return new And(l,r);
}

AST* pOr(AST* l, AST* r) 
{
    return new Or(l,r);
}

AST* pNeg(AST* f) 
{
    return new Neg(f);
}

void getAtom(AST *f, vector<string> &s)
{
    if (dynamic_cast<Atom*>(f)) {
        s.push_back(dynamic_cast<Atom*>(f)->atom);
    } else if (dynamic_cast<Neg*>(f)) {
        getAtom(dynamic_cast<Neg*>(f)->formula, s);
    } else if (dynamic_cast<And*>(f)) {
        getAtom(dynamic_cast<And*>(f)->left,s);
        getAtom(dynamic_cast<And*>(f)->right,s);
    } else if (dynamic_cast<Or*>(f)) {
        getAtom(dynamic_cast<Or*>(f)->left,s);
        getAtom(dynamic_cast<Or*>(f)->right,s);
    }
}

AtomicProps* AllAtomProps(AST *f) 
{
    vector<string> s;
    getAtom(f, s);

    return new AtomicProps(s);
}

void getEval (AST *f) {

    if (dynamic_cast<Atom*>(f)) {
        dynamic_cast<Atom*>(f)->eval();
    } else if (dynamic_cast<Neg*>(f)) {
        getEval(dynamic_cast<Neg*>(f)->formula);
        dynamic_cast<Neg*>(f)->eval();
    } else if (dynamic_cast<And*>(f)) {
        getEval(dynamic_cast<And*>(f)->left);
        getEval(dynamic_cast<And*>(f)->right);
        dynamic_cast<And*>(f)->eval();

    } else if (dynamic_cast<Or*>(f)) {
        getEval(dynamic_cast<Or*>(f)->left);
        getEval(dynamic_cast<Or*>(f)->right);
        dynamic_cast<Or*>(f)->eval();
    }
} 

void valChk(AST *f)
{
    if (dynamic_cast<Atom*>(f)) {
        for(int j = 0; j < dynamic_cast<Atom*>(f)->interp.size();j++) {
            cout<<"atom i expr = "<< dynamic_cast<Atom*>(f)->interp[j].expr;
            cout<<", atom i val = "<< dynamic_cast<Atom*>(f)->interp[j].value<<endl;
        }
    } else if (dynamic_cast<Neg*>(f)) {
        for(int j = 0; j < dynamic_cast<Neg*>(f)->interp.size();j++) {
            cout<<"neg i expr = "<< dynamic_cast<Neg*>(f)->interp[j].expr;
            cout<<", neg i val = "<< dynamic_cast<Neg*>(f)->interp[j].value<<endl;
        }
    } else if (dynamic_cast<And*>(f)) {
        for(int j = 0; j < dynamic_cast<And*>(f)->interp.size();j++) {
            cout<<"and i expr = "<< dynamic_cast<And*>(f)->interp[j].expr;
            cout<<", and i val = "<< dynamic_cast<And*>(f)->interp[j].value<<endl;
        }
    } else if (dynamic_cast<Or*>(f)) {
        for(int j = 0; j < dynamic_cast<Or*>(f)->interp.size();j++) {
            cout<<"or i expr = "<< dynamic_cast<Or*>(f)->interp[j].expr;
            cout<<", or i val = "<< dynamic_cast<Or*>(f)->interp[j].value<<endl;
        }
    }
}


void eval(AST *f) {
    getEval(f);
    valChk(f);
}



int main() 
{
    // AST *f = pAnd(pAtom("q"),pAnd(pAtom("p"),pAtom("r")));   // AST 객체 생성
    AST *f = pNeg(pOr(pAtom("q"),pOr(pAtom("p"),pAtom("r"))));  
    AtomicProps *p =AllAtomProps(f);    // AST타입의 객체 f를 통해 atom 구하기
    cout<<"----------------------------"<<endl;

    // AtomicProps 요소 출력 확인
    for(int i=0; i < p->sets.size(); i++) {
        cout<<"p->sets["<<i<<"] : "<<p->sets[i]<<endl;
    }
    cout<<"----------------------------"<<endl;
    
    eval(f);


    return 0;
}