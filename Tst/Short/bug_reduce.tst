LIB "tst.lib";
tst_init();
// error in NF: wrong strat. due to wrong rank

ring r = 0,(x,y),ds;
poly f = x^5+y^5;
ideal j = jacob(f),f;
j = std(j);
maxideal(6);
NF(x3y3,j);             //results in x3y3 correct
NF(maxideal(6),j);      //results ii 0    NOT correct
// and for module:
NF(x3y3*gen(1),j*gen(1));             //results in x3y3 correct
NF(maxideal(6)*gen(1),j*gen(1));      //results ii 0    NOT correct

tst_status(1);$
