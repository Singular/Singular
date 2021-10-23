LIB "tst.lib"; tst_init();
echo = 2;

LIB("syzextra.so");

option(redSB);option(redTail);

ring r=(0,q),(U,V,S,T),lp;

poly o = (-q4-5q3-2q2+10q-4)/(q)*ST2+(-q4-6q3-5q2+12q-4)*ST+(q5+5q4+2q3-10q2+4q)*T2+(q5+6q4+5q3-12q2+4q)*T;

poly O = o;

O;

simplify(O, 1 + 2 + 4 + 8);

leadcoef(o) / leadcoef(O);

o = o - lead(o);
O = O - lead(O);

leadcoef(o) / leadcoef(O);


o = o - lead(o);
O = O - lead(O);

leadcoef(o) / leadcoef(O);

o = o - lead(o);
O = O - lead(O);

leadcoef(o) / leadcoef(O);

o;
O;


tst_status(1);$

$$$


/*
matrix C[4][4];
C[1,2]=1/q;
C[1,3]=1;
C[1,4]=1;
C[2,3]=1;
C[2,4]=1;
C[3,4]=1/q;
def R=nc_algebra(C,0);
setring R;
R;

DetailedPrint(R);

*/


// characteristic : 0
// 1 parameter : q
// minpoly : 0
// number of vars : 4
// block 1 : ordering lp
// : names U V S T
// block 2 : ordering C
// noncommutative relations:
// VU=1/(q)*UV
// TS=1/(q)*ST
ideal I=U*V+S-V-1,U-V-V*T,U+V-V*S;

I;
// I[1]=UV-V+S-1
// I[2]=U-VT-V
// I[3]=U-VS+V

slimgb(I);// correct


option(mem);
option(prot);
option(teach);



$$


LIB "teachstd.lib";
printlevel = 0;
standard(I);

simplify(_, 1+2+4);

ideal II = _;
slimgb(II);


interred(II);



$



// std(ideal(II[7], II[1]));

std(ideal(II[7], II[3], II[9]));

$$$


std(I);
// G[1]=T
// G[2]=S-1
// G[3]=V
// G[4]=U



matrix M;
ideal H=liftstd(I,M);

// _[1]=T
// _[2]=S-1
// _[3]=V
// _[4]=U
H;
// H[1]=(q7-q6-3q5+5q4-2q3)*S+(-q7+q6+3q5-5q4+2q3)
// H[2]=(-q11+2q10+2q9-8q8+7q7-2q6)*V
// H[3]=(q6-3q4+2q3)*U
std(H);
// _[1]=S-1
// _[2]=V
// _[3]=U

M;
print(M);




kill r, R;

$$$$


ring r=(0,Q),(U,V,S,T),lp;
matrix C[4][4];
C[1,2]=Q;
C[1,3]=1;
C[1,4]=1;
C[2,3]=1;
C[2,4]=1;
C[3,4]=Q;
print(C);
def R=nc_algebra(C,0);
setring R;
R;
ideal I=U*V+S-V-1,U-V-V*T,U+V-V*S;
option(redSB);
option(redTail);
slimgb(I);// correct
std(I); // is correct as well!!!


// U, V, S-1


matrix Z;
liftstd(I,Z);


// _[1]=(2Q11-5Q10+3Q9+Q8-Q7)*S+(-2Q11+5Q10-3Q9-Q8+Q7)
// _[2]=(-2Q16+7Q15-8Q14+2Q13+2Q12-Q11)*V
// _[3]=(2Q7-3Q6+Q4)*U


simplify(_,1+2+4 );

$$

