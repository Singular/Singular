LIB "tst.lib"; tst_init();
LIB "grobcov.lib";
"Casas conjecture for degree 4";
ring R=(0,a0,a1,a2,a3,a4),(x1,x2,x3),dp;
ideal F=x1^4+(4*a3)*x1^3+(6*a2)*x1^2+(4*a1)*x1+(a0),
x1^3+(3*a3)*x1^2+(3*a2)*x1+(a1),
x2^4+(4*a3)*x2^3+(6*a2)*x2^2+(4*a1)*x2+(a0),
x2^2+(2*a3)*x2+(a2),
x3^4+(4*a3)*x3^3+(6*a2)*x3^2+(4*a1)*x3+(a0),
x3+(a3);
cgsdr(F);
tst_status(1);$
