LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: spcurve.tst,v 1.1 1999-07-21 10:08:58 Singular Exp $");
LIB "spcurve.lib";
example isCMcod2;
example CMtype;
example matrixT1;
example semiCMcod2;
example discr;
example qhmatrix;
example relweight;
example posweight;
example mod2id;
example id2mod;
example subrInterred;
intvec watchProgress=0,1,1,1,1;
example KSpencerKernel;
tst_status(1);$
