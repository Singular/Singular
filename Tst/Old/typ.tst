//
// test script for type command
//
//option(prot);
ring r1=32003,(a,b,x,y,z),ls;
r1;
"--------------------------";
poly s1=12;
poly s2=x2;
int k=10;
ideal i=x2,y2;
module m=[0,x2],[z4,0];
type m;
"---------------------------";
type r1;
"----------------------------";
type s1;
"---------------------------";
type s2;
"---------------------------";
type k;
"------------------------------";
type i;
"--------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
