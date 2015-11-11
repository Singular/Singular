LIB "tst.lib"; tst_init();


// Example 4
ring rng = (integer),(x,y),(dp(2),C);
poly  a = 2x;
ideal J = 3y,2x,xy;
quotient( a*J, J );

tst_status(1);$
