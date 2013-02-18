LIB "tst.lib"; tst_init();

ring R_loc = 0, (x,y), ds;
poly f = 2x2-y3;
ring Rt_loc = (0,t), (x,y), ds;
setring Rt_loc;
poly F = imap(R_loc,f)-(t/2)*y2;
poly f_def = subst(F,t,1/10);

tst_status(1);$
