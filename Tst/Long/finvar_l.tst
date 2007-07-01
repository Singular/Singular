// File: finvar_l.tst
// Long tests for finvar.lib

LIB "tst.lib";
tst_init();
LIB "finvar.lib";
option(redSB);


ring R1=0,(u,v,w,x,y,z),(L(255),lp);
matrix A[6][6];
A[1,2]=1;
A[2,3]=1;
A[3,1]=1;
A[4,5]=1;
A[5,6]=1;
A[6,4]=1;
matrix B[6][6];
B[1,2]=1;
B[2,1]=1;
B[3,3]=1;
B[4,5]=1;
B[5,4]=1;
B[6,6]=1;
list L=invariant_ring(A,B,intvec(0,0,1));
// the following is not unique!
L;
tst_status();
// the following is unique
groebner(ideal(L[1])+ideal(L[3]));
tst_status();
kill R1;

ring R3=2,(w,x,y,z),(L(255),lp);
matrix A[4][4]=0,1,0,0,-1,0,0,0,0,0,0,-1,0,0,1,0;
matrix B[4][4]=0,0,1,0,0,0,0,-1,-1,0,0,0,0,1,0,0;
list L=invariant_ring(A,B,intvec(0,0,1));
// the following is not unique!
L;
tst_status();
kill R3;

ring R4=11,(x,y),lp;
matrix A[2][2]=4^3,0,0,4^2;
number f=4+4^4-4^2-4^3;
matrix D[2][2]=4^4-4,4^2-4^3,4^2-4^3,4-4^4;
D=(1/f)*D;
list L1=invariant_ring(A,D,intvec(0,0,1));
// the following is not unique!
L1;
tst_status();
// it is a bit faster to work without Molien series:
list L2=invariant_ring(A,D,intvec(1,0,1));
// the following is not unique!
L2;
tst_status();
matrix TST4a = groebner(ideal(L1[1])+ideal(L1[3]));
matrix TST4b = groebner(ideal(L2[1])+ideal(L2[3]));
// the following is unique
TST4a;
TST4a==TST4b;
tst_status();
kill R4;

ring R5 = (0),(W1,W2,W3,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12),(L(255),dp(15),C);
matrix P[1][15] = W1+W2+W3,W1*W2+W1*W3+W2*W3,W1*W2*W3,V7+V10,V7*V10,V3+V6+V8+V9+V11+V12,V8*V9+V3*V11+V6*V12,V6*V8+V3*V9+V11*V12,V3*V8+V6*V9+V6*V11+V9*V11+V3*V12+V8*V12,V3*V6*V11+V3*V8*V11+V8*V9*V11+V3*V6*V12+V6*V9*V12+V8*V9*V12,V3^6+V6^6+V8^6+V9^6+V11^6+V12^6,V1,V2+V4+V5,V2*V4+V2*V5+V4*V5,V2*V4*V5;
matrix A1[15][15] = 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0;
matrix A2[15][15] = 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0;
matrix REY,M = reynolds_molien(A1,A2);
// the following is unique
REY;
M;
tst_status();
matrix IS1 = irred_secondary_char0(P,REY,M,1);
// the following is not unique!
IS1;
tst_status();
matrix IS2 = irred_secondary_no_molien(P,REY,1);
// the following is not unique!
IS2;
tst_status();
matrix TST5a = groebner(ideal(P)+ideal(IS1));
matrix TST5b = groebner(ideal(P)+ideal(IS2));
// the following is unique
TST5a;
TST5a==TST5b;
tst_status();
kill R5;

ring R6 = (0),(W1,W2,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11),(L(255),dp(13),C);
matrix A[13][13] = 0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0;
list L1 = invariant_ring(A,intvec(0,0,1));
// the following is not unique!
L1;
tst_status();
list L2 = invariant_ring(A,intvec(1,0,1));
// the following is not unique!
L2;
tst_status();
matrix TST6a = groebner(ideal(L1[1])+ideal(L1[3]));
matrix TST6b = groebner(ideal(L2[1])+ideal(L2[3]));
// the following is unique
TST6a;
TST6a==TST6b;
tst_status();
kill R6;

