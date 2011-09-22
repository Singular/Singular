LIB "tst.lib"; tst_init();
ring r=0,(x,y),dp;
// determine f with deg(f)=2 and with given values v of f
// at 9 points: (2,3)^0=(1,1),...,(2,3)^8=(2^8,3^8)
// valuation point: (2,3)
ideal p=2,3;
ideal v=1,2,3,4,5,6,7,8,9;
poly ip=vandermonde(p,v,2);
ip[1..5];  //  the 5 first terms of ip:
// compute value of ip at the point 2^8,3^8, result must be 9
subst(subst(ip,x,2^8),y,3^8);
tst_status(1);$
