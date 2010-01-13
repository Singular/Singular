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

print(diff(x2,x));// -> 0
print(diff(ideal(x),ideal(x2))); // BUG: 0x
print(diff(ideal(x),ideal(x2+x,x4+x)));// BUG: 0x+1,0x3+1

tst_status(1);$

