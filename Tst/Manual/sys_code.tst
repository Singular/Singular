LIB "tst.lib"; tst_init();
LIB "brnoeth.lib";
ring s=3,T,lp;
matrix C[2][5]=0,1,0,1,1,0,1,0,0,1;
print(C);
list L=sys_code(C);
L[3];
// here is the generator matrix in standard form
print(L[1]);
// here is the control matrix in standard form
print(L[2]);
// we can check that both codes are dual to each other
print(L[1]*transpose(L[2]));
tst_status(1);$
