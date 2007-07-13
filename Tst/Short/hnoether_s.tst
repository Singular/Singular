// File: hnoether_s.tst
// Short tests for hnoether lib
LIB "tst.lib";
tst_init();
LIB "hnoether.lib";
// ------------ test of develop: -------------------
ring r=0,(x,y),ds;
develop(x2+y2);
develop(x2+y+y2);
develop((x2+y3)*(x2+y3+xy2));
poly p_1 = y2+x3;
poly p_2 = p_1^2 + x5y;
poly p_3 = p_2^2 + x^10 *p_1;
param(develop(p_1));
param(develop(p_3));
develop(p_3,-1);
develop(p_3,4);
poly heme=xy8+y8+x4y6+4x3y6+2x5y5+6x6y4+4x8y3+x10y2+4x9y2+2x11y+x12;
list Hne=develop(heme);
print(Hne[1]);
kill r;
ring r=2,(x,y,t),ds;
poly f=y2+x7y+x9+x8;
param(develop(f));
kill r;
// ------------ test of hnexpansion: -------------------
ring r=0,(x,y),dp;
hnexpansion(x6-y4);
hnexpansion((y-x2+x3)*(y-x2-x3));
hnexpansion((x7-2x4y2+xy4-1y5)*(x7-4x4y2+4xy4-1y5));

ring F2=2,(x,y,t),ds;
def L=hnexpansion(y2+x7y+x9+x8);
param(L);
kill L,F2;

//--------------- examples with change to ring extension
ring R=32003,(x,y),dp;
def L=hnexpansion(x6+y4);
def HNring = L[1]; setring HNring;  displayHNE(hne);
kill L,HNring;

//--------------- example with more than one ring change
ring F3=3,(x,y),dp;
poly f=(x3-xy2+y3)*(x2+y2)*(x4-x3y+xy3+y4);
list L=hnexpansion(f);
def HNring = L[1]; setring HNring;  displayHNE(hne);
map T; int i;
for (i=1; i<=size(hne); i++)
{
 T=basering,param(hne[i]);
 T(f);
}
kill hne,HNring,F3,L;

// ------------ test of extdevelop: -------------------
setring r;
list Hne=develop(x2+y3+y4-y5,-1);
extdevelop(Hne,2);
extdevelop(Hne,4);
extdevelop(Hne,10);
kill Hne;
// ------------ test of essdevelop:  ------------------
setring R;
list L=essdevelop(x6+y4);
L[2];  // number of conjugated branches
def HNring = L[1]; setring HNring;  displayHNE(hne);
kill hne,L,HNring;
setring r;
// ------------ test of param: ------------------------
param(develop(x2+y3));
param(develop(y2+x3+x4));
param(develop(y2+x3+x4),0);
// ------------ test of displayHNE: -------------------
example displayHNE;
// ----------------------------------------------------------------------------
// ------- test of invariants, displayInvariants, intersection, stripHNE,   ---
// ---- puiseux2generators, multiplicities, newtonpoly, is_irred, HNdevelop ---
example invariants;
example displayInvariants;
list L=hnexpansion((x2-y3)*(x2+y3));
intersection(L[1],L[2]);
kill L;
example stripHNE;
puiseux2generators(intvec(3,7,15,31,63,127),intvec(2,2,2,2,2,2));
setring r;
multsequence(develop(x5+y7));
example newtonpoly;
example is_irred;
is_irred((x2+y3)*(x2+y3+xy2));
// ------- test of getnm, T_Transform, T1_Transform, T2_Transform, koeff, -----
// ------------  redleit, squarefree, allsquarefree, referencepoly   ----------
example getnm;
T_Transform(y2+x3,1,2);
T1_Transform(y-x2+x3,1,2);
T2_Transform(y2+x3,-1,3,2,y2+x3);
example referencepoly;
koeff(x2+2xy+3xy2-x2y-2y3,1,2);
example redleit;
example squarefree;
example allsquarefree;

// --------------- additions: -----------------------------
example further_hn_proc;
example leit;
example testreducible;
example T_Transform;
example T1_Transform;
example T2_Transform;
example koeff;
example multiplicities;
example puiseux2generators;
example intersection;
example multsequence;
example displayMultsequence;
example separateHNE;
example charPoly;
example find_in_list;
example get_last_divisor;
example extdevelop;
example factorfirst;
example factorlist;
example develop;
example param;
example hnexpansion;
example delta;
example is_NND;
tst_status(1);$
