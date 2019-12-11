LIB "tst.lib"; tst_init();
   ring R = (0,x),(y,z,u,v),lp;
minpoly = x2+1;
ideal i = x3,x2+y+z+u+v,xyzuv-1; i;
def P = par2varRing(i)[1]; P;
setring(P);
Id[1];
setring R;
module m = x3*[1,1,1], (xyzuv-1)*[1,0,1];
def Q = par2varRing(m)[1]; Q;
setring(Q);
print(Id[1]);
tst_status(1);$
