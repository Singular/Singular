LIB "tst.lib"; tst_init();
LIB "ainvar.lib";
ring q=0,(x,y,z,u,v,w),dp;
poly p=x2yz-x2v;
ideal dom =x-w,u2w+1,yz-v;
reduction(p,dom);
reduction(p,dom,w);
tst_status(1);$
