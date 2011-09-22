LIB "tst.lib"; tst_init();
LIB "hnoether.lib";
int p=printlevel; printlevel=-1;
ring r=0,(x,y),dp;
list hne1=develop(x);
list hne2=develop(x+y);
list hne3=develop(x+y2);
separateHNE(hne1,hne2);  // two transversal lines
separateHNE(hne1,hne3);  // one quadratic transform. gives 1st example
printlevel=p;
tst_status(1);$
