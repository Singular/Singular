LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ux","Ut","U";
list P="a";
list V="t","x";
setinitials(V,D,P);
def s1=scheme(u(Ut)+a*u(Ux),backward(Ux,U,x),forward(Ut,U,t));
s1;
tst_status(1);$
