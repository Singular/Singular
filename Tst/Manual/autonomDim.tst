LIB "tst.lib"; tst_init();
LIB "control.lib";
// Cauchy1 example
ring r=0,(s1,s2,s3,s4),dp;
module R= [s1,-s2],
[s2, s1],
[s3,-s4],
[s4, s3];
R=transpose(R);
view( R );
view( autonomDim(R) );
tst_status(1);$