ring R7 = (0),(a,b,c,d,e,f),(L(255),dp(6),C);
matrix A1[6][6] = 
0,0,1,0,0,0,
1,0,0,0,0,0,
0,1,0,0,0,0,
0,0,0,0,1,0,
0,0,0,0,0,1,
0,0,0,1,0,0;
matrix A2[6][6] =
0,1,0,0,0,0,
0,0,0,1,0,0,
0,0,0,0,0,1,
1,0,0,0,0,0,
0,0,1,0,0,0,
0,0,0,0,1,0;
list L1 = invariant_ring(A1,A2,intvec(0,0,1));
// the following is not unique!
L1;
tst_status();
list L2 = invariant_ring(A1,A2,intvec(1,0,1));
// the following is not unique!
L2;
tst_status();
matrix TST7a = groebner(ideal(L1[1])+ideal(L1[3]));
matrix TST7b = groebner(ideal(L2[1])+ideal(L2[3]));
// the following is unique
TST7a;
TST7a==TST7b;
tst_status();
kill R7;

ring R8 = (0),(a,b,c,d,e,f),(L(255),dp(6),C);
matrix A1[6][6] = 
1,0,0,0,0,0,
0,0,0,1,0,0,
0,0,0,0,1,0,
0,1,0,0,0,0,
0,0,1,0,0,0,
0,0,0,0,0,1;
matrix A2[6][6] =
0,1,0,0,0,0,
0,0,0,1,0,0,
0,0,0,0,0,1,
1,0,0,0,0,0,
0,0,1,0,0,0,
0,0,0,0,1,0;
list L1 = invariant_ring(A1,A2,intvec(0,0,1));
// the following is not unique!
L1;
tst_status();
list L2 = invariant_ring(A1,A2,intvec(1,0,1));
// the following is not unique!
L2;
tst_status();
matrix TST8a = groebner(ideal(L1[1])+ideal(L1[3]));
matrix TST8b = groebner(ideal(L2[1])+ideal(L2[3]));
// the following is unique
TST8a;
TST8a==TST8b;
tst_status();
kill R8;

ring R9 = 0,(x(1..6)),(L(255),dp);
matrix A1 [6][6]= 
0,0,0,0,0,1,
0,0,0,0,1,0,
0,0,0,1,0,0,
0,0,1,0,0,0,
0,1,0,0,0,0,
1,0,0,0,0,0;
matrix A2[6][6]=
0,1,0,0,0,0,
0,0,1,0,0,0,
1,0,0,0,0,0,
0,0,0,0,1,0,
0,0,0,0,0,1,
0,0,0,1,0,0;
list L1 = invariant_ring(A1,A2,intvec(0,0,1));
// the following is not unique!
L1;
tst_status();
list L2 = invariant_ring(A1,A2,intvec(1,0,1));
// the following is not unique!
L2;
tst_status();
matrix TST9a = groebner(ideal(L1[1])+ideal(L1[3]));
matrix TST9b = groebner(ideal(L2[1])+ideal(L2[3]));
// the following is unique
TST9a;
TST9a==TST9b;
tst_status();
kill R9;

ring R10 = 0,(x(1..8)),(L(255),dp);
matrix A1 [8][8]= 
0,0,0,0,0,0,0,1,
0,0,0,0,0,0,1,0,
0,0,0,0,0,1,0,0,
0,0,0,0,1,0,0,0,
0,0,0,1,0,0,0,0,
0,0,1,0,0,0,0,0,
0,1,0,0,0,0,0,0,
1,0,0,0,0,0,0,0;
matrix A2[8][8]=
0,1,0,0,0,0,0,0,
0,0,1,0,0,0,0,0,
0,0,0,1,0,0,0,0,
1,0,0,0,0,0,0,0,
0,0,0,0,0,1,0,0,
0,0,0,0,0,0,1,0,
0,0,0,0,0,0,0,1,
0,0,0,0,1,0,0,0;
list L = invariant_ring(A1,A2,intvec(0,0,1));
// the following is not unique!
L;
tst_status();
matrix REY, M = reynolds_molien(A1,A2);
matrix IS1 = irred_secondary_char0(L[1],REY,M,1);
// the following is not unique!
IS1;
tst_status();
matrix IS2 = irred_secondary_no_molien(L[1],REY,1);
// the following is not unique!
IS2;
tst_status();
matrix TST10a = groebner(ideal(L[1])+ideal(L[3]));
matrix TST10b = groebner(ideal(L[1])+ideal(IS1));
matrix TST10c = groebner(ideal(L[1])+ideal(IS2));
// the following is unique
TST10a;
TST10a==TST10b;
TST10a==TST10c;
tst_status();
kill R10;

