LIB "tst.lib"; tst_init();
LIB "modstd.lib";
ring R1 = 0, (x,y,z,t), dp;
ideal I = 3x3+x2+1, 11y5+y3+2, 5z4+z2+4;
ideal J = modStd(I);
J;
I = homog(I, t);
J = modStd(I);
J;

ring R2 = 0, (x,y,z), ds;
ideal I = jacob(x5+y6+z7+xyz);
ideal J = modStd(I, 0);
J;

ring R3 = 0, x(1..4), lp;
ideal I = cyclic(4);
ideal J1 = modStd(I, 1);   // default
ideal J2 = modStd(I, 0);
size(reduce(J1, J2));
size(reduce(J2, J1));
tst_status(1);$
