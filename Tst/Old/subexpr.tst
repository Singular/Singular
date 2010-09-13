ring r= 32003,(x,y,z),lp;
ideal i = x,y,z,x2z,yxz;
i;
i[2..3] = 3,4;                         // overwrite elems 2 .. 3
i;
matrix m[2][2] = i;                    // four elems from i
m;
m[1..2,1] = 1,2;                       // overwrite m[1,1], m[2,1]
m;
matrix mm[5][5];
mm[2..4,2..4] = 1,1,1,1,1,1,1,1,1;     // obvious ?
LIB "lib";                             // make formmat available
formmat(mm);
kill r;
LIB "tst.lib";tst_status(1);$;
