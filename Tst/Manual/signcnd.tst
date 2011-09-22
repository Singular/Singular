LIB "tst.lib"; tst_init();
LIB "signcond.lib";
ring r = 0,(x,y),dp;
ideal i = (x-2)*(x+3)*x,y*(y-1);
ideal P = x,y;
list l = signcnd(P,i);
size(l[1]);     // = the number of sign conditions of P on V(i)
//Each element of l[1] indicates a sign condition of the polynomials of P.
//The following means P[1] > 0, P[2] = 0:
l[1][2];
//Each element of l[2] indicates how many elements of V(I) give rise to
//the sign condition expressed by the same position on the first list.
//The following means that exactly 1 element of V(I) gives rise to the
//condition P[1] > 0, P[2] = 0:
l[2][2];
tst_status(1);$
