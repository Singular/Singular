LIB "tst.lib"; tst_init();
LIB "sing.lib";
int p      = printlevel;
printlevel = 1;
ring r     = 32003,(x,y,z),(c,ds);
ideal i    = xy,xz,yz;
module T   = T_1(i);
vdim(T);                      // Tjurina number = dim_K(T_1), should be 3
list L=T_1(i,"");
module kB  = kbase(L[1]);
print(L[2]*kB);               // basis of 1st order miniversal deformation
show(L[2]);                   // presentation of normal bundle
print(L[3]);                  // relations of i
print(transpose(L[3])*L[2]);  // should be 0 (mod i)
printlevel = p;
tst_status(1);$
