LIB "tst.lib";
tst_init();

// Testing ratFunc stuff
ring R=0,(x,y),dp;
ratFunc F=x2,xy;
F;
string(F);
ratFunc G=x2+y3;
G;
ratFunc H=1,x;
H;
ratFunc K=y;

G+H-F;
F*G;
F^3;
F/G;

F==H;
1/(K*F)==H;

kill F,G,H,K,R;


ring R=0,(x,y),dp;
Poly F=x2-xy+y3;
ratFunc G=x2+y,x+y;

ring S=0,(u,v,w),dp;
ratFunc r1=u,v;
ratFunc r2=w,u;

ratFunc H=PolyCompratFunc(F,r1,r2);
H=ratFuncCompratFunc(G,r1,r2);

kill R,F,G,H,S,r1,r2;



// Testing chart and chartmap stuff

ring R=0,(a,b,c,d),dp;
chart U=ad-bc;
U;
Ideal I=ideal(c,d);
ratFunc F=a,c;
ratFunc G=b,d;
list L=list(list(F),list(G));
ring S=0,(x),dp;
chart V=0;

U==U;
U!=V;

chartmap phi=U,V,I,L;
phi;

chartmap psi=U,V,I,F,G;
phi==psi;

psi=U,V,L;
phi!=psi;

psi=U,V,F,G;
phi==psi;

kill R,U,I,L,S,V,phi,psi,F,G;

ring R=0,(a,b,c,d),dp;
chart U=ideal(ad-bc,ac-bd);
Ideal Dom1=ideal(c,d);
ratFunc F1=a,c;
ratFunc F2=b,c;
ratFunc F3=1;
ratFunc G1=b,d;
ratFunc G2=a,d;
ratFunc G3=1;

ring S=0,(x,y,z),dp;
chart V=xy-z2;
chartmap psi=U,V,Dom1,F1,F2,F3,G1,G2,G3;
Ideal Dom2=ideal(y,z);
Ideal I=ideal(x,y);
ratFunc H1=z,y;
ratFunc H2=x,z;
ChartmapCompIdeal(psi,I);

ring T=0,(x),dp;
chart W=0;
chartmap phi=V,W,Dom2,H1,H2;

phi*psi;

kill R,S,T,I,Dom1,Dom2,U,V,W,F1,F2,F3,G1,G2,G3,H1,H2,phi,psi;



// Testing for schemes

ring R=0,(x,y),dp;
chart U=x2-y3;
scheme X=ChartToScheme(U);
X;
scheme Y=makeAffineScheme(x2-y3);
X==Y;

kill R,U,X,Y;

ring R=0,(x(1..3)),dp;
scheme X=makeProjScheme(ideal(x(1)^2-x(2)*x(3)));
ring S=0,(y(1..2)),dp;
scheme Y=makeProjScheme(ideal(y(1)-y(2)));

scheme Z=FibreProduct(X,Y);
Z;

kill R,S,X,Y,Z;

ring R=0,(x,y,z),dp;
scheme X=makeProjScheme(0);
scheme Y=makeProjScheme(ideal(x2-yz),X);
scheme Z=makeProjScheme(ideal(xy2-z3),X);

scheme S=intersection(Y,Z);
S=union(Y,Z);

kill R,X,Y,Z,S;

ring R=0,(x,y,z),dp;
scheme X=makeProjScheme(x2y3z4);
isReduced(X);

scheme S=ReducedScheme(X);
isReduced(S);

kill R,X,S;

ring R=0,(x,y,z),dp;
scheme X=makeAffineScheme(x-yz);
dimension(X);

X=makeProjScheme(x);
dimension(X);

X=makeProjScheme(ideal(x,y));
dimension(X);

X=makeProjScheme(ideal(x,y,z));
dimension(X);

kill R,X;

ring R=0,(x,y,z),dp;
scheme X=makeProjScheme(ideal(x2,xy));
list L=IrredDec(X);

X==union(L[1][1],L[2][1]);

kill R,X,L;


// Testing Divisor stuff

ring R=0,(x,y,z),dp;
scheme X=makeProjScheme(0);
scheme Y=makeProjScheme(ideal(x-z),X);
divisor A=list(Y,2),X;
Y=makeProjScheme(ideal(x2-zy),X);
scheme Z=makeProjScheme(ideal(x2z-z2y+y3),X);
divisor B=list(Y,1),list(Z,1),X;
Y=makeProjScheme(ideal(y),X);
divisor C=list(Y,1),X;
IntersectionNumber(3*C-A,B);

divisor D=A+B;
divisor E=3*C-B;
D+E;

kill R,X,Y,Z,A,B,C,D,E;



// Testing morphism stuff

ring R=0,(x,y,z),dp;
scheme X=makeProjScheme(xz-y2);
ring S=0,(a,b),dp;
scheme Y=makeProjScheme(0);

