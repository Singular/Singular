//
// test script for freemodule command
//
pagelength = 10000;
ring r1 = 32003,(x,y,z),(c,dp);
r1;
"-------------------------------";
freemodule(3);
listvar(all);
kill r1;
$;
