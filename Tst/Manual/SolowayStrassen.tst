LIB "tst.lib"; tst_init();
LIB "crypto.lib";
bigint h=10;
bigint p=h^100+267;
//p=h^100+43723;
//p=h^200+632347;
SolowayStrassen(h,3);
tst_status(1);$
