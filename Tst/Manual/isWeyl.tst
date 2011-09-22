LIB "tst.lib"; tst_init();
LIB "nctools.lib";
ring r = 0,(a,b,c,d),dp;
isWeyl();
def D = Weyl(1); setring D; //make from r a Weyl algebra
b*a;
isWeyl();
ring t = 0,(Dx,x,y,Dy),dp;
matrix M[4][4]; M[1,2]=-1; M[3,4]=1;
def T = nc_algebra(1,M); setring T;
isWeyl();
tst_status(1);$
