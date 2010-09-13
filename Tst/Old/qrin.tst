//
// test script for qring command
//
ring r1 = 32003,(x,y,z),dp;
r1;
"------------------------------";
poly s4=x;
poly s5=y;
poly s6=z;
ideal i1=s4,s5,s6;
ideal j1=std(i1);
j1;
qring q1=j1;
basering;
"----------------------------";
ring r2=32003,(x,y,z),ds;
poly s1=x3y4+123x7y+34x2yz6+23y6z4;
poly s2=34x8y6z4+9z9;
poly s3=9xz7+89xy7z8;
ideal i2=s1,s2,s3;
ideal j2=std(i2);
j2;
qring q2=j2;
basering;
"----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
