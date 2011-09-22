LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def A = makeUsl2();
setring A; // this algebra is U(sl_2)
ideal i=e,f,h;
i=std(i);
resolution F=nres(i,0); F;
list lF = F; lF;
print(betti(lF), "betti");
resolution MF=minres(F);  MF;
list lMF = F; lMF;
print(betti(lMF), "betti");
tst_status(1);$
