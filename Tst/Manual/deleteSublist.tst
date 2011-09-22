LIB "tst.lib"; tst_init();
LIB "general.lib";
list l=1,2,3,4,5;
intvec v=1,3,4;
l=deleteSublist(v,l);
l;
tst_status(1);$
