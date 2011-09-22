LIB "tst.lib"; tst_init();
LIB "primdec.lib";
ring  r = 0,(x,y,z),lp;
module M = x2-y2,z3;
Ann(M);
M = [1,x2],[y,x];
Ann(M);
qring Q=std(xy-1);
module M=imap(r,M);
Ann(M);
tst_status(1);$
