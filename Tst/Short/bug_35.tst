LIB "tst.lib";
tst_init();

ring r=2,(x,y),wp(4,7);
ideal K=xy2+x4+x2,x6+y3+x4+x2+1,y4+x4y+x2y;
LIB"primdec.lib";
ideal J=radical(K);
J;
option(redSB);
ring R=2,(x,y),wp(4,7);
ideal I=y2+x3+x,x4+x2y+y+1;
I=std(I);
ideal F=finduni(I);
F;
reduce(F,I);

//
tst_status(1);$
