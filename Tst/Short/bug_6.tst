LIB "tst.lib";
tst_init();

ring r;
ring r1;
map f=r,ideal(x2,x3);
setring r;
preimage(r1,f,ideal(0));

tst_status(1);$
