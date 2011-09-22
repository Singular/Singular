LIB "tst.lib"; tst_init();
LIB "random.lib";
ring r=0,(t,x,y,z),ds;
ideal i= x3+y4,z4+yx,t+x+y+z;
genericid(i,0,10);
module m=[x,0,0,0],[0,y2,0,0],[0,0,z3,0],[0,0,0,t4];
print(genericid(m));
tst_status(1);$