def T=X.cover[1].in;
setring T;
Ideal Dom=1;
ratFunc F=y;
chartmap f=X.cover[1],Y.cover[1],Dom,F;
list L=list(list(f,1,1));
kill T;

def T=X.cover[1].in;
setring T;
Dom=y;
F=1,y;
f=X.cover[1],Y.cover[2],Dom,F;
L=L+list(list(f,1,2));
kill T;

def T=X.cover[2].in;
setring T;
Dom=1;
F=z;
f=X.cover[2],Y.cover[1],Dom,F;
L=L+list(list(f,2,1));
kill T;

def T=X.cover[2].in;
setring T;
Dom=1;
F=x;
f=X.cover[2],Y.cover[2],Dom,F;
L=L+list(list(f,2,2));
kill T;

def T=X.cover[3].in;
setring T;
Dom=x;
F=y,x;
f=X.cover[3],Y.cover[1],Dom,F;
L=L+list(list(f,3,1));
kill T;

def T=X.cover[3].in;
setring T;
Dom=1;
F=y;
f=X.cover[3],Y.cover[2],Dom,F;
L=L+list(list(f,3,2));

morphism phi=X,Y,L;
setring S;
scheme Z=makeProjScheme(ideal(3a-2b),Y);
InvImage(Z,phi);
kill T;


def T=Y.cover[1].in;
setring T;
Dom=1;
F=b;
ratFunc G=b2;
f=Y.cover[1],X.cover[1],Dom,F,G;
L=list(list(f,1,1));
kill T;

def T=Y.cover[1].in;
setring T;
Dom=b;
F=1,b;
G=b;
f=Y.cover[1],X.cover[2],Dom,F,G;
L=L+list(list(f,1,2));
kill T;

def T=Y.cover[1].in;
setring T;
Dom=b;
F=1,b2;
G=1,b;
f=Y.cover[1],X.cover[3],Dom,F,G;
L=L+list(list(f,1,3));
kill T;

def T=Y.cover[2].in;
setring T;
Dom=a;
F=1,a;
G=1,a2;
f=Y.cover[2],X.cover[1],Dom,F,G;
L=L+list(list(f,2,1));
kill T;

def T=Y.cover[2].in;
setring T;
Dom=a;
F=a;
G=1,a;
f=Y.cover[2],X.cover[2],Dom,F,G;
L=L+list(list(f,2,2));
kill T;

def T=Y.cover[2].in;
setring T;
Dom=1;
F=a2;
G=a;
f=Y.cover[2],X.cover[3],Dom,F,G;
L=L+list(list(f,2,3));
kill Dom,F,G,f;

morphism psi=Y,X,L;

morphism id=psi*phi;
kill R,S,T,X,Y,Z,L,phi,psi,id;

ring R=0,(x,y),dp;
chart U=0;
Ideal Dom=1;
list L=list(makeratFunc(x),makeratFunc(y),makeratFunc(1));
ring S=0,(a,b,c),dp;
chart V=0;
chartmap phi=U,V,Dom,L;

morphism Phi=ChartmapToMorphism(phi);

kill R,Phi,phi,U,Dom,L,S,V;


// Testing Blow Up stuff

ring R=0,(x,y,z),dp;
scheme X=makeProjScheme(0);
scheme Y=makeProjScheme(ideal(y2z-x3-x2z),X);
scheme Z=makeProjScheme(ideal(x,y),X);
list L=blowUp(X,Z);
strictTransform(Y,L[2]);
BlowUpMapToMorphism(L[2]);

kill R,X,Y,Z,L;



// More testing of scheme stuff

ring R=0,(x,y,z),dp;
scheme X=makeProjScheme(0);
scheme Z=makeProjScheme(ideal(x-z,y-z)*ideal(x+z,y-z),X);
CompleteIntersectionCover(Z,X);

kill R,X,Z;


ring R=0,(x,y),dp;
scheme X=makeAffineScheme(ideal(y-x2));
HybrSmoothTest(X);

scheme Y=makeAffineScheme(ideal(y2-x3));
HybrSmoothTest(Y);

scheme C=makeAffineScheme(ideal(x,y),Y);
scheme Z=blowUp(Y,C)[1];
HybrSmoothTest(Z);

kill R,X,Y,C,Z;


ring R=0,(x,y,z),dp;
scheme X=makeProjScheme(x2y3z5);
isSmooth(X);

X=makeProjScheme(ideal(xyz-x3));
isSmooth(X);

X=makeAffineScheme(ideal(x2y-x));
isSmooth(X);

X=makeProjScheme(y2x-x3-x2z);
isSmooth(X);

scheme Z=makeProjScheme(ideal(x,y),X);
list L=blowUp(X,Z);
isSmooth(L[1]);

kill R,X,Z,L;


tst_status(1);$