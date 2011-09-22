LIB "tst.lib"; tst_init();
LIB "dmod.lib";
//  LIB "dmod.lib"; option(prot); option(mem);
ring r = 0,(x,y),Dp;
poly F = x^3+y^3+x*y^3;
def A = Sannfs(F); // here we get LD = ann f^s
setring A;
poly F = imap(r,F);
def B = annfs0(LD,F); // to obtain BS polynomial
list BS = imap(B,BS);   poly bs = fl2poly(BS,"s");
poly PS = operatorModulo(F,LD,bs);
LD = groebner(LD);
PS = NF(PS,subst(LD,s,s+1));; // reduction modulo Ann s^{s+1}
size(PS);
lead(PS);
reduce(PS*F-bs,LD); // check the defining property of PS
tst_status(1);$
