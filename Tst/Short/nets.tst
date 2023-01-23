LIB "tst.lib";
tst_init();

LIB "nets.lib";


ring r1=101,(x,y,z),lp;
list L=111, x+y, x3y-z, y+3z4;
print("list L=111, x+y, x3y-z, y+3z4");
netList(L);
kill r1;

ring r1=101,(x,y,z),lp;
number a= 5;
netNumber(a);
kill r1;

ring r=0,x,lp;
netCoefficientRing(r);
kill r;


ring r=0,x,lp;
int n=2;
int m=5;
netPrimePower(n,m);
kill r;


ring r = 32003,(x,y,z),dp;
netRing(r);
kill r;

ring r = 32003,(x(1..10)),dp;
netRing(r);
kill r;

ring r = 32003,(x(1..5)(1..8)),dp;
netRing(r);
kill r;

ring r = 0,(a,b,c,d),lp;
netRing(r);

kill r;
ring r = 7,(x,y,z),ds;
netRing(r);


kill r;
ring r = 7,(x(1..6)),(lp(3),dp);
netRing(r);

kill r;
ring r = 0,(x,y,z,a,b,c),(ds(3), dp(3));
netRing(r);

kill r;
ring r = 0,(x,y,z),(c,wp(2,1,3));
netRing(r);

kill r;
ring r = (7,a,b,c),(x,y,z),Dp;
netRing(r);

kill r;
ring r = (7,a),(x,y,z),dp;
minpoly = a^2+a+3;
netRing(r);

kill r;
ring r = (7^2,a),(x,y,z),dp;
netRing(r);

kill r;
ring r = real,(x,y,z),dp;
netRing(r);

kill r;
ring r = (real,50),(x,y,z),dp;
netRing(r);

kill r;
ring r = (real,10,50),(x,y,z),dp;
netRing(r);

kill r;
ring r = (complex,30,j),(x,y,z),dp;
netRing(r);

kill r;
ring r = complex,(x,y,z),dp;
netRing(r);

kill r;
ring R = 7,(x,y,z), dp;
qring r = maxideal(2);
netRing(r);


kill r;
ring r1=101,(x,y,z),lp;
ring r2=103,(a,b,c),lp;
map f=r1,ab,ac,bc;
netMap(f);
kill r1,r2;


int a=111111;
int b=22222;
int c=3333;
int d=444;
bigintmat M[2][2]=a,b,c,d;
netBigIntMat(M);
kill a,b,c,d,M;

ring r;
bigint a=1111111111111111;
bigint b=2222222222222222;
bigint c=3333333333333333;
bigint d=44;
bigintmat M[2][2]=a,b,c,d;
netBigIntMatShort(M);
kill a,b,c,d,M;


int a=111111;
int b=222222;
int c=333333;
int d=444444;
intmat M[2][2]=a,b,c,d;
netIntMat(M);
kill a,b,c,d,M;


int a=111111;
int b=222222;
int c=333333;
int d=444444;
intvec V=a,b,c,d;
netIntVector(V);
kill a,b,c,d,V;

int a=111111;
int b=222222;
int c=333333;
int d=444444;
intvec V=a,b,c,d;
netIntVectorShort(V,4);
kill a,b,c,d,V;


ring r1=101,(x,y,z),lp;
poly a=2x3y4+300xy-234z23;
poly b=2x3y4z;
poly c=x3y4z5;
poly d=5x6y7z10;
poly e=2x3y;
poly f=4y5z8;
matrix M[2][3]=a,b,c,d,e,f;
print(M);
netmatrix(M);
kill r1;

ring r1=101,(x,y,z),lp;
poly a=2x3y4+300xy-234z23+z25+z26+z27+z28+z29+z30+z^30000+z^30001+z^30002+z^30003;
poly b=2x3y4z;
poly c=x3y4z5;
poly d=5x6y7z10;
poly e=2x3y-2x3y4+300xy-234z23;
poly f=4y5z8;
matrix M[2][3]=a,b,c,d,e,f;
netmatrixShort(M, 10);
kill r1;

setring r;

int j=5;
netInt(j);


bigint M=5;
netBigInt(M);
kill M;

string M="Hallo";
netString(M);
kill M;



ring r1=101,(x,y,z),lp;
poly a=2x3y4;
poly b=2x3y4z;
poly c=x3y4z5;
poly d=5x6y7z10;
poly e=2x3y;
poly f=4y5z8;
vector V=[a,b,c,d,e,f];
netvector(V);
kill r1;


ring r1=101,(x,y,z),lp;
poly a=2x3y4;
poly b=2x3y4z-5x6y7z10;
poly c=x3y4z5;
poly d=5x6y7z10;
poly e=2x3y;
poly f=4y5z8+5x6y7z10;
vector V=[a,b,c,d,e,f];
netvectorShort(V);
kill r1;



ring r=32003,(x,y,z),lp;
poly p=x4+4y4+4z4-x3-3y3-3z3+1x2+2y2+z2-x-1y-z1;
p;
netPoly(p);
kill p;

poly p2=x3yz+xy3z+xyz3-2x2yz-2xy2z-2xyz2+1xyz+x1yzxy1z;
p2;
netPoly(p2);

poly p=x+y+z-x2-3y-4z4+xy+xz+2xy-x2y-xz2-y2z2;
p;
netPoly(p);
kill r;


ring r=32003,(x(1..10)),lp;
poly p=x(1)*x(2)*x(3)+2*x(1)^2+2*x(1)*x(2);
p;
netPoly(p);
kill p;

poly p=x(1)^2*x(2)^3*x(3)^4-2*x(1)^1*x(2)^2+2*x(1)*x(2)*x(10);
p;
netPoly(p);
kill r;

ring r=7,(x,y,z),lp;
poly p=17x2+24y;
p;
netPoly(p);
kill r;

ring r=(7,a,b,c),(x,y,z),Dp;
poly p=2ax2+by-cz3;
p;
netPoly(p);
kill r;

ring r=(7,a),(x,y,z),dp;
minpoly = a^2+a+3;
poly p=2ax2+y-az3;
p;
netPoly(p);
kill r;

ring r = (complex,30,j),(x,y,z),dp;
poly p=2x2+y-z3+20*j;
p;
netPoly(p);
kill r;



ring r;
ideal I=2x3y4,2x3y4z+x3y4z5,5x6y7z10-2x3y+4y5z8;
netIdeal(I);

tst_status(1);$


