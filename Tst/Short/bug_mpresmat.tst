LIB "tst.lib"; tst_init();

ring R=(0,t),(x,y,z),lp;
poly f=x^6+y^6 -z^6 -(x+y-z)^6;
poly g=f/((x-z)*(y-z));
ideal J=g,x*diff(g,x),y*diff(g,y),z*diff(g,z);
J;
module m=mpresmat(J,0);

tst_status(1);$

