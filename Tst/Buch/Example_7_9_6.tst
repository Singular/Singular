LIB "tst.lib";
tst_init();

ring A=(2,a),(x,y),(c,dp);
qring B=std(x2+y2+a);
ideal I=x2+a,y;
print(mres(I,2));

setring A;
qring C=std(x2+y2+1);
ideal I=x+1,y;
print(mres(I,2));

tst_status(1);$
