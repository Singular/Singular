LIB "tst.lib"; tst_init();
LIB "general.lib";
primecoeffs(intvec(7*8*121,7*8));"";
ring r = 0,(b,c,t),dp;
ideal I = -13b6c3t+4b5c4t,-10b4c2t-5b4ct2;
primecoeffs(I);
tst_status(1);$
