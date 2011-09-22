LIB "tst.lib"; tst_init();
LIB "jacobson.lib";
ring R=(0,m,M,L1,L2,m1,m2,g), D, lp; // two pendula example
matrix P[3][4]=m1*L1*D^2,m2*L2*D^2,(M+m1+m2)*D^2,-1,
m1*L1^2*D^2-m1*L1*g,0,m1*L1*D^2,0,0,
m2*L2^2*D^2-m2*L2*g,m2*L2*D^2,0;
list s=smith(P,1);  // returns a list with 3 entries
print(s[2]); // a diagonal form, close to the Smith form
print(s[1]); // U, left transformation matrix
list t = divideUnits(s);
print(t[2]); // the Smith form of the matrix P
print(t[1]); // U', modified left transformation matrix
tst_status(1);$
