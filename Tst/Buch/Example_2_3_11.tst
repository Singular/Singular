LIB "tst.lib";
tst_init();

ring A=0,(x,y,z),(c,dp);
module I=[x+1,y,1],[xy,z,z2];
std(I);

ring B=0,(x,y,z),dp;
module I=fetch(A,I);
std(I);

ring C=0,(x,y,z),lp;
module I=fetch(A,I);
std(I);

ring D=0,(x,y,z),(c,ds);
module I=fetch(A,I);
std(I);

ring E=0,(x,y,z),ds;
module I=fetch(A,I);
std(I);

tst_status(1);$