ring R11 = 0,(x(1..7)),(L(255),dp);
matrix A1[7][7] =
0,0,0,0,0,0,1,
1,0,0,0,0,0,0,
0,1,0,0,0,0,0,
0,0,1,0,0,0,0,
0,0,0,1,0,0,0,
0,0,0,0,1,0,0,
0,0,0,0,0,1,0;
matrix A2[7][7] =
1,0,0,0,0,0,0,
0,0,0,0,0,0,1,
0,0,0,0,0,1,0,
0,0,0,0,1,0,0,
0,0,0,1,0,0,0,
0,0,1,0,0,0,0,
0,1,0,0,0,0,0;
list L = invariant_ring(A1,A2,intvec(0,0,1));
// the following is not unique!
L;
tst_status();
matrix REY,M = reynolds_molien(A1,A2);
matrix IS1 = irred_secondary_char0(L[1],REY,M,1);
// the following is not unique!
IS1;
tst_status();
matrix IS2 = irred_secondary_no_molien(L[1],REY,1);
// the following is not unique!
IS2;
tst_status();
// these are the elementary symmetric polynomials, 
// that can be used as primary invariants in this example:
matrix P[1][7] = 
x(1)+x(2)+x(3)+x(4)+x(5)+x(6)+x(7),
x(1)*x(2)+x(1)*x(3)+x(1)*x(4)+x(2)*x(3)+x(2)*x(4)+x(3)*x(4)+x(1)*x(5)+x(2)*x(5)+x(3)*x(5)+x(4)*x(5)+x(1)*x(6)+x(2)*x(6)+x(3)*x(6)+x(4)*x(6)+x(5)*x(6)+x(1)*x(7)+x(2)*x(7)+x(3)*x(7)+x(4)*x(7)+x(5)*x(7)+x(6)*x(7),
x(1)*x(2)*x(3)+x(1)*x(2)*x(4)+x(1)*x(3)*x(4)+x(2)*x(3)*x(4)+x(1)*x(2)*x(5)+x(1)*x(3)*x(5)+x(1)*x(4)*x(5)+x(2)*x(3)*x(5)+x(2)*x(4)*x(5)+x(3)*x(4)*x(5)+x(1)*x(2)*x(6)+x(1)*x(3)*x(6)+x(1)*x(4)*x(6)+x(1)*x(5)*x(6)+x(2)*x(3)*x(6)+x(2)*x(4)*x(6)+x(2)*x(5)*x(6)+x(3)*x(4)*x(6)+x(3)*x(5)*x(6)+x(4)*x(5)*x(6)+x(1)*x(2)*x(7)+x(1)*x(3)*x(7)+x(1)*x(4)*x(7)+x(1)*x(5)*x(7)+x(1)*x(6)*x(7)+x(2)*x(3)*x(7)+x(2)*x(4)*x(7)+x(2)*x(5)*x(7)+x(2)*x(6)*x(7)+x(3)*x(4)*x(7)+x(3)*x(5)*x(7)+x(3)*x(6)*x(7)+x(4)*x(5)*x(7)+x(4)*x(6)*x(7)+x(5)*x(6)*x(7),
x(1)*x(2)*x(3)*x(4)+x(1)*x(2)*x(3)*x(5)+x(1)*x(2)*x(4)*x(5)+x(1)*x(3)*x(4)*x(5)+x(2)*x(3)*x(4)*x(5)+x(1)*x(2)*x(3)*x(6)+x(1)*x(2)*x(4)*x(6)+x(1)*x(2)*x(5)*x(6)+x(1)*x(3)*x(4)*x(6)+x(1)*x(3)*x(5)*x(6)+x(1)*x(4)*x(5)*x(6)+x(2)*x(3)*x(4)*x(6)+x(2)*x(3)*x(5)*x(6)+x(2)*x(4)*x(5)*x(6)+x(3)*x(4)*x(5)*x(6)+x(1)*x(2)*x(3)*x(7)+x(1)*x(2)*x(4)*x(7)+x(1)*x(2)*x(5)*x(7)+x(1)*x(2)*x(6)*x(7)+x(1)*x(3)*x(4)*x(7)+x(1)*x(3)*x(5)*x(7)+x(1)*x(3)*x(6)*x(7)+x(1)*x(4)*x(5)*x(7)+x(1)*x(4)*x(6)*x(7)+x(1)*x(5)*x(6)*x(7)+x(2)*x(3)*x(4)*x(7)+x(2)*x(3)*x(5)*x(7)+x(2)*x(3)*x(6)*x(7)+x(2)*x(4)*x(5)*x(7)+x(2)*x(4)*x(6)*x(7)+x(2)*x(5)*x(6)*x(7)+x(3)*x(4)*x(5)*x(7)+x(3)*x(4)*x(6)*x(7)+x(3)*x(5)*x(6)*x(7)+x(4)*x(5)*x(6)*x(7),
x(1)*x(2)*x(3)*x(4)*x(5)+x(1)*x(2)*x(3)*x(4)*x(6)+x(1)*x(2)*x(3)*x(5)*x(6)+x(1)*x(2)*x(4)*x(5)*x(6)+x(1)*x(3)*x(4)*x(5)*x(6)+x(2)*x(3)*x(4)*x(5)*x(6)+x(1)*x(2)*x(3)*x(4)*x(7)+x(1)*x(2)*x(3)*x(5)*x(7)+x(1)*x(2)*x(3)*x(6)*x(7)+x(1)*x(2)*x(4)*x(5)*x(7)+x(1)*x(2)*x(4)*x(6)*x(7)+x(1)*x(2)*x(5)*x(6)*x(7)+x(1)*x(3)*x(4)*x(5)*x(7)+x(1)*x(3)*x(4)*x(6)*x(7)+x(1)*x(3)*x(5)*x(6)*x(7)+x(1)*x(4)*x(5)*x(6)*x(7)+x(2)*x(3)*x(4)*x(5)*x(7)+x(2)*x(3)*x(4)*x(6)*x(7)+x(2)*x(3)*x(5)*x(6)*x(7)+x(2)*x(4)*x(5)*x(6)*x(7)+x(3)*x(4)*x(5)*x(6)*x(7),
x(1)*x(2)*x(3)*x(4)*x(5)*x(6)+x(1)*x(2)*x(3)*x(4)*x(5)*x(7)+x(1)*x(2)*x(3)*x(4)*x(6)*x(7)+x(1)*x(2)*x(3)*x(5)*x(6)*x(7)+x(1)*x(2)*x(4)*x(5)*x(6)*x(7)+x(1)*x(3)*x(4)*x(5)*x(6)*x(7)+x(2)*x(3)*x(4)*x(5)*x(6)*x(7),
x(1)*x(2)*x(3)*x(4)*x(5)*x(6)*x(7);
matrix IS3 = irred_secondary_char0(P,REY,M,1);
// the following is not unique!
IS3;
tst_status();
matrix IS4 = irred_secondary_no_molien(P,REY,1);
// the following is not unique!
IS4;
tst_status();
matrix TST11a = groebner(ideal(L[1])+ideal(L[3]));
matrix TST11b = groebner(ideal(L[1])+ideal(IS1));
matrix TST11c = groebner(ideal(L[1])+ideal(IS2));
matrix TST11d = groebner(ideal(P)+ideal(IS3));
matrix TST11e = groebner(ideal(P)+ideal(IS4));
// the following is unique
TST11a;
TST11a==TST11b;
TST11a==TST11c;
TST11a==TST11d;
TST11a==TST11e;
tst_status();
kill R11;

