LIB "tst.lib";
tst_init();
tst_ignore("CVS ID $Id: spcurve.tst,v 1.2 2000-12-12 14:02:30 anne Exp $");
LIB "spcurve.lib";
example isCMcod2;
example CMtype;
example matrixT1;
example semiCMcod2;
example discr;
example qhmatrix;
example relweight;
example posweight;
intvec watchProgress=0,1,1,1,1;
example KSpencerKernel;
tst_status(1);$
