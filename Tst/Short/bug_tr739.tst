LIB "tst.lib";
tst_init();

ring rng = 2,(x,y,z),dp;
ideal I = x*z^2+y^2+y, x*y+y^2*z^2;
matrix cs=char_series(I);
print(cs);

tst_status(1);$
