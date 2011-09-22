LIB "tst.lib"; tst_init();
LIB "inout.lib";
ring S;
matrix M=matrix(freemodule(2),3,3);
int ALLprint; export ALLprint;
allprint("M =",M);
kill ALLprint;
tst_status(1);$
