LIB "tst.lib"; tst_init();
ring r=0,(x,y),dp;
def R=nc_algebra(-1,0);  //anti-commutative algebra
setring R;
module M=[x,x],[y,0];
module N=[0,y^2],[y,x];
option(redSB);
module Res;
Res=intersect(M,N);
print(Res);
kill r,R;
//--------------------------------
LIB "ncalg.lib";
ring r=0,(x,d),dp;
def RR=Weyl(); // make r into Weyl algebra
setring RR;
ideal I = x+d^2;
ideal J = d-1;
ideal H = intersect(I,J);
H;
tst_status(1);$
