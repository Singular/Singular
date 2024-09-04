LIB "tst.lib"; tst_init();

ring r;
matrix m[0][0];
LIB"polylib.lib";
rad_con(0,ideal(m)); // mapping an empty ideal
rad_con(1,ideal(m));
rad_con(x,ideal(m));

tst_status(1);$
