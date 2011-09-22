LIB "tst.lib"; tst_init();
LIB "ring.lib";
ring r=0,(x,y,z),ds;
show(r);"";
// blocksize is derived from no of vars:
int t=5;
def R1=extendring(t,"a","dp");         //t global: "dp" -> "dp(5)"
show(R1); setring R1; "";
def R2=extendring(4,"T(","c,dp",1,r);    //"dp" -> "c,..,dp(4)"
show(R2); setring R2; "";
// no intvec given, blocksize given: given blocksize is used:
def R3=extendring(4,"T(","dp(2)",0,r);   // "dp(2)" -> "dp(2)"
show(R3); setring R3; "";
// intvec given: weights and blocksize is derived from given intvec
// (no specification of a blocksize in the given ordstr is allowed!)
// if intvec does not cover all given blocks, the last block is used
// for the remaining variables, if intvec has too many components,
// the last ones are ignored
intvec v=3,2,3,4,1,3;
def R4=extendring(10,"A","ds,ws,Dp,dp",v,0,r);
// v covers 3 blocks: v[1] (=3) : no of components of ws
// next v[1] values (=v[2..4]) give weights
// remaining components of v are used for the remaining blocks
show(R4);
kill r,R1,R2,R3,R4;
tst_status(1);$
