LIB "tst.lib";
tst_init();
//
// test script for quotient command
//
ring r1 = 32003,(x,y,z),(c,ls);
r1;
"-------------------------------";
ideal i1=maxideal(3);
ideal i2=x2+xyz,y2-z4y,z3+y6x2z;
ideal i3=quotient(i1,i2);
i3;
"---------------------------------";
ideal i4=quotient(i2,i1);
i4;
"---------------------------------";
ideal i5=quotient(i2,maxideal(2));
i5;
"--------------------------------";
listvar(all);
kill r1;
ring r=181,(x,y,z),(c,ls);
ideal id1=maxideal(3);
id1;
ideal id2=x2+xyz,y2-z3y,z3+y5xz;
id2;
ideal id6=quotient(id1,id2);
id6;
ideal id7=quotient(id2,id1);
id7;
tst_status(1);$;
