LIB "tst.lib"; tst_init();
LIB "general.lib";
ring r=0,(x,y),dp;
poly p=((x2+y3)^2+xy6)*((x3+y2)^2+x10y);
p=p^2;
//timeFactorize(p,2);
//timeFactorize(p,20);
tst_status(1);$
