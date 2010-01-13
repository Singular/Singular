LIB "tst.lib";
tst_init();

ring r=0,(x,y),dp;
ideal H = x,y;
ideal I = x2,y2;
print(diff(H,I));
ring r2=2,(x,y),dp;
ideal H = x,y;
ideal I = x2,y2;
print(diff(H,I));

tst_status(1);$

