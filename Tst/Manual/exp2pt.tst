LIB "tst.lib"; tst_init();
LIB "findifs.lib";
" EXAMPLE:"; 
ring r = (0,dh,dt),(Tx,Tt),dp;
poly M = (4*dh*Tx^2+1)*(Tt-1)^2;
print(exp2pt(M));
print(exp2pt(M,"F"));
tst_status(1);$
