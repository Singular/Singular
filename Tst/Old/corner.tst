//
// test script for maxdeg command
//
LIB"polylib.lib";
ring r1 = 32003,(x,y,z),(c,ds);
option(prot);
r1;
"-------------------------------";
poly s1=x3y2+21328x5y+10667x2y4+21328x2yz3+10666xy6+10667y9;
poly s2=x2y2z2+3z8;
poly s3=5x4y2+4xy5+2x2y2z3+y7+11x10;
ideal i1=s1,s2,s3;
test(27);
ideal i2=std(i1);
maxdeg(i2);
"--------------------------------";
poly s4=x6+y6+z6;
poly s5=xy5+z7+xz8;
poly s6=yz8+x4y4+x8z8;
ideal i3=s4,s5,s6;
ideal i4=std(i3);
maxdeg(i4);
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
