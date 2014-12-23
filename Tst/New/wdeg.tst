LIB "tst.lib"; tst_init();

intvec iv1m1 = intvec(1,-1);

"iv1m1: ", iv1m1;


ring A = (0),(x,d),(dp(2),C);


// TRACE=3; option(prot);

deg(xd4+xd2+2d3, iv1m1);
deg(xd2+2d3, iv1m1);
deg(2d3, iv1m1);


ring B = (0),(d),(dp(1),C);
iv1m1 = intvec(-1);

deg(d4+d2+2d3, iv1m1);
deg(d2+2d3, iv1m1);
deg(d3, iv1m1);

tst_status(1);$


