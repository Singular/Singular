LIB "tst.lib"; tst_init(); option(prot);

ring be=32003,(a,b,c,d,e),dp;
ideal i=
a5-b5,
b5-c5,
c5-d5,
d5-e5,
a4b+b4c+c4d+d4e+e4a;

eliminate(i,ab);

tst_status(1);$
