LIB "tst.lib";
tst_init();

ring r;
list L=ideal(x);
L[1]=L[1],L[1];
L;
//
list LL;
LL[1] = LL[1],LL[1]; //should give an error
//
list LLL;
LLL[3]=ideal(1);
LLL[3][1]=x;
LLL;

tst_status(1);$
