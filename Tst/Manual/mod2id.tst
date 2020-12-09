LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r=0,(e(1),e(2),x,y,z),(dp(2),ds(3));
module mo=x*gen(1)+y*gen(2);
intvec iv=2,1;
mod2id(mo,iv);
tst_status(1);$
