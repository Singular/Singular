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
list hne=develop(heme);
print(hne[1]);
kill r;
ring r=2,(x,y,t),ds;
poly f=y2+x7y+x9+x8;
param(develop(f));
kill r;
// ------------ test of reddevelop: -------------------
ring r=0,(x,y),dp;
reddevelop(x6-y4);
setring r;
kill HNEring;
list hne=reddevelop((y-x2+x3)*(y-x2-x3));
setring r;
kill HNEring;
reddevelop((x7-2x4y2+xy4-1y5)*(x7-4x4y2+4xy4-1y5));
kill HNEring;
//--------------- examples with change to ring extension
ring R=32003,(x,y),dp;
reddevelop(x6+y4);
kill HNEring;
ring F2=2,(x,y),dp;
reddevelop(y2+x7y+x9+x8);
kill HNEring;
//--------------- example with more than one ring change
ring F3=3,(x,y),dp;
list hne=reddevelop((x3-xy2+y3)*(x2+y2)*(x4-x3y+xy3+y4));
map T; int i;
poly f=(x3-xy2+y3)*(x2+y2)*(x4-x3y+xy3+y4);
for (i=1; i<=size(hne); i++) {
 T=basering,param(hne[i]);
 T(f);
}
kill HNEring;
// ------------ test of extdevelop: -------------------
setring r;
list hne=develop(x2+y3+y4-y5,-1);
extdevelop(hne,2);
extdevelop(hne,4);
extdevelop(hne,10);
kill hne;
// ------------ test of essdevelop:  ------------------
setring R;
essdevelop(x6+y4);
kill HNEring;
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
list hne=reddevelop((x2-y3)*(x2+y3));
intersection(hne[1],hne[2]);
example stripHNE;
puiseux2generators(intvec(3,7,15,31,63,127),intvec(2,2,2,2,2,2));
setring r;
multiplicities(develop(x5+y7));
example newtonpoly;
example is_irred;
is_irred((x2+y3)*(x2+y3+xy2));
example HNdevelop;
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
list L=list(poly(x-y),ideal(x,y));
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
example essdevelop;
example factorlist;
example develop;
example param;
example reddevelop;
example hnexpansion;
example sethnering;
example delta;
example is_NND;

tst_status(1);$
