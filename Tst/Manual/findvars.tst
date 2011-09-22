LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring s  = 0,(e,f,x,y,t,u,v,w,a,d),dp;
ideal i = w2+f2-1, x2+t2+a2-1;
findvars(i);
findvars(i,1);
tst_status(1);$
