LIB "tst.lib";
tst_init();

// rOppoite
ring r;
def o1=opposite(r);o1;
def o2=opposite(o1);o2;
ring r1=0,(x,y,z),lp;
def o3=opposite(r1);o3;
def o4=opposite(o3);o4;


tst_status(1);$
