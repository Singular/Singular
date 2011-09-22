LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
ring r=0,(x,y),dp;
list Hne=hnexpansion((x6-y10)*(x+y2-y3)*(x+y2+y3));
multsequence(Hne[1]),"  |  ",multsequence(Hne[2]),"  |  ",
multsequence(Hne[3]),"  |  ",multsequence(Hne[4]);
multsequence(Hne);
// The meaning of the entries of the 2nd matrix is as follows:
displayMultsequence(Hne);
tst_status(1);$