ring R12 = (0),(W1,W2,W3,V5,V3,V12,V11,V14,V9,V6,V4,V7,V2,V8,V10,V1,V13,V15),((L(255),dp)(18),C);
matrix P[1][18] = W1+W2+W3,W1*W2+W1*W3+W2*W3,W1*W2*W3,V5+V9+V8,V5*V9+V5*V8+V9*V8,V5*V9*V8,V1+V13+V15,V1*V13+V1*V15+V13*V15,V1*V13*V15,V12+V11+V6,V12*V11+V12*V6+V11*V6,V12*V11*V6,V3+V14+V4+V7+V2+V10,V3*V7+V14*V2+V4*V10,V14*V4+V7*V2+V3*V10,V3*V4+V14*V7+V3*V2+V4*V2+V14*V10+V7*V10,V3*V14*V7+V3*V4*V7+V3*V14*V2+V4*V7*V10+V14*V2*V10+V4*V2*V10,V3^6+V14^6+V4^6+V7^6+V2^6+V10^6;
matrix A1[18][18] = 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1;
matrix A2[18][18] = 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0;
matrix REY,M = reynolds_molien(A1,A2);
kill A1,A2;
matrix IS1 = irred_secondary_char0(P,REY,M,1);
// the following is not unique!
IS1;
tst_status();
matrix IS2 = irred_secondary_no_molien(P,REY,1);
kill REY;
// the following is not unique!
IS2;
tst_status();
matrix TST12a = groebner(ideal(P)+ideal(IS1));
matrix TST12b = groebner(ideal(P)+ideal(IS2));
// the following is unique
TST12a;
TST12a==TST12b;
tst_status();

