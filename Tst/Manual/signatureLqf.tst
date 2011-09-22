LIB "tst.lib"; tst_init();
LIB "phindex.lib";
ring r=0,(x(1..4)),ds;
poly Ax=4*x(1)^2+x(2)^2+x(3)^2+x(4)^2-4*x(1)*x(2)-4*x(1)*x(3)+4*x(1)*x(4)+4*x(2)*x(3)-4*x(2)*x(4);
signatureLqf(Ax,1); //The rank of Ax is 3+1=4
poly Bx=2*x(1)*x(4)+x(2)^2+x(3)^2;
signatureLqf(Bx);
tst_status(1);$
