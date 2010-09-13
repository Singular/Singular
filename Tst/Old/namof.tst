//
// test script for nameof command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;
LIB "lib0";
"-------------------------------";
int i=9;
string s=nameof(i);
s;
"---------------------------------";
string t=nameof(s);
t;
"------------------------------";
string s1=nameof(1);
"-----------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
