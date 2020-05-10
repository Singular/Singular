LIB "tst.lib"; tst_init();
LIB"freegb.lib";

option(redSB); option(redTail);

ring r = 0,(x,y,z),(c,dp);
ring R = freeAlgebra(r, 10, 3);
ideal h1 = x,y,z;
ideal h2 = x;
modulo(h1, h2);

tst_status(1);$
