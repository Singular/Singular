LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r=0,(e(1),e(2),x,y,z),(dp(2),ds(3));
ideal i=e(2)^2,e(1)*e(2),e(1)^2,e(1)*y+e(2)*x;
intvec iv=2,1;
id2mod(i,iv);
tst_status(1);$
