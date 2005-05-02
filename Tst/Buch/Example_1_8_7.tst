LIB "tst.lib";
tst_init();

ring  A=0,(x,y,z),lp;
ideal I=x2+y+z-1,
         x+y2+z-1,
         x+y+z2-1;
ideal J=groebner(I);        
J;

LIB"solve.lib";
laguerre_solve(J[1],6);

def C=triang_solve(triangMH(J),6);
setring C;

rlist;

setring A;
vdim(J);
LIB "sing.lib";
ideal sJ=groebner(slocus(I));
def D=triang_solve(triangMH(sJ),6);
setring D;
rlist;

tst_status(1);$
