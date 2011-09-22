LIB "tst.lib"; tst_init();
LIB "resolve.lib";
ring R=0,(x,y),dp;
ideal W;
ideal J=x2-y3;
intvec b=1;
list E;
ideal abb=maxideal(1);
intvec v;
intvec w=-1;
matrix M;
list BO=W,J,b,E,abb,v,w,M,v;
CenterBO(BO);
tst_status(1);$
