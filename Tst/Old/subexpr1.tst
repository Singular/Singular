//
// examples for subexpr notation in matrices and ideals
//
LIB "lib";
ring r = 33,(x,y,z),lp;
matrix mm[5][5] = x1,x2,x3,x4,x5;
     mm[1,1..5];
     mm[2,1..5];
     mm[3,1..5];
     mm[4,1..5];
     mm[5,1..5];
"----------------";
mm[1..5,1] = mm[1,1..5];
     mm[1,1..5];
     mm[2,1..5];
     mm[3,1..5];
     mm[4,1..5];
     mm[5,1..5];
"----------------";
3 * mm[1..5,1];
"----------------";
mm[1..5,1] * 4;
mm[1..5,2] = mm[1..5,1] + 2 * mm[1..5,1];
"----------------";
     mm[1,1..5];
     mm[2,1..5];
     mm[3,1..5];
     mm[4,1..5];
     mm[5,1..5];
"----------------";
fixmat(mm,6);  // formmat(mm) would be slower !!
mm[1..3,1..3] = stransp(3,3,mm[1..3,1..3]);
"----------------";
fixmat(mm,6);
matrix u[8][8] = unitmat(8,4);
"----------------";
fixmat(u,3);
kill r;
LIB "tst.lib";tst_status(1);$
