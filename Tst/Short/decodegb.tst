LIB "tst.lib";
tst_init();

LIB"decodegb.lib";
example sysCRHT;
example sysCRHTMindist;
example sysNewton;
example sysBin;
example encode;
example syndrome;
example sysQE;
example errorInsert;
example errorRand;
example randomCheck;
example genMDSMat;
example mindist;
example decode;
//example decodeRandom; // no timings allowed in tst files
//example decodeCode; // no timings allowed in tst files
example vanishId;
example sysFL;
//example decodeRandomFL; // no timings allowed in tst files

tst_status(1);$
