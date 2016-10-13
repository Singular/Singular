LIB "tst.lib";
tst_init();
LIB"ncHilb.lib";

    ring r=0,(x,y,z,w),dp;
    module p1=[1,y,x],[-1,x,y];
    module p2=[1,z,x],[-1,x,z];
    module p3=[1,w,x],[-1,x,w];
    module p4=[1,z,y],[-1,y,z];
    module p5=[1,w,y],[-1,y,w];
    module p6=[1,w,z],[-1,z,w];
    list l2=list(p1,p2,p3,p4,p5,p6);
    nchilb(l2,5);
tst_status(1);$
