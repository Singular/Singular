LIB "tst.lib";
tst_init();

ring r1 = integer,(x,y,z),dp;
ideal i = x4+y2+z2;
qring q = i;
ideal s = 2z,-4z2,-4yz,-4xz,-2x2z-2yz-2z2,-x4-y2-z2;
module a=syz(s);
print( NF(module(transpose(a)*transpose(s)),std(0)) );

ring r2=integer,(x, y),dp;
ideal I=81*y^6-162*y^5-27*y^4-x^2-9*y^2+18*y+3;
qring qr2=I;
ideal s=6,
-36,
-12*x,
-18*y^3-18*y^2-18*x-18*y-18,
-6*x*y^3-18*x*y^2-18*x^2-18*x*y-18*x,
-81*y^6+162*y^5+27*y^4+x^2+9*y^2-18*y-3;
module a=syz(s);
print( NF(module(transpose(a)*transpose(s)),std(0)) );

ring r3=integer,(x, y),dp;
ideal I=81*y^6-162*y^5;
qring qr3=I;
ideal s=6;
module a=syz(s);
print( NF(module(transpose(a)*transpose(s)),std(0)) );

ring r4 = integer,(x, y),dp;
ideal I=81*y^6-162*y^5-27*y^4-x^2-9*y^2+18*y+3;
qring qr4=I;
ideal s=6,
-36,
-12*x,
-18*y^3-18*y^2-18*x-18*y-18,
-6*x*y^3-18*x*y^2-18*x^2-18*x*y-18*x;
module a=syz(s);
print( NF(module(transpose(a)*transpose(s)),std(0)) );

ring r5=(integer),(x,y,z),(dp(3),C);

ideal I = x*y-3,
3*y^6+18*y^5*z-9*y^4*z^2-27*y^2*z^4-162*y*z^5+81*z^6-y^4,
27*x*z^6+3*y^5+18*y^4*z-9*y^3*z^2-27*y*z^4-162*z^5-y^3,
9*x^2*z^6-54*x*z^5+3*y^4+18*y^3*z-9*y^2*z^2-27*z^4-y^2,
3*x^3*z^6-18*x^2*z^5-9*x*z^4+3*y^3+18*y^2*z-9*y*z^2-y,
x^4*z^6-6*x^3*z^5-3*x^2*z^4+3*y^2+18*y*z-9*z^2-1;

module a=syz(I);
print( module(transpose(a)*transpose(I)) );
kill a;

ideal s =x*y+1,
-4,
-2*x*y-2,
-2*y^3-2*y^2*z-2*y*z^2-2*z^3-2*y^2,
-2*x*z^3+2*y^2+2*y*z+2*z^2+2*y,
-x*y+3,
-3*y^6-18*y^5*z+9*y^4*z^2+27*y^2*z^4+162*y*z^5-81*z^6+y^4,
-27*x*z^6-3*y^5-18*y^4*z+9*y^3*z^2+27*y*z^4+162*z^5+y^3,
-9*x^2*z^6+54*x*z^5-3*y^4-18*y^3*z+9*y^2*z^2+27*z^4+y^2;

module a=syz(s);
print( module(transpose(a)*transpose(s)) );

tst_status(1);$

