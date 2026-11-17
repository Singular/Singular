LIB "tst.lib";
tst_init();

// Exercise invar.lib with the default std algorithm in a separate process,
// so its fixed random seed is independent of the stdhilb regression test.
LIB "invar.lib";

finite(6);
matrix id=unitmat(3);
matrix m3[3][3]=0,1,0,0,0,1,1,0,0;
matrix m2[3][3]=0,1,0,1,0,0,0,0,1;
list a=id,m3,m3*m3,m2,m2*m3,m2*m3*m3;
matrix rep=finiterep(a);
invar(rep);

tst_status(1);$
