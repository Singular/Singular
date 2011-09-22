LIB "tst.lib"; tst_init();
LIB "algebra.lib";
ring s = 0,(x,y,z,a,b,c),(lp(3),dp);
ideal i= a -(xy)^3+x2-z, b -y2-1, c -z3;
ideal j = a -(xy)^3+x2-z, b -y2-1, c -z3, xy;
finitenessTest(std(i),1..3);
finitenessTest(std(j),1..3);
tst_status(1);$
