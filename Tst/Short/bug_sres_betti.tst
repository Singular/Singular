LIB "tst.lib";
tst_init();
ring r=32003,(y,t,x,z,w),(dp, c);
ideal i=6*t^5*x^3*z^4+5*x^5*z^7+4*t^4*y*z^3*w^4,
5*t^3*x^2*y^7*z^4+5*t^6*x*y^3*z^3*w^3+4*t^5*x^5*y*w^5,
2*t^8*z^6+6*t^5*y*z^7*w+6*t^4*y^2*z^3*w^5;
resolution rs = sres(std(i), 0);rs;
resolution rl = lres(i, 0);rl;
intmat is = betti(rs);is;
intmat il = betti(rl);il;
if (size(is) != size(il)) {"ERROR: is != il";}
tst_status(1); 
$
