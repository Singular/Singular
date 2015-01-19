LIB "tst.lib";
tst_init();

//bug in computing ecart with mixe degree orderings
system("alarm",60);
 ring B=0,(x,y,u),(a(1,1,-1),dp);
 poly f=-2x4-2x2y2+5x2y+y3-2y2;
 ideal i=jacob(f);
 ideal k=i,u-f;
 ideal G=std(k);
 G;
 reduce(x2,G);

tst_status(1);$
