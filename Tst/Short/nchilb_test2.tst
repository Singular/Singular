LIB "tst.lib";
tst_init();
LIB"ncHilb.lib";
    ring r=0,(x,y,z),Dp;

    module p1=[1,y,z],[-1,z,y];
    module p2=[1,x,z,x],[-1,z,x,z];
    module p3=[1,x,z,z,x,z],[-1,z,x,z,z,x];
    module p4=[1,x,z,z,z,x,z];[-1,z,x,z,z,x,x];
    list l2=list(p1,p2,p3,p4);

    nchilb(l2,6,1);
tst_status(1);$
