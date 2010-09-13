//
// test script for nrows command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
ncols(freemodule(6));
"--------------------------------";
module m1=[0,0,0,1,0];
nrows(m1);
"-------------------------------";
vector v1=[x2,y2,z2];
vector v2=[x,y,z];
module m2=v1,v2;
nrows(m2);
"------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
