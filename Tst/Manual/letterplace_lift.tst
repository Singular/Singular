LIB "tst.lib"; tst_init();
LIB"freegb.lib";

option(redSB); option(redTail);

ring r = 0,(x,y,z),(c,Dp);
ring R = freeAlgebra(r, 20, 3);
// poly p = y*y*y*y + x*x*x + x*y*z + z*z;
ideal M = 3*x*x + y*z, 4*y*y*y + x*z, x*y + 2*z; // = jacob(p)
ideal SM = 3*x*x + y*z, 4*y*y*y + 2*x*x*y + 5*x*z;
lift(M, SM);

kill r; kill R;

ring r = 0,(a,b,c,d,e,f,g),(c,Dp);
ring R = freeAlgebra(r, 5, 30);
ideal I = a*b-e, b*a-e, c*d-e, d*c-e, f*(d-a)-e, (d-a)*f-e, g*(b+f)-e, (b+f)*g-e, e*e-e, e*a-a, a*e-a, e*b-b, b*e-b, e*c-c, c*e-c, e*d-d, d*e-d, e*f-f, f*e-f, e*g-g, g*e-g;
ideal J = twostd(I); // 29 elts in deg at most 3
poly hua = a - a*c*a - g;
lift(J,hua);

kill r; kill R;

ring r = 0, (a,b,c,d,e,f,g),(c, Dp);
ring R = freeAlgebra(r, 20, 30);
ideal I = a*b-e, b*a-e, c*d-e, d*c-e, f*(d-a)-e, (d-a)*f-e, g*(b+f)-e, (b+f)*g-e, e*e-e, e*a-a, a*e-a, e*b-b, b*e-b, e*c-c, c*e-c, e*d-d, d*e-d, e*f-f, f*e-f, e*g-g, g*e-g;
matrix T;
ideal J = liftstd(I,T);
J;
T;

tst_status(1);$
