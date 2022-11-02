LIB "tst.lib";
tst_init();

// checking for overflow in std/interred/groebner
option(redSB);

ring r1=(0),(a,b,c,d,e,f,g,h),(lp(8),C);
ideal I=c-d1024,
b-c16,
bd1024-c17,
a-b16,
ad1024-c257,
ac16-b17;
std(I);
interred(I);
groebner(I);

ring r2=(0),(a,b,c,d,e,f,g,h),(lp(8),C);
ideal I=c-d1024,
b-c16,
a-b16;
std(I);
interred(I);
groebner(I);

ring r3=0,(b,c,d,e,f,g,h),lp;
ideal I=b-c^64,c-d^1024;
std(I);
interred(I);
groebner(I);

ring r4=(0),(a,b,c,d,e,f,g,h),(lp(8),C,L(262144));
ideal I=c-d1024,
b-c16,
bd1024-c17,
a-b16,
ad1024-c257,
ac16-b17;
std(I);
interred(I);
groebner(I);

tst_status(1); $;

