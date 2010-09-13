//
// test script for timer command
//
ring r = 13,(x,y),dp;
r;
"--------------------------";
timer=1;
poly s1=x2;
s1;
"------------------------";
timer =20;
s1;
"-----------------------";
timer=0;
s1;
"------------------------";
listvar(all);
kill r;
LIB "tst.lib";tst_status(1);$;
