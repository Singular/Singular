//
// test script for betti command
//
pagelength = 10000;
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
$;
