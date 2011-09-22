LIB "tst.lib"; tst_init();
LIB "curvepar.lib";
ring r=0,(x(1..3),a),dp;
poly f=a2+1;
ideal i=x(1)^2+a*x(2)^3,x(3)^2-x(2);
list l=1,3,2;
list B=BlowingUp(f,i,l);
B;
tst_status(1);$
