LIB "tst.lib";
tst_init();

LIB "modules.lib";

 ring r;
 int n=4;
 id(n);
 kill r;kill n;

 ring r;
 int n=4;
 int m=3;
 zero(n,m);
 kill r;kill m; kill n;

 ring r;
 Deg(x);
 list l=2,2,2;
 SetDeg(l);
 Deg(x);
 kill r;kill l;

 ring r;
 matrix m[1][3]=x,y,z;
 Matrix M=m;
 Module N=coker(M);
 N;
 Resolution R = Res(N);
 R;
 kill r;kill M;kill N;kill R;

 ring r;
 int n=3;
 list l=1,2,3;
 freeModule(r,n,l);
 kill r;kill n;kill l;


 ring r;
 ideal i=x,y,z+x;
 i;
 Ideal I=i;
 I;
 kill r;kill i;kill I;

 ring r;
 matrix m[1][3]=x,y,z;
 Matrix M=m;
 Module N=coker(M);
 N;
 Resolution R = mRes(N);
 R;
 R.dd[2];
 kill r;kill M;kill N;kill R;

 ring r;
 matrix m[1][3]=x,y,z;
 Matrix M=m;
 Module N=coker(M);
 N;
 Resolution R = sRes(N);
 R;
 R.dd[2];
 kill r;kill M;kill N;kill R;

 ring r;
 matrix m[1][3]=x,y,z;
 Matrix M=m;
 Module N=coker(M);
 N;
 Resolution R = Res(N);
 R;
 R.dd[2];
 kill r;kill M;kill N;kill R;

 ring r;
 matrix m[1][3]=x,y,z;
 Matrix M=m;
 Module N=coker(M);
 Resolution R=mRes(N);
 R;
 Betti(R);
 kill r;kill M;kill N;kill R;


 ring r;
 matrix m[2][2]=x,y3,z,xz;
 Matrix M=m;
 M;
 Target(M);
 kill r;kill M;kill m;

 ring r;
 matrix m[2][2]=x,y3,z,xz;
 Matrix M=m;
 M;
 Source(M);
 kill r;kill M;kill m;

 ring r;
 matrix m[2][2]=x,y3,z,xz;
 Matrix Ma=m;
 FreeModule M=Source(Ma);
 M;
 Degree(M);
 kill r;kill M;kill m;



 ring r;
 matrix m[2][2]=x,y2,z,xz;
 Matrix M=m;
 matrix n[2][3]=z2,xyz,x2y2,xy,x3,y4;
 Matrix N=n;
 M;
 N;
 subquotient(M,N);
 kill r;kill m;kill M;kill n;kill N;

 ring R=0,(x,y),(lp,c);
 Matrix M=id(2);
 Module src=image(M);
 matrix rules[2][2]=x,y,x2,y2;
 Module tar=coker(M);
 src;
 tar;
 rules;
 homomorphism(rules,src,tar);
 kill R,M,src,rules,tar;


 ring r;
 matrix m[2][2]=x,y2,z,xz;
 Matrix M=m;
 M;
 coker(M);
 kill r,m,M;

 ring r;
 matrix m[2][2]=x,y2,z,xz;
 Matrix M=m;
 M;
 image(M);
 kill r,m,M;


 ring r;
 matrix m[1][3]=x,y2,z3;
 Matrix M=m;
 M;
 Ker(M);
 kill r,M,m;


 ring R=0,(x,y),(lp,c);
 Matrix M=id(2);
 Module src=image(M);
 matrix rules[2][2]=x,y,x2,y2;
 Module tar=coker(M);
 src;
 tar;
 rules;
 Homomorphism f=homomorphism(rules,src,tar);
 f;
 target(f);
 source(f);
 kill R,M,src,rules,tar,f;

 ring r;
 matrix ma[2][2]=x,y,x2,y2;
 Matrix m=ma;
 Module M=image(m);
 matrix na[2][2]=xy,x2,y2,x;
 Matrix n=na;
 Module N=image(na);
 M;
 N;
 N+M;
 kill r,ma,m,M,na,n,N;

 ring r;
 matrix ma[2][2]=x,y,x,y;
 Matrix m=ma;
 Module M=image(m);
 matrix na[2][1]=-y,x;
 Matrix n=na;
 M;
 Module N=image(n);
 N;
 N==M;
 N=coker(n);
 N;
 N==M;
 kill r,ma,m,M,na,n,N;

 ring r;
 matrix ma[2][2]=x,y,x,y;
 Matrix m=ma;
 Module M=image(m);
 M;
 Degrees(M);
 kill ma,m,M;


 ring r;
 matrix ma[2][2]=x,y,x,y;
 Matrix M=ma;
 matrix na[2][1]=-y,x;
 Matrix N=na;
 M;
 N;
 N==M;
 M==M;
 kill r,ma,M,na,N;

 ring r;
 matrix m[2][2]=x,y2,z,xz;
 Matrix M=m;
 M;
 Module N=image(M);
 N;
 kill r,m,M,N;


 ring r;
 list L = 1,1,1;
 FreeModule F = freeModule(r,3,L);
 freeModule2Module(F);
 kill r,L,F;

 ring r;
 matrix m[2][2]=x,y2,z,xz;
 Matrix M=m;
 netMatrix(M);
 kill r,m,M;

 ring r;
 Module M = image(id(3));
 makeVector([x,y,z],M);
 kill r,M;

 ring r;
 matrix m[2][1] = x,-y;
 Module M = subquotient(id(2),m);
 Vector V = [x,y],M;
 Vector W = [0,2y],M;
 Vector U = [x,y2],M;
 compareVectors(V,W);
 compareVectors(U,V);
 kill r,m,M,V,W,U;


 ring R = 0,(x,y),dp;
 matrix a[1][2] = x,y;
 Matrix A = a;
 matrix b[1][2] = x2,y2;
 Matrix B = b;
 Module M = subquotient(A,B);
 presentation(M);
 kill R,a,A,b,B,M;


