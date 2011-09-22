LIB "tst.lib"; tst_init();
LIB "cisimplicial.lib";
intmat A[3][4] = 10,3,2,1,2,1,1,3,5,0,1,2;
print(A);
intvec v = 23,12,10;
belongSemigroup(v,A);
"// A * (1,3,1,2) = v";
belongSemigroup(v,A,3);
"// v is not a combination of the first 3 columns of A";
intvec w = 12,4,1;
belongSemigroup(w,A);
"// w is not a combination of the columns of A";
tst_status(1);$
