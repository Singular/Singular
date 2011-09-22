LIB "tst.lib"; tst_init();
LIB "jacobson.lib";
ring r = 0,x,Dp;
matrix m[3][2]=x, x^4+x^2+21, x^4+x^2+x, x^3+x, 4*x^2+x, x;
list s=smith(m,1);
print(s[2]);  // non-normalized Smith form of m
print(s[1]*m*s[3] - s[2]); // check U*M*V = D
list t = divideUnits(s);
print(t[2]); // the Smith form of m
tst_status(1);$
