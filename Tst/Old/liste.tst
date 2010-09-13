//
// test script for listvar command
//
ring r1=32003,(x,y,z),dp;
r1;
"-----------------------";
poly s2=12x2+3y2;
int i=34;
ring r2=0,(x,y,z),ds;
poly s1=x4+y2;
ideal i1=x2,y2,z2;
listvar();
"--------------------------";
listvar(r2);
"--------------------------";
listvar(ring);
"----------------------------";
listvar(poly);
"----------------------------";
listvar(i1); 
"--------------------------";
listvar(all);
kill r1,r2;
LIB "tst.lib";tst_status(1);$;

