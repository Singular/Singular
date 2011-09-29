LIB "tst.lib"; tst_init();
//      H7 l, char 0, test0,11,1: 61/31 ohne vollst; Reduktion
ring r= 0,(x,y),lp;
poly f=x5+y11+xy9+x3y9;
ideal i=jacob(f);
i;

option(prot, nosugarCrit);
std(i);

option(sugarCrit);
std(i);
tst_status(1);$
