LIB "tst.lib";
tst_init();

ring A=0,(x,y),(c,ds);
ideal I=x,y;
resolution Re=mres(I,0);
Re;

print(Re);

print(betti(Re),"betti");

qring R=std(xy);
module M=[x,0],[0,y];
resolution Re=mres(M,4);
Re;

print(Re);

tst_status(1);$
