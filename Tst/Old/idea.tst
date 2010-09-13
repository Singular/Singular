//
// test script for ideal command
//
ring r1 = 0,(x,y,z),ds;
r1;
"------------------------------";
poly s1=12x2y4+34xyz5-78x2y4z2;
poly s2=3*x^2*y^3+6*x^3*y^2*z^4;
int i=2;
int j=6;
int k=45;
poly s3=i*x^k*y^j+k*x^(i+j)*z^k;
poly s4=7/9x3z7;
poly s5=12;
poly s6;
ideal i1=s1,s1+s2,s3-s1,s4*s3,s5^6;
i1;
"--------------------------------";
ideal i2=i1^2;
i2;
"--------------------------------";
ideal i3=i1*i2;
i3;
"----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
