LIB "tst.lib";
tst_init();


ring A=0,(x,y,z),(c,dp);
module M=[xy,xz],[x,x];
module N=[y2,z2],[x,x];

M+N;

intersect(M,N);

quotient(M,N);

 quotient(N,M);


qring Q=std(x5);
module M=fetch(A,M);
module Null;
M;

Null;

quotient(Null,M);


tst_status(1);$
