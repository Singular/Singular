LIB "tst.lib";
tst_init();
ring r=0,(t,s,x,y,Dx,Dy),lp;
int N=6;
matrix W[N][N]=0;
W[3,5]=1;
W[4,6]=1;
W[1,2]=t;
def S=ncalgebra(1,W);setring S;
S;
ideal i = s + (x^4+y^5+x*y^4)*t, Dx+(4*x^3+y^4)*t, Dy+(5*y^4+4*x*y^3)*t;
ideal I=std(i);
I;
tst_status(1);$
