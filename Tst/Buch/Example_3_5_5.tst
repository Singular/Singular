LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),dp;
ideal I=yz,xz;
indepSet(std(I));

indepSet(std(I),1);

ring A  = 0,(x,y,t),dp;
ideal I = y2-x3-3t2x2;
indepSet(std(I),1);          

LIB"surf.lib";
//plot(lead(I));
//plot(I);

tst_status(1);$
