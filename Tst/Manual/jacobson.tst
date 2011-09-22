LIB "tst.lib"; tst_init();
LIB "jacobson.lib";
ring r = 0,(x,d),Dp;
def R = nc_algebra(1,1);   setring R; // the 1st Weyl algebra
matrix m[2][2] = d,x,0,d; print(m);
list J = jacobson(m); // returns a list with 3 entries
print(J[2]); // a Jacobson Form D for m
print(J[1]*m*J[3] - J[2]); // check that U*M*V = D
/*   now, let us do the same for the shift algebra  */
ring r2 = 0,(x,s),Dp;
def R2 = nc_algebra(1,s);   setring R2; // the 1st shift algebra
matrix m[2][2] = s,x,0,s; print(m); // matrix of the same for as above
list J = jacobson(m);
print(J[2]); // a Jacobson Form D, quite different from above
print(J[1]*m*J[3] - J[2]); // check that U*M*V = D
tst_status(1);$
