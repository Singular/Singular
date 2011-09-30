LIB "tst.lib"; tst_init();

ring BR = 2, (x,y), (lp,C);
poly A = y2+y+1;

ring R = (2,@a), (x,y), (lp,C);
map psi=BR,x,@a;
minpoly=number(psi(A));


tst_status(1);$
