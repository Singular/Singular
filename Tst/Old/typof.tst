//
// test script for typeof command
//
pagelength = 10000;
ring r1 = 0,(x,y,z),dp;
r1;
"--------------------------------";
int i=10;
string s=typeof(i);
s;
"--------------------------------";
typeof(s);
"------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
