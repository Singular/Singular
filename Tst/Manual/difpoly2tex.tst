LIB "tst.lib"; tst_init();
LIB "findifs.lib";
ring r = (0,dh,dt,V),(Tx,Tt),dp;
poly M = (4*dh*Tx+dt)^2*(Tt-1) + V*Tt*Tx;
ideal I = decoef(M,dt);
list L; L[1] = V;
difpoly2tex(I,L);
poly G = V*dh^2*(Tt-Tx)^2;
difpoly2tex(I,L,G);
tst_status(1);$
