//
// test script for size command
//
ring r1 = 32003,(x,y,z),dp;
r1;
"-------------------------------";
string s="feierabend";
intvec iv=1,2,3;
poly p=x+2y-z;
vector v=[x-z,x,y,0];
ideal i=p,z;
module m=v,[0,1],[2,3,0],4*v;
size(s);
"--------------------------";
size(iv);
"---------------------------";
size(p);
"---------------------------";
size(v);
"-----------------------------";
size(i);
"-------------------------------";
size(m);
"------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
