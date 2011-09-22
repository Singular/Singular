LIB "tst.lib"; tst_init();
LIB "normaliz.lib";
setNmzFilename("VeryInteresting");
intmat sgr[3][3]=1,2,3,4,5,6,7,8,10;
intmat sgrnormal=normaliz(sgr,0);
readNmzData("cst");
readNmzData("typ");
tst_status(1);$
