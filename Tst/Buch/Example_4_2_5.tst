LIB "tst.lib";
tst_init();

option(redSB);     
ring R=0,(x,y),lp;
ideal Q=y4-4y3-10y2+28y+49,x3-6x2y+3x2+12xy2-12xy+3x-8y3
+13y2-8y-6;

factorize(Q[1]);    

ideal prim=std(y2-2y-7);
poly q=3x-6y+3;
poly f2=Q[2];
reduce(q^3-27*f2,prim);

tst_status(1);$
