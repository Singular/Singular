// File: finvar_l.tst
// Long tests for finvar.lib

LIB "tst.lib";
tst_init();
LIB "finvar.lib";

ring R1=0,(u,v,w,x,y,z),dp;
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
L;

ring R2=29,(x,y,z),dp;
number e=7;
number i=12;
number s=6;
number a=(e^5-e^2)/(i*s);
number b=(e^3-e^4)/(i*s);                          
number c=(e^6-e)/(i*s);                            
matrix A[3][3]=a,b,c,b,c,a,c,a,b;
matrix B[3][3]=e,0,0,0,e^4,0,0,0,e^2;
list L=primary_invariants(A,B,intvec(1,0,1));
L;
matrix S=secondary_no_molien(L[1..size(L)],1);
S;

ring R3=2,(w,x,y,z),dp;
matrix A[4][4]=0,1,0,0,-1,0,0,0,0,0,0,-1,0,0,1,0;
matrix B[4][4]=0,0,1,0,0,0,0,-1,-1,0,0,0,0,1,0,0;
list L=invariant_ring(A,B,intvec(0,0,1));
L;

ring R4=11,(x,y),dp;
matrix A[2][2]=4^3,0,0,4^2;
number f=4+4^4-4^2-4^3;
matrix D[2][2]=4^4-4,4^2-4^3,4^2-4^3,4-4^4;
D=(1/f)*D;
list L=invariant_ring(A,D,intvec(0,0,1));
L;

$
