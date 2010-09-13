//
// test script for minbase command
//
ring r1 = 32003,(x,y,z),(c,ls);
r1;
"-------------------------------";
ideal i1=x2+xyz,y2-z3y,z3+y5xz;
ideal i2=maxideal(3)+i1;
i2;
minbase(i2);
"-------------------------------";
i1;
minbase(i1);
"-------------------------------";
ring r2=0,(x,y,z),(c,ds);
r2;
"-------------------------------";
ideal i3=x4+y4z,y2+xz2,z2+yz2;
ideal i4=maxideal(2)+i3;
i4;
minbase(i4);
"---------------------------------";
i3;
minbase(i3);
"--------------------------------";
ring r3=0,(x(1..8)),ds;
r3;
"-------------------------------";
matrix m[4][4]=maxideal(3);
ideal i5=maxideal(2)+minor(m,2);
i5;
minbase(i5);
"---------------------------------";
listvar(all);
kill r1,r2,r3;
LIB "tst.lib";tst_status(1);$;
