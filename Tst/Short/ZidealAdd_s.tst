//
// test script for addition of ideals over rings
//
LIB "tst.lib";
tst_init();

ring r=integer,(x,y),dp;
ideal I=9,x;
ideal J=3,x2;
I+J;
I=9,x;
J=3,-x;
I+J;
I=-3,x;
J=3,2x;
I+J;
kill r;
// case over Z/m
ring r=(integer,10),(x,y),dp;
ideal I=4,x;
ideal J=6,x2;
I+J;
I=5,x;
J=5,-x;
I+J;
I=2,x;
J=4,2x;
I+J;
I=9,x;
J=2,x;
I+J;
I=5,2x;
J=3x;
I+J;
kill r;
// case over Z/2^n
ring r=(integer,2,3),(x,y),dp;
ideal I=4,-x2;
ideal J=2x2;
I+J;
I=2,4x;
J=2x;
I+J;
kill r;

tst_status(1);$
