option();

LIB "tst.lib"; tst_init();

option();


ring vc= 32003,(x,y,z,t),ds;
degBound=1;
ideal i= 49y5-9y4+41x2z+12y3+33y2+11y+73t,
33y4+21x2y+17x2z+63y3+11xy+77y2+91y+1t, 26y2+44xz+12y+9t;
i=i^2; i;


// option(redSB);
option(prot);
// test(23+32);
option(weightM);

option();
std(i);
option();

tst_status(1);$