ring r;
matrix m[2][2]=x,y2,z,xz;
matrix n[2][2]=1,2,3,4;
Matrix M = m;
Matrix N = n;
tensorMatrix(M,N);
kill r,m,n,M,N;


ring R = 0,(x,y,z),dp;
matrix a[1][2] = x,y;
Matrix A = a;
matrix b[1][2] = x2,y2;
Matrix B = b;
Module M = subquotient(A,B);
M;
matrix c[2][2]=x,y2,z,xz;
Matrix C=c;
matrix d[2][3]=z2,xyz,x2y2,xy,x3,y4;
Matrix D=d;
Module N = subquotient(C,D);
N;
tensorModule(M,N);
kill R,a,A,b,B,M,c,C,d,D,N;


ring R = 0,(x,y,z),dp;
matrix a[1][2] = x,y;
Matrix A = a;
matrix b[1][2] = x2,y2;
Matrix B = b;
Module M = subquotient(A,B);
M;
FreeModule F = freeModule(R,3,0);
F;
tensorModFreemod(M,F);
kill R,a,A,b,B,M,F;


ring R = 0,(x,y,z),dp;
FreeModule F = freeModule(R,3,0);
F;
tensorFreeModule(F,F);
kill R,F;


ring R = 0,(x,y,z),dp;
matrix a[2][3] = -x,-y^2,x^3,y,x,0;
matrix b[1][2] = x^2-y^3,xy;
Matrix A = a;
Matrix B = b;
Module M = coker(A);
Module N = coker(B);
Module H = hom(M,N);
H;
kill R,a,b,A,B,M,N,H;


ring R = 0,(x,y),dp;
matrix a[1][2] = x,y;
Matrix A = a;
matrix b[1][2] = x2,y2;
Matrix B = b;
Module M = subquotient(A,B);
Module C = presentation(M);
Vector V = [x2,y4],C;
interpret(V);
kill R,a,A,b,B,M,C,V;


ring S;
matrix gens[2][3] = x2+y-3z4,y+xy,xyz+4,3+z2x,z3-3x+3,2+x+y+z7;
vector v = 2x*[gens[1..2,1]] + (y-z2)*[gens[1..2,2]] + 5*[gens[1..2,3]];
Matrix Gens = gens;
Module M = subquotient(Gens,zero(2,3));
M.interpretation = list(list(1,1,1),list());
Vector V = v,M;
interpret(V);
kill S,gens,v,Gens,M,V;



ring R;
matrix gens[2][3] = x2,xy,4,z2x,3x+3,z;
vector v = 2x*[gens[1..2,1]] + (y-z2)*[gens[1..2,2]] + 5*[gens[1..2,3]];
Matrix Gens = gens;
Module S = subquotient(Gens,zero(2,3));
Module N = coker(id(3));
matrix E = N.generators.hom;
Vector E1 = [1,0,0],N;
Vector E2 = [0,1,0],N;
Vector E3 = [0,0,1],N;
S.interpretation = list(list(E1,E2,E3),list([gens[1..2,1]],[gens[1..2,2]],[gens[1..2,3]]));
Vector V = v,S;
Vector W = interpret(V),N;
V;
Vector Vnew = interpretInv(W,S);
Vnew;
V==Vnew;
kill R,gens,v,Gens,S,N,E,E1,E2,E3,V,W,Vnew;


ring R = 0,(x,y,z),dp;
matrix a[2][3] = -x,-y^2,x^3,y,x,0;
matrix b[1][2] = x^2-y^3,xy;
Matrix A = a;
Matrix B = b;
Module M = coker(A);
Module N = coker(B);
hom(M,N);
kill R,a,b,A,B,M,N;


ring R=0,(x,y),(lp,c);
Matrix M=id(2);
Module src=image(M);
matrix rules[2][2]=x,y,xy,y2;
Module tar=coker(M);
Homomorphism f=homomorphism(rules,src,tar);
f;
kerHom(f);
kill R,M,src,rules,tar,f;


tst_status(1);$


