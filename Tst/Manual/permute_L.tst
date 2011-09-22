LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
list L=list();
L[1]="a";
L[2]="b";
L[3]="c";
L[4]="d";
intvec P=1,3,4,2;
// the list L is permuted according to P :
permute_L(L,P);
tst_status(1);$
