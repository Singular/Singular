LIB "tst.lib"; tst_init();
LIB "finitediff.lib";
list D="Ux","Ut","U";
list P="a";
list V="t","x";
setinitials(V,D,P);
def s1=scheme(u(Ut)+a*u(Ux),laxfrX(Ux,U,x),laxfrT(Ut,U,x));
s1;
def s2=errormap(s1);
s2;
def s3=ComplexValue(s2);s3;
qepcad(s3);
tst_status(1);$
