//
// test script for execute command
//
pagelength = 10000;
ring r1=32003,(x,y,z),dp;
r1;
"-------------------------------";
ideal j=x+y,z4+23y;
system("sh","/bin/rm -f save_j");
write("save_j",j);
ring r2=0,(x,y,z),Dp;
string s="ideal k="+read("save_j")+";";
execute s;
k;
"-------------------------------";
listvar(all);
kill r1,r2;
LIB "tst.lib";tst_status(1);$;
