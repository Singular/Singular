LIB "tst.lib"; tst_init();
ring r = (0,a),(e,f,h),(c,dp);
matrix D[3][3];
D[1,2]=-h;  D[1,3]=2*e;  D[2,3]=-2*f;
def R=nc_algebra(1,D); // this algebra is a parametric U(sl_2)
setring R;
ideal i = e,h-a; // consider this parametric ideal
i = std(i);
print(matrix(i)); // print a compact presentation of i
poly Z = 4*e*f+h^2-2*h; // a central element
Z = Z - NF(Z,i); // a central character
ideal j = std(Z);
j;
matrix T = lift(i,j);
print(T);
ideal tj = ideal(transpose(T)*transpose(matrix(i)));
size(ideal(matrix(j)-matrix(tj))); // test for 0
tst_status(1);$
