LIB "tst.lib";
tst_init();

ring r=0,(x,y,z),ws(1,2,3);
ring s=0,(a,b),dp;
r+s;
s+r;

tst_status(1);$
