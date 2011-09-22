LIB "tst.lib"; tst_init();
LIB "finvar.lib";
intvec dv=5,5,5,10,10;
print(power_products(dv,10));
print(power_products(dv,7));
tst_status(1);$
