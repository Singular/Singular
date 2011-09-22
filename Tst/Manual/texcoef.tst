LIB "tst.lib"; tst_init();
LIB "findifs.lib";
" EXAMPLE:"; 
ring r = (0,dh,dt),(Tx,Tt),dp;
number n1,n2,n3 = dt/(4*dh^2-dt),(dt+dh)^2, 1/dh;
n1; texcoef(n1);
n2; texcoef(n2);
n3; texcoef(n3);
tst_status(1);$
