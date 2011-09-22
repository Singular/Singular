LIB "tst.lib"; tst_init();
LIB "latex.lib";
ring r2 = 13,(x,y),dp;
poly f = (x+1+y)^2*x3y*(2x-2y)*y12;
texfactorize("",f);
ring R49 = (7,a),x,dp;
minpoly = a2+a+3;
poly f = (a24x5+x3)*a2x6*(x+1)^2;
f;
texfactorize("",f);
tst_status(1);$