ring R13 = (0),(W1,W2,W3,V1,V2,V3,V4,V5,V6,V7,V8,V9,V10,V11,V12,V13,V14,V15,V16,V17,V18,V19),(L(255),dp(22),C);
matrix P[1][22] = W1+W2+W3,W1*W2+W1*W3+W2*W3,W1*W2*W3,V6,V1,V2+V13+V18,V2*V13+V2*V18+V13*V18,V2*V13*V18,V3+V4+V7+V8+V10+V12,V7*V8+V3*V10+V4*V12,V4*V7+V3*V8+V10*V12,V3*V7+V4*V8+V4*V10+V8*V10+V3*V12+V7*V12,V3*V4*V10+V3*V7*V10+V7*V8*V10+V3*V4*V12+V4*V8*V12+V7*V8*V12,V3^6+V4^6+V7^6+V8^6+V10^6+V12^6,V11+V15,V11*V15,V5+V9+V14+V16+V17+V19,V5*V16+V9*V17+V14*V19,V9*V14+V16*V17+V5*V19,V5*V14+V9*V16+V5*V17+V14*V17+V9*V19+V16*V19,V5*V9*V16+V5*V14*V16+V5*V9*V17+V14*V16*V19+V9*V17*V19+V14*V17*V19,V5^6+V9^6+V14^6+V16^6+V17^6+V19^6;
matrix A1[22][22] = 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0;
matrix A2[22][22] = 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0;
matrix REY,M = reynolds_molien(A1,A2);
kill A1,A2;
matrix IS1 = irred_secondary_char0(P,REY,M,1);
// the following is not unique!
IS1;
tst_status();
matrix IS2 = irred_secondary_no_molien(P,REY,1);
// the following is not unique!
IS2;
kill REY;
tst_status();
matrix TST13a = groebner(ideal(P)+ideal(IS1));
matrix TST13b = groebner(ideal(P)+ideal(IS2));
// the following is unique
TST13a;
TST13a==TST13b;
tst_status();

ring R14 = 0,(x(1..9)),dp;
matrix Gperm = 
   invariant_algebra_perm(list(list(list(1,2,3,4,5,6,7,8,9))));
list L = group_reynolds(GetGroup(list(list(list(1,2,3,4,5,6,7,8,9)))));
matrix Grey  = 
   invariant_algebra_reynolds(L[1]);
// the following is not unique!
Gperm;
Grey;
// the following is unique!
matrix Tst14a = groebner(ideal(Gperm));
matrix Tst14b = groebner(ideal(Grey));
Tst14a;
Test14a==Tst14b;


tst_status(1);$
