//
// test script for maxdeg command
//
pagelength = 10000;
ring r = 0,(x,y,z),dp;
r;
"---------------------------";
ideal i1=x6y5,x8+z9+y7,xyz6+xz9;
i1;
degBound=10;
test(24);
ideal i2=std(i1);
i2;
"--------------------------";
listvar(all);
kill r;
$;
