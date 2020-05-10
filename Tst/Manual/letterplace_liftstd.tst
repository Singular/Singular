LIB "tst.lib"; tst_init();
LIB"freegb.lib";

option(redSB); option(redTail);

ring r = 0,(x,y,z),(c,Dp);
ring R = freeAlgebra(r, 20, 3);
// poly p = y*y*y*y + x*x*x + x*y*z + z*z;
ideal M = 3*x*x + y*z, 4*y*y*y + x*z, x*y + 2*z; // = jacob(p)
//ideal SM = 3*x*x + y*z, 4*y*y*y + 2*x*x*y + 5*x*z;
matrix T;
module S;
ideal J = liftstd(M, T, S);
J;
T;
S;

kill r; kill R;

ring r = 0,(a,b,c,d,e,f,g),(c,Dp);
ring R = freeAlgebra(r, 10, 30);
ideal I = a*b-e, b*a-e, c*d-e, d*c-e, f*(d-a)-e, (d-a)*f-e, g*(b+f)-e, (b+f)*g-e, e*e-e, e*a-a, a*e-a, e*b-b, b*e-b, e*c-c, c*e-c, e*d-d, d*e-d, e*f-f, f*e-f, e*g-g, g*e-g;
matrix T;
module S;
ideal J = liftstd(I, T, S);
J;
T;
S;

kill r; kill R;

ring r = 0, (a,b,c,d,e,f,g),(c, Dp);
ring R = freeAlgebra(r, 20, 30);
ideal I = a*b-e, b*a-e, c*d-e, d*c-e, f*(d-a)-e, (d-a)*f-e, g*(b+f)-e, (b+f)*g-e, e*e-e, e*a-a, a*e-a, e*b-b, b*e-b, e*c-c, c*e-c, e*d-d, d*e-d, e*f-f, f*e-f, e*g-g, g*e-g;
matrix T;
module S;
ideal J = liftstd(I, T, S);
J;
T;
S;

tst_status(1);$

// to test the results testLift(I, T) should be equal to J, testLift(I, S) should be zero and reduce(J, std(I)) and reduce(std(I), J) should be zero
