LIB "tst.lib";
tst_init();
LIB"ncHilb.lib";
    ring r=0,(X,Y,Z),dp;
    module w =[1,Y];
    module p1 =[1,Y,Z];
    module p2 =[1,Y,Z,X];
    module p3 =[1,Y,Z,Z,X,Z];
    module p4 =[1,Y,Z,Z,Z,X,Z];
    module p5 =[1,Y,Z,Z,Z,Z,X,Z];
    module p6 =[1,Y,Z,Z,Z,Z,Z,X,Z];
    module p7 =[1,Y,Z,Z,Z,Z,Z,Z,X,Z];
    module p8 =[1,Y,Z,Z,Z,Z,Z,Z,Z,X,Z];
    list l1=list(p1,p2,p3,p4,p5,p6,p7,p8);
    rcolon(l1,w,10);
tst_status(1);$
