//
// test script for typeof command
//
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
