//
// test script for vdim command
//
ring r1 = 0,(x,y,z),ds;
r1;
"--------------------------------";
ideal i1=x2+y2,x2-y2,x2+z2,x2-z2;
ideal j1=std(i1);
i1;
vdim(i1);
lead(i1);
j1;
vdim(j1);
lead(j1);
"--------------------------------";
vector v1=[x2+y2,y2-x2];
vector v2=[x2,y2,z2];
module m=v1,v2;
vdim(m);
module m1=std(m);
vdim(m1);
"--------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
