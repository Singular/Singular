LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
ring R = 0,x,dp;
intmat m[5][5] =
13,25,37,83,294,
12,-33,9,0,64,
77,12,34,6,1,
43,2,88,91,100,
-46,32,37,42,15;
lll(m);
list l =
intvec(13,25,37, 83, 294),
intvec(12, -33, 9,0,64),
intvec (77,12,34,6,1),
intvec (43,2,88,91,100),
intvec (-46,32,37,42,15);
lll(l);
tst_status(1);$
