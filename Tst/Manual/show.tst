LIB "tst.lib"; tst_init();
LIB "inout.lib";
ring r;
show(r);
ideal i=x^3+y^5-6*z^3,xy,x3-y2;
show(i,3);            // introduce 3 space tabs before information
vector v=x*gen(1)+y*gen(3);
module m=v,2*v+gen(4);
list L = i,v,m;
show(L);
ring S=(0,T),(a,b,c,d),ws(1,2,3,4);
minpoly = T^2+1;
ideal i=a2+b,c2+T^2*d2; i=std(i);
qring Q=i;
show(Q);
map F=r,a2,b^2,3*c3;
show(F);
// Apply 'show' to i (which does not belong to the basering) by typing
// ring r; ideal i=xy,x3-y2; ring Q; show(r,"i");
tst_status(1);$
