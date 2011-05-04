//-------------------------------------------------
//an error of 0.8.5b: donati@hera.unice.fr
ring r = 0, (x,y), ds;
poly f = x4+y5+x2y3;
ideal i = jacob(f);
ideal j = std(i);
j;
//j[1]=2x3+1xy3
//j[2]=3x2y2+5y4
//j[3]=10xy4
//j[4]=1y6

reduce(x4+y5+x2y3, j);
//-----------------------------------------------
//an error of 0.8.6: (ralf) div by zero -> abort
random(5,5);
//------------------------------------------------
//an error of 0.8.6: (gerik) result was false:
f=0;
f==0;
//------------------------------------------------
//an error of 0.8.7: (hgrass) core dump
matrix a[2][2]=1,2,3,4;
matrix b[1][0];
b; // could not display strange matrices
a=b; //could not assign strange matrices
listvar(all);
//-----------------------------------------------
//an error of 0.8.7d (hannes) ordering a incorrect
ring hs=0,(x,y),(c,a(5,3),dp);
hs;
poly f=x2+y3;
f;
ord(f);
deg(f);
//----------------------------------------------
//an error of 0.8.7d (gmg) jump to undefined address
ring r=0,(x,y),a(2,3);
poly f=x,y;
f;
//---------------------------------------------
//an error of 0.8.8c (thomas) (last entry missing)
ring r;
ideal i1=x,0,y;
ideal i2;
i1+i2;
kill r;
//---------------------------------------------
//an error of 0.8.8c (greuel) result was 0y
ring r0=0,(x,y),dp;
poly f=100x+101y;
subst(f,x,0);
//---------------------------------------------
//an error of 0.8.8d (greuel) result was totally wrong
// for mixed orderings
ring r3 = 32003,(t,x,y),(lp(1),ds);
ideal i = x-t3-t5, y-t10;
ideal is = std(i);
is;
//---------------------------------------------
//an error of 0.8.8.f (greuel) segmentation fault
ring r4 = 31,(t,x,y,z),dp;
poly tau=(1/2)*(1+6);
poly alpha=(1/4)*(2*tau+1);
ideal pencil = (tau^6-1)*x2y2z2-tau^4*(x4y2+y4z2+z4x2)+tau^2*(x2y4+y2z4+z2x4),
	       -t2*(x2+y2+z2-t2)*2;
matrix ppar[2][1] = 1, alpha;
ideal f = minor(ppar*matrix(pencil),1);
module jf = module(jacob(f));
module kf =std(jf);
dim(kf);
mult(kf);
degree(kf);
kill r4;
//---------------------------------------------
//an error of 0.9.2d (gmg) input not a resolvente
LIB "matrix.lib";LIB "homolog.lib";


int n=5;             
int m=3;
int N=n*(n+1) div 2;       //Anz. der Variablen
ring R=32003,x(1..N),dp;
matrix X=symmat(n);    //aus matrix.lib
ideal J=minor(X,m+1);
J=std(J);
module D=J*freemodule(N)+transpose(jacob(J)); // der Differentialmodul
D = transpose(D);
ncols(D); nrows(D);
option(prot);    

module E=Ext(2,D,J);

LIB "tst.lib";tst_status(1);$
