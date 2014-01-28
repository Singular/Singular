// bug_526.tst
LIB "tst.lib";
tst_init();
LIB "sing.lib";

ring rng = 0,x,dp;
ideal i = 0 ;
ASSUME( 0, idealsEqual( ideal(1) , slocus(i) ) );


tst_status(1);$
