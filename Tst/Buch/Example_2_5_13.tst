LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),(c,dp);
ideal I=yz+z2,y2+xz,xy+z2,z3,xz2-z3,x2z;

resolution Re=mres(I,0);
Re;

print(Re);

resolution Se=sres(I,0);
Se;

print(Se);

print(minres(Se));


tst_status(1);$
