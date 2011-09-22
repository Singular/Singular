LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
option(prot);
def R = makeUsl2();
// this algebra is U(sl_2)
setring R;
ideal I = e2, f2, h2-1;
I=std(I);
I;
kill R;
//------------------------------------------
def RQ = makeQso3(3);
// this algebra is U'_q(so_3),
// where Q is a 6th root of unity
setring RQ;
RQ;
ideal J=x2, y2, z2;
J=std(J);
J;
tst_status(1);$
