LIB "tst.lib";
tst_init();
LIB "integralbasis.lib";

// The Puiseux expansions corresponding to the factor (y7 + x4) are finite
ring r = 0, (x,y), dp;
poly f = (y7 + x4) * (y7 + y6x + x4);
puiseuxList(f,-1,1,2);

// Both classes of expansions have the same initial terms, computation cannot be splitted when computing the singular part.
ring r2 = 0, (x,y), dp;
poly f = (y7 + x4) * (y7 + y6x + x4) + y30;
puiseuxList(f,-1,1,2);

// Finally statistics...
tst_status(1);$
