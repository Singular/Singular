LIB "tst.lib"; tst_init();


// Example 3
ring rng = (integer),(x,y),(dp(2),C);
ideal I  = xy-2x-8y,y-2,x-8;
std(I);

tst_status(1);$
