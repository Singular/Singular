LIB "tst.lib"; tst_init();
LIB "resolve.lib";
ring R=0,(x,y,z),dp;
ideal W=z^2-x;
ideal J=x*y^2+x^3;
intvec b=1;
list E;
ideal abb=maxideal(1);
intvec v;
intvec w=-1;
matrix M;
list BO=W,J,b,E,abb,v,w,M;
DeltaList(BO);
tst_status(1);$
