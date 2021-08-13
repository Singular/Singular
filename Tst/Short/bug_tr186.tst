LIB "tst.lib";
tst_init();

// new: liftstd with 3 args:
ring r;
ideal i=x,y,z;
matrix m;
module s=[1,2];
liftstd(i,m,s);
print(m);
print(s);
ring R=0,(x,y,z),dp;
poly f=x3+y7+z2+xyz;
ideal i=jacob(f);
matrix T;
ideal sm=liftstd(i,T);
sm;
print(T);
matrix(sm)-matrix(i)*T;
module s;
sm=liftstd(i,T,s);
print(s);
matrix(i)*matrix(s);
tst_status(1);$
