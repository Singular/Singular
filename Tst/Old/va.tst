//
// test script for var command
//
ring r1 = 0,(x,y,z),dp;
r1;
varstr(r1,3);
"-----------------------------------";
ring r2=0,(x(1..10),y(1..9),z(1..8)),dp;
r2;
varstr(r2,10);
varstr(r2,12);
varstr(r2,22);
varstr(basering,3);
varstr(r2,28);
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
