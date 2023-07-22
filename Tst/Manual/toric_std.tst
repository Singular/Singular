LIB "tst.lib"; tst_init();
LIB "toric.lib";
ring r=0,(x,y,z),wp(3,2,1);
// call with toric ideal (of the matrix A=(1,1,1))
ideal I=x-y,x-z;
ideal J=toric_std(I);
J;
// call with the same ideal, but badly chosen generators:
// 1) not only binomials
I=x-y,2x-y-z;
J=toric_std(I);
// 2) binomials whose monomials are not relatively prime
I=x-y,xy-yz,y-z;
J=toric_std(I);
J;
// call with a non-toric ideal that seems to be toric
I=x-yz,xy-z;
J=toric_std(I);
J;
// comparison with real standard basis and saturation
ideal H=std(I);
H;
LIB "elim.lib";
sat_with_exp(H,xyz);
tst_status(1);$
