LIB "tst.lib";
tst_init();

LIB "modular.lib";

ring R = 0, (x,y), dp;
ideal I = x9y2+x10, x2y7-y8;
modular("std", list(I));

tst_status(1);$
