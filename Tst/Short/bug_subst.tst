LIB "tst.lib";
tst_init();
bigint m=memory(0);
ring r=0,(x,y),dp;
poly p=(x+y)^10;
poly q;
q = subst(p,x,x+y); 
ideal P=p;
ideal Q=subst(P,x,x+y);
kill r;
memory(0)-m;

tst_status(1);$
