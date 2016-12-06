LIB "tst.lib";
tst_init();
LIB"ncHilb.lib";

    ring r=0,(a,b),dp;
    module p1=[1,a,a,a];
    module p2=[1,a,b,b];
    module p3=[1,a,a,b];

    list l3=list(p1,p2,p3);
    nchilb(l3,5,2);
tst_status(1);$
