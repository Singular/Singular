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
$;
