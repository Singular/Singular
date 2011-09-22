LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring r= 0, (a,b,c,d,e,f),dp;
matrix A1[6][6] = 0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0;
matrix A2[6][6] = 0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0;
list L = primary_invariants(A1,A2);
matrix IS = irred_secondary_char0(L[1],L[2],L[3],0);
IS;
tst_status(1);$
