//
// test script for nvars command
//
ring r1 = 32003,(x,y,z),(c,ds);
r1;
nvars(r1);
"--------------------------------";
ring r2=23,(x(1..36)),ds;
r2;
nvars(r2);
"-------------------------------";
ring r3=0,(x,y,z(1..18)),ds;
r3;
nvars(r3);
"------------------------------";
ring r4=0,(x(1..6),y(1..7),z(1..8),a),ds;
r4;
nvars(r4);
"--------------------------------";
listvar(all);
kill r1,r2,r3,r4;
LIB "tst.lib";tst_status(1);$;
