LIB "tst.lib"; tst_init();
LIB "grobcov.lib";
ring R=(0,a,b,c),(x,y),dp;
setglobalrings();
poly f=(b^2-1)*x^3*y+(c^2-1)*x*y^2+(c^2*b-b)*x+(a-bc)*y;
ideal N=(ab-c)*(a-b),(a-bc)*(a-b);
ideal W=a^2-b^2,bc;
def r=redspec(N,W);
pnormalform(f,r[1],r[2]);
tst_status(1);$
