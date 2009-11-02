LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id$");
ring r=32003,(y,t,x,z,w),dp;
ideal i=6*t^5*x^3*z^4+5*x^5*z^7+4*t^4*y*z^3*w^4,
5*t^3*x^2*y^7*z^4+5*t^6*x*y^3*z^3*w^3+4*t^5*x^5*y*w^5,
2*t^8*z^6+6*t^5*y*z^7*w+6*t^4*y^2*z^3*w^5;
resolution rs=lres(i,0);
betti(rs);
list I=rs;
betti(I);
minres(rs);
list J=rs;
betti(J);
kill r;
ring an=0,(t,x,y,z,w),(dp,C);
ideal i=
2t3x5y2z+t5x2yw3+2x2yw8,
3t6x2yz2+9t3y2zw5+2xy3z2w5,
t3x5y2z2+tx2y3w6+2t5zw6;
resolution rs=lres(i,0);
betti(rs);
list I=rs;
betti(I);
minres(rs);
list J=rs;
betti(J);
kill an;
ring h3=32003,(z,y,t,x,w),(dp,C);
ideal i=
5*t^3*x^8*y^2*z+1*t^5*x^2*y*w^6+2*x^2*y*w^11+5*t*w^13,
7*x*y^6*z^2+4*t^6*x*w^2+9*t^3*y^2*z*w^3+6*x^2*y*w^6,
8*t*x^5*y^3+2*t^3*x^3*y^2*w+4*t^6*z*w^2+3*x*w^8;
resolution rs=lres(i,0);
betti(rs);
list I=rs;
betti(I);
minres(rs);
list J=rs;
betti(J);
kill h3;
tst_status(1);$
