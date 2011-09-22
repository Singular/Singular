LIB "tst.lib"; tst_init();
LIB "signcond.lib";
ring r = 0,(x,y),dp;
ideal i = (x-2)*(x+3)*x,(y-1)*(y+2)*(y+4);
ideal P = x,y;
list l = signcnd(P,i);
psigncnd(P,l);
tst_status(1);$
