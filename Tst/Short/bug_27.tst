LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),ds;
ideal i=zx+y3,z+y3,z+xy;
interred(i); 

tst_status(1);$

