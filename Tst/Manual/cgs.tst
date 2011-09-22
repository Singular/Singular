LIB "tst.lib"; tst_init();
LIB "compregb.lib";
ring RingVar=(0,a,b),(x,y,t),lp;
ring RingAll=0,(x,y,t,a,b),(lp(3),dp);
ideal polys=x^3-a,y^4-b,x+y-t;
list vars=x,y,t;
list paras=a,b;
list G = cgs(polys,vars,paras,RingVar,RingAll);
G;
tst_status(1);$
