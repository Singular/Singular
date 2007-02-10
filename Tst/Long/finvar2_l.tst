// File: finvar2_l.tst
// Long tests for finvar.lib

LIB "tst.lib";
tst_init();
LIB "finvar.lib";
option(redSB);


ring R2=29,(x,y,z),(L(255),lp);
number e=7;
number i=12;
number s=6;
number a=(e^5-e^2)/(i*s);
number b=(e^3-e^4)/(i*s);                          
number c=(e^6-e)/(i*s);                            
matrix A[3][3]=a,b,c,b,c,a,c,a,b;
matrix B[3][3]=e,0,0,0,e^4,0,0,0,e^2;
list L=primary_invariants(A,B,intvec(1,0,1));
// the following is not unique!
L;
tst_status();
matrix S, IS1 = secondary_and_irreducibles_no_molien(L[1..size(L)],1);
// the following is not unique!
S;
IS1;
tst_status();
kill S;
matrix IS2=irred_secondary_no_molien(L[1..size(L)],1);
// the following is not unique!
IS2;
tst_status();
list L2 = invariant_ring(A,B,intvec(0,0,1));
// the following is not unique!
L2;
tst_status();
list L3 = invariant_ring(A,B,intvec(1,0,1));
// the following is not unique!
L3;
tst_status();
matrix TST2a = groebner(ideal(L[1])+ideal(IS1));
matrix TST2b = groebner(ideal(L[1])+ideal(IS2));
kill L,IS1,IS2;
matrix TST2c = groebner(ideal(L2[1])+ideal(L2[3]));
matrix TST2d = groebner(ideal(L3[1])+ideal(L3[3]));
// the following is unique
TST2a;
TST2a==TST2b;
TST2a==TST2c;
TST2a==TST2d;
tst_status();
kill R2;

tst_status(1);$
