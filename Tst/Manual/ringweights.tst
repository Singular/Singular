LIB "tst.lib"; tst_init();
LIB "ring.lib";
ring r0 = 0,(x,y,z),dp;
ringweights(r0);
ring r1 = 0,x(1..5),(ds(3),wp(2,3));
ringweights(r1);"";
// an example for enlarging the ring, keeping the first weights:
intvec v = ringweights(r1),6,2,3,4,5;
ring R = 0,x(1..10),(a(v),dp);
ordstr(R);
tst_status(1);$
