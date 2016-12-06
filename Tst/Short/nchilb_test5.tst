LIB "tst.lib";
tst_init();
LIB"ncHilb.lib";
 ring r=0,(x,y,z),dp;

    module p1=[1,y,z,z];
    module p2=[1,y,y,z];
    module p3=[1,x,z,z];
    module p4=[1,x,z,y];
    module p5=[1,x,y,z];
    module p6=[1,x,y,y];
    module p7=[1,x,x,z];
    module p8=[1,x,x,y];
    module p9=[1,y,z,y,z];
    module p10=[1,y,z,x,z];
    module p11=[1,y,z,x,y];
    module p12=[1,x,z,x,z];
    module p13=[1,x,z,x,y];
    module p14=[1,x,y,x,z];
    module p15=[1,x,y,x,y];
    module p16=[1,y,z,y,x,z];
    module p17=[1,y,z,y,x,y];
    module p18=[1,y,z,y,y,x,z];
    module p19=[1,y,z,y,y,x,y];
    module p20=[1,y,z,y,y,y,x,z];
    module p21=[1,y,z,y,y,y,x,y];

    list l5=list(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,
    p14,p15,p16,p17,p18,p19,p20,p21);
    nchilb(l5,7,1,2,3);
tst_status(1);$
