LIB "tst.lib"; tst_init();
LIB "findifs.lib";
" EXAMPLE:"; 
ring r = (0,dh,dt),(Tx,Tt),dp;
poly P = (4*dh^2-dt)*Tx^3*Tt + dt*dh*Tt^2 + dh*Tt;
decoef(P,dt);
decoef(P,dh);
tst_status(1);$
