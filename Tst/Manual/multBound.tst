LIB "tst.lib"; tst_init();
ring r=0,(x,y,z),ds;
ideal i,j;
i=x7+y7+z6,x6+y8+z7,x7+y5+z8,
x2y3+y2z3+x3z2,x3y2+y3z2+x2z3;
multBound=100;
j=std(i);
degree(j);
multBound=0;  //disables multBound
j=std(i);
degree(j);
tst_status(1);$
