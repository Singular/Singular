LIB "tst.lib";
tst_init();
option(noredefine);

LIB "ncModslimgb.lib";

ring r=0,(x,y),dp;
poly P=y4+x3+x*y3;
def A=Sannfs(P);
setring A;
ideal bs=LD,imap(r,P);

ideal Iparallel=ncmodslimgb(bs,0,2);
ideal Iserial=ncmodslimgb(bs);
Iparallel[1];
Iserial[1];
Iparallel[1]==Iserial[1];

tst_status(1);$
