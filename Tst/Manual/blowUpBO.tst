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
intvec ma;
list BO=W,J,b,E,abb,v,w,M,ma;
ideal C=CenterBO(BO)[1];
list blow=blowUpBO(BO,C,0);
def Q=blow[1];
setring Q;
BO;
tst_status(1);$
