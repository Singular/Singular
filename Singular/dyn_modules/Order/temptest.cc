#include <libpolys/coeffs/bigintmat.h>
#include "temptest.h"
#include <reporter/reporter.h>
#include "libpolys/coeffs/numbers.h"
#include "libpolys/coeffs/coeffs.h"
#include "Singular/ipid.h"
//#include "kernel/febase.h"
#include "lattice.h"
#include <iostream>


#include "kernel/numeric/mpr_numeric.h"

using namespace std;

//Temporary testfunction to play arround with new functions
//NOTE: remove later
bigintmat* temp_test(bigintmat& a) {
    PrintS("This is a Test\n");
    bigintmat* bim = new bigintmat(&a);
    bim->Print();

    cout << "TEST " << getCoeffType(bim->basecoeffs()) << '\n';
//     lattice* l = new lattice(bim);
//     l->LLL();
//     delete l;
//     l->MLLL();
//     bigintmat n = l->get_lattice();
    bigintmat* c = bimAdd(bim,bim);
    delete bim;
    return c;
}

number temp_test2(number a) {
    PrintS("This is a Test 2\n");
    number b = nCopy(a);
    number c = nCopy(a);//squareroot(b,currRing->cf,10);
    coeffs coef = currRing->cf;//nInitChar(n_Q,NULL);

    //cout << "CoeffType of currRing: " << getCoeffType(currRing->cf) << '\n';
    //cout << "n_Greater(n_Init(3,coef),n_Init(3,coef),coef): " << n_Greater(n_Init(3,coef),n_Init(3,coef),coef) << '\n';
    //Test enumerate

    bigintmat *m = new bigintmat(3,3,coef);
    for(int i=1; i<=3; i++) {
        m->set(i,i,n_Init(1,coef), coef);
    }
    m->set(1,2,n_Init(1,coef), coef);
    m->set(2,3,n_Init(1,coef), coef);
    m->set(3,1,n_Init(1,coef), coef);

    m->Print();
    PrintS("\n");

    lattice* l = new lattice(m);
    //l->LLL();
    bigintmat* enumer = NULL;// = new bigintmat(3,,coef);
    enumer = l->enumerate_all(c);
    //bigintmat *reduced = l->get_reduced_basis();
    //reduced->Print();
    if(enumer !=NULL){
        enumer->transpose()->Print();
        PrintS("\n");
    }/*
    PrintS("new number\n");
    enumer = l->enumerate_next( c);
    if(enumer !=NULL){
        enumer->Print();
        PrintS("\n");
    }
    PrintS("new number and bigintmat\n");
    bigintmat* x= new bigintmat(3,1,coef);
    x->set(2,1,n_Init(-4,coef), coef);
    x->set(3,1,n_Init(1,coef), coef);
    enumer = l->enumerate_next( c, x);
    if(enumer !=NULL){
        enumer->Print();
        PrintS("\n");
    }
    for(int i=1;i<4;i++){
        enumer = l->enumerate_next();
        if(enumer !=NULL){
            enumer->Print();
        }
        PrintS("\n");
    }
    PrintS("new bigintmat\n");
    x->set(3,1,n_Init(8,coef), coef);
    enumer = l->enumerate_next(x);
    if(enumer !=NULL){
        enumer->Print();
    }
    PrintS("til now it works\n");
    delete l;
    //*/

    /*//test minkowski
    PrintS("Test Minkowski\n");PrintS("Elements\n");
    bigintmat ** elementarray = new bigintmat*[4];
    elementarray[0] = new bigintmat(4,1,coef);
    elementarray[1] = new bigintmat(4,1,coef);
    elementarray[2] = new bigintmat(4,1,coef);
    elementarray[3] = new bigintmat(4,1,coef);
    elementarray[0]->rawset(2,1,n_Init(1,coef),coef);

    elementarray[1]->rawset(1,1,n_Init(3,coef),coef);
    elementarray[1]->rawset(2,1,n_Init(7,coef),coef);
    elementarray[1]->rawset(3,1,n_Init(13,coef),coef);

    elementarray[2]->rawset(1,1,n_Init(-1,coef),coef);
    elementarray[2]->rawset(2,1,n_Init(42,coef),coef);
    elementarray[2]->rawset(3,1,n_Init(-5,coef),coef);
    elementarray[2]->rawset(4,1,n_Init(21,coef),coef);

    elementarray[3]->rawset(1,1,n_Init(2,coef),coef);
    elementarray[3]->rawset(2,1,n_Init(-1,coef),coef);
    elementarray[3]->rawset(3,1,n_Init(-5,coef),coef);
    elementarray[3]->rawset(4,1,n_Init(1,coef),coef);
    /// poly with real and imag roots
    PrintS("polynomial\n");
    number * poly = new number[5];//(number *)omAlloc( (5) * sizeof( number ) );//new number[5];
    //poly[0] = n_Init(6,coef);
    //poly[1] = n_Init(0,coef);
    //poly[2] = n_Init(5,coef);//positiv imagin??r, negatic reelle wurzeln
    //poly[3] = n_Init(0,coef);
    //poly[4] = n_Init(1,coef);
    poly[0] = n_Init(-1,coef);
    poly[1] = n_Init(0,coef);
    poly[2] = n_Init(0,coef);
    poly[3] = n_Init(3,coef);
    poly[4] = n_Init(1,coef);
    int prec = 42;
    //coeffs rea = nInitChar(n_long_R,NULL);
    //setGMPFloatDigits( prec, prec);
    //number abc = n_Init(1,rea);
    //abc = n_Div(abc,n_Init(3333,rea),rea);
    //n_Print(abc,rea);
    //PrintS("\n");
    bigintmat * gitter = NULL;PrintS("Call function\n");
    gitter = minkowksi(elementarray,4,poly,4,coef,prec);
    if(gitter !=NULL){
        gitter->Print();PrintS("\n");
        //cout << "CoeffType of gitter: " << getCoeffType(gitter->basecoeffs()) << '\n';
        coeffs rea = gitter->basecoeffs();
        number sqrt = squareroot(n_Init(100,rea),rea,prec);
        n_Print(sqrt,rea);
        PrintS("\n");
    }
    //*/
    //n_Print(c,currRing->cf);
    PrintS("\n");

    return b;
}

