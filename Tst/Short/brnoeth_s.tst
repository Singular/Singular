LIB "tst.lib";
tst_init();

LIB "brnoeth.lib";

list L=list();
L[1]="a";
L[2]="b";
L[3]="c";
L[4]="d";
intvec P=1,3,4,2;
permute_L(L,P);
kill L,P;

ring s=2,T,lp;
matrix G[3][7]=1,0,1,0,1,0,1,0,1,1,0,0,1,1,0,0,0,1,1,1,1;
print(G);
matrix H=dual_code(G);
print(H);
kill s;

ring s=3,T,lp;
matrix C[2][5]=0,1,0,1,1,0,1,0,0,1;
print(C);
list L=sys_code(C);
print(L[1]);
print(L[2]);
L[3];
print(L[1]*transpose(L[2]));
kill s;

ring s=2,(x,y),lp;
ideal I=x4+x,y4+y;
list L=closed_points(I);
L;
I=x8+x,y8+y;
L=closed_points(I);
L;
I=x16+x,y16+y;
L=closed_points(I);
L;
kill s;

ring r=2,(x,y),lp;
poly f;
ideal I;
intvec DD;
intvec G,D,F;

// KLEIN quartic over F_4 (p=2) 

f=x3y+y3+x;
list KLEIN=Adj_div(f);
KLEIN=NSplaces(1..5,KLEIN);
def R=KLEIN[1][2];
setring R;
G=4,4;
list LG=BrillNoether(G,KLEIN);
LG;
G=6,0,0,6;
LG=BrillNoether(G,KLEIN);
LG;
G=5,-1,3;
LG=BrillNoether(G,KLEIN);
LG;
list WS=Weierstrass(1,10,KLEIN);
WS[1];
WS[2];
KLEIN=extcurve(3,KLEIN);
KLEIN;
def RP=KLEIN[1][5];
setring RP;
size(POINTS);
def RR=KLEIN[1][4];
setring RR;
G=5,5,5;
D=4..24;
matrix CK=AGcode_Omega(G,D,KLEIN);
print(CK);
matrix HK=dual_code(CK);
print(HK);
list SK=sys_code(CK);
print(SK[1]);
print(SK[2]);
SK[3];
setring r;
kill R,RR,RP;

// HERMITE curve over F_4 (p=2) 

f=x3+y2+y;
list HC2=Adj_div(f);
HC2=NSplaces(1..5,HC2);
HC2=extcurve(2,HC2);
def RP=HC2[1][5];
setring RP;
size(POINTS);
def projR=HC2[1][2];
setring projR;
Weierstrass(1,12,HC2);
def ER2=HC2[1][4];
setring ER2;
kill projR,RP;
G=5;
D=2..9;
matrix C2=AGcode_Omega(G,D,HC2);
print(C2);
list SC2=sys_code(C2);
print(SC2[1]);
print(SC2[2]);
SC2[3];
matrix H2=dual_code(C2);
print(H2);
list SH2=sys_code(H2);
print(SH2[1]);
print(SH2[2]);
SH2[3];
F=2;
list SV2=prepSV(G,D,F,HC2);
matrix y[1][8];
y[1,3]=a;
print(decodeSV(y,SV2));
y[1,3]=0;
y[1,6]=1;
print(decodeSV(y,SV2));
killall();

// HERMITE curve over F_9 (p=3) 

ring r=3,(x,y),lp;
list HC3=Adj_div(y3+y-x4);
HC3=NSplaces(1..3,HC3);
HC3=extcurve(2,HC3);
def projR=HC3[1][2];
setring projR;
Weierstrass(1,15,HC3);
def RP=HC3[1][5];
setring RP;
size(POINTS);
def ER3=HC3[1][4];
setring ER3;
intvec G=15;
intvec D=2..28;
// we already have a rational divisor G and 27 more points over F_9;
// let us construct the corresponding residual AG code of type 
//     [27,14,>=11] over F_9
matrix C3=AGcode_Omega(G,D,HC3);
print(C3);
list SC3=sys_code(C3);
print(SC3[1]);
print(SC3[2]);
SC3[3];
matrix H3=dual_code(C3);
print(H3);
list SH3=sys_code(H3);
print(SH3[1]);
print(SH3[2]);
SH3[3];
// we can correct 3 errors and the genus is 3, thus F must have 
//    degree 6 and support disjoint to that of D : 
intvec F=6;
list SV3=prepSV(G,D,F,HC3);
// now we produce 3 errors on the zero-codeword :
matrix y[1][27];
y[1,1]=1;
y[1,2]=a;
y[1,3]=1;
print(decodeSV(y,SV3));
y[1,1]=0;
y[1,2]=a;
y[1,10]=a+1;
y[1,3]=0;
y[1,21]=1;
print(decodeSV(y,SV3));
y[1,4]=a;
y[1,2]=0;
y[1,12]=a;
y[1,10]=0;
y[1,21]=0;
y[1,26]=a+1;
print(decodeSV(y,SV3));

// killall();

// char 3 !

ring r=3,(x,y),dp;
list C=Adj_div(2x+x3+2x5+(1+x2+x4)*y4+x6y12);
C=NSplaces(1..2,C);
C=extcurve(2,C);

def SS1=C[1][5];
setring SS1;
POINTS;

setring r;
def SS2=C[5][2][1];
setring SS2;

def RR=C[1][2];
setring RR;
list WS=Weierstrass(17,6,C);
WS;

tst_status(1);$

