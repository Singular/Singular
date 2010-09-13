//
// test script for vector command
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
vector v=[s1,s3-s2,s4-s1]+s5*gen(5);
v;
"------------------------------";
vector v1=[x2,y2];
v1;
"----------------------------";
vector v2=[1,0];
v2;
"----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
