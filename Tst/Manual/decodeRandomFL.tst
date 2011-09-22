LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
// correcting one error for one random binary code of length 25,
// redundancy 14; 10 words are processed
decodeRandomFL(25,14,2,1,1,1,10,"");
tst_status(1);$
