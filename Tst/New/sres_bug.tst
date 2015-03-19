LIB "tst.lib"; tst_init();

ring r = (0,w),(Y,z),(dp);
ideal H = Y*z + Y;

qring q = std(H);


basering;
ideal R = ( (w/16)*z );
sres(R, 1);

R = std( R );
sres(R, 1);

tst_status(1);$




