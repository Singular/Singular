LIB "tst.lib"; tst_init();
LIB "dmodapp.lib";
ring r = 0,(a,x,b,Da,Dx,Db),dp;
def D3 = Weyl();
setring D3;
ideal I = a*Db-Dx+2*Da, x*Db-Da, x*Da+a*Da+b*Db+1,
x*Dx-2*x*Da-a*Da, b*Db^2+Dx*Da-Da^2+Db,
a*Dx*Da+2*x*Da^2+a*Da^2+b*Dx*Db+Dx+2*Da;
intvec w = 1,0,0;
def rm = restrictionModule(I,w);
setring rm; rm;
print(resMod);
tst_status(1);$
