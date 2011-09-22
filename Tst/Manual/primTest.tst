LIB "tst.lib"; tst_init();
LIB "mprimdec.lib";
ring r=0,(x,y,z),lp;
ideal i=x+1,y-1,z;
i=std(i);
ideal primId=primTest(i,z);
primId;
i=x,z2,yz,y2;
i=std(i);
primId=primTest(i);
primId;
tst_status(1);$
