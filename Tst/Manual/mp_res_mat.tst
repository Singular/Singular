LIB "tst.lib"; tst_init();
LIB "solve.lib";
// compute resultant matrix in ring with parameters (sparse resultant matrix)
ring rsq= (0,u0,u1,u2),(x1,x2),lp;
ideal i= u0+u1*x1+u2*x2,x1^2 + x2^2 - 10,x1^2 + x1*x2 + 2*x2^2 - 16;
module m = mp_res_mat(i);
print(m);
// computing sparse resultant
det(m);
// compute resultant matrix (Macaulay resultant matrix)
ring rdq= (0,u0,u1,u2),(x0,x1,x2),lp;
ideal h=  homog(imap(rsq,i),x0);
h;
module m = mp_res_mat(h,1);
print(m);
// computing Macaulay resultant (should be the same as above!)
det(m);
// compute numerical sparse resultant matrix
setring rsq;
ideal ir= 15+2*x1+5*x2,x1^2 + x2^2 - 10,x1^2 + x1*x2 + 2*x2^2 - 16;
module mn = mp_res_mat(ir);
print(mn);
// computing sparse resultant
det(mn);
tst_status(1);$
