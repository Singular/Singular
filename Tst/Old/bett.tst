//
// test script for betti command
//
ring r1=32003,(x,y,z),dp;
r1;
"-------------------------------";
ideal i1=xy-yz,x3-y2z,z3-xy2;
i1;
list T=mres(i1,0);
print(betti(T),"betti");
"------------------------------";
listvar(all);
kill r1;
LIB "tst.lib";tst_status(1);$;
