LIB "tst.lib";
tst_init();

// singclap_pmod
ring r=QQ,x,dp;
(x+1) % x;
(x+1)^2 %(x+1);
(x+1) mod x;
(x+1)^2 mod(x+1);
ring r2=ZZ,x,dp;
(x+1) % x;
(x+1)^2 %(x+1);
(x+1) mod x;
(x+1)^2 mod(x+1);
ring r3=ZZ/5,x,dp;
(x+1) % x;
(x+1)^2 %(x+1);
(x+1) mod x;
(x+1)^2 mod(x+1);

tst_status(1);$
