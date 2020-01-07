LIB "tst.lib"; tst_init();
LIB"freegb.lib";

option(redSB); option(redTail);

// name: Uso3squared_c_dp
ring r = 0,(x,y,z),(C,dp);
ring R = freeAlgebra(r, 13, 12);
ideal I = (bracket(x,y)-z)^2, (bracket(z,x)-y)^2, (bracket(y,z)-x)^2;
I = twostd(I); // deg = 7
module S = syz(I); // 6 gens
S;
S = twostd(S);
module S2 = syz(S);
S2;
S2 = twostd(S2);
module S3 = syz(S2);
S3; // zero, correct

kill r; kill R;

// name: Uso3squared_C_dp
ring r = 0,(x,y,z),(C,dp);
ring R = freeAlgebra(r, 13, 12);
ideal I = (bracket(x,y)-z)^2, (bracket(z,x)-y)^2, (bracket(y,z)-x)^2;
I = twostd(I); // deg = 7
module S = syz(I); // 12 gens
S;
S = twostd(S);
module S2 = syz(S);
S2;
S2 = twostd(S2); // 6 gens
module S3 = syz(S2);
S3; // zero, correct

kill r; kill R;

// name: Uso3squared_dp_c
ring r = 0,(x,y,z),(dp,c);
ring R = freeAlgebra(r, 13, 9);
ideal I = (bracket(x,y)-z)^2, (bracket(z,x)-y)^2, (bracket(y,z)-x)^2;
I = twostd(I); // deg = 7
module S = syz(I); // 6 gens
S;
S = twostd(S);
module S2 = syz(S);
S2;
S2 = twostd(S2); // 3 gens
module S3 = syz(S2);
S3; // zero, correct

kill r; kill R;

ring r = 0,(a,b,c,d,e,f,g),Dp;
ring R = freeAlgebra(r, 5, 30);
ideal I = a*b-e, b*a-e, c*d-e, d*c-e, f*(d-a)-e, (d-a)*f-e, g*(b+f)-e, (b+f)*g-e, e*e-e, e*a-a, a*e-a, e*b-b, b*e-b, e*c-c, c*e-c, e*d-d, d*e-d, e*f-f, f*e-f, e*g-g, g*e-g;
ideal J = twostd(I); // 29 elts in deg at most 3
module S1 = syz(J); // size 132 of deg 3
S1;

tst_status(1);$
