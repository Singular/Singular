//
// test script for mres command
//
option(prot);
ring r1=32003,(x,y,z),dp;
r1;
"-----------------------";
ideal i=x2,y2,z2;
i;
list(mres(i,0)); 
"--------------------------";
module m=[x2,0],[0,y4];
m;
list(mres(m,0));
"-------------------------------";
ring r2=31991,(t,x,y,z,w),ls;
ideal j=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
j;
list(mres(j,0));
"-------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;

