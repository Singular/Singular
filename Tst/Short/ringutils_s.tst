LIB "tst.lib";
tst_init();


LIB"ring.lib";

option("warn");
ASSUME(0, optionIsSet("warn") );
option("nowarn");
ASSUME(0, 0 == optionIsSet("warn") );
//----------------------------
ring rng = real,x,dp;
ASSUME(0, 0== isQuotientRing(rng) ) ;

qring qrng = 1;
ASSUME(0, isQuotientRing(qrng) ) ;

ring rng2 = integer,x,dp;
ASSUME(0, 0 == isQuotientRing(rng2) ) ;

qring qrng2=0;
ASSUME(0, not isQuotientRing(qrng2) ) ;

ring rng3 = 0,x,dp;
ASSUME(0, 0 == isQuotientRing(rng3) ) ;

qring qrng3=1;
ASSUME(0, isQuotientRing(qrng3) ) ;
//--------------------------
ring rng = integer,x,dp;
ASSUME(0, hasRingCoefficientRing( rng ) );

qring q = ideal(x);
ASSUME(0, hasRingCoefficientRing( q ) );

ring rng2 = 0,x,dp;
ASSUME(0, 0==hasRingCoefficientRing( rng2 ) );

ring rng3 = (0,a),x,dp;
ASSUME(0, 0==hasRingCoefficientRing( rng3 ) );

ring rng4 = (real,a),x,dp;
ASSUME(0, 0==hasRingCoefficientRing( rng4 ) );

ring rng5 = (real),x,dp;
ASSUME(0, 0==hasRingCoefficientRing( rng5 ) );


tst_status(1); $;

