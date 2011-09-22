LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring s = 32003,(x,y,z),dp;
ideal i=x3+y2,xz+z2;
def R_r=sortandmap(i);
show(R_r);
setring R_r; IMAG;
kill R_r; setring s;
def R_r=sortandmap(i,1,xy,0,z,0,"ds",0,"lp",0);
show(R_r);
setring R_r; IMAG;
kill R_r;
tst_status(1);$
