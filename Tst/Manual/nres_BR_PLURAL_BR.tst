LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
def A = makeUsl2();
setring A; // this algebra is U(sl_2)
option(redSB);
option(redTail);
ideal i = e,f,h;
i = std(i);
resolution F=nres(i,0); F;
list l = F; l;
// see the exactness at this point:
size(ideal(transpose(l[2])*transpose(l[1])));
// see the exactness at this point:
size(ideal(transpose(l[3])*transpose(l[2])));
print(betti(l), "betti");
print(betti(minres(l)), "betti");
tst_status(1);$
