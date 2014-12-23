LIB "tst.lib"; tst_init();

ring R = 0, (x, y, U(1), U(2)), (dp(4), C); R;

ideal I;
I[1]=x^3*U(2);
I[2]=y^3*U(1);
I[3]=y^4;
I[4]=x^4;
I[5]=U(1)*U(2);

ideal rad=1;

// TRACE=3; option(prot);
quotient(I, rad);


tst_status(1);$
