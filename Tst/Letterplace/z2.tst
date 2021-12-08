LIB "tst.lib"; tst_init();
LIB "freegb.lib";
ring r = integer,(x,y,z),Dp;
ring R = freeAlgebra(r,5,2); // length bound is 5, rank of the free bimodule is 2
ideal I = 2*x, 3*y;
matrix T1 = lift(I, ideal(y*z*x,x*z*y));
print(T1);
-y*z*I[1] + I[2]*z*x; // gives y*z*x
matrix T2 = lift(I, ideal(y*z^2*x,x*z^2*y));
print(T2);
-y*z^2*I[1] + I[2]*z^2*x; // gives y*z^2*x

tst_status(1);$
