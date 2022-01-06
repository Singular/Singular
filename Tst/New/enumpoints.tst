LIB "tst.lib";
tst_init();

LIB"enumpoints.lib";
ring R2 = (0,q),(x,y,z,w),dp;
minpoly = q16-q12+q8-q4+1;
poly s = 2q13-q9-q7+q5-q3-q;
ideal Togliatti = 64*(x-w)*(x^4 -4*x^3*w -10*x^2*y^2 -4*x^2*w^2 +16*x*w^3 -20*x*y^2*w+5*y^4 +16*w^4 -20*y^2*w^2) -5*s*(2*z -s*w)*(4*(x^2+y^2-z^2) +(1+3*(5-s^2))*w^2)^2;
matrix Jac = jacob(Togliatti);
ideal I2 = Togliatti+Jac;
projPoints(std(I2));

ring R1 = 7823,(x,y,z,w),dp;
ideal I1 = 4*x*z + 2*x*w + y^2 + 4*y*w + 7821*z^2 + 7820*w^2,
4*x^2 + 4*x*y + 7821*x*w + 7822*y^2 + 7821*y*w + 7821*z^2 + 7819*z*w + 7820*w^2;
projPoints(std(I1));

tst_status(1);$

