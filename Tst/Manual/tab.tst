LIB "tst.lib"; tst_init();
LIB "inout.lib";
for(int n=0; n<=5; n=n+1)
{ tab(5-n)+"*"+tab(n)+"+"+tab(n)+"*"; }
tst_status(1);$
