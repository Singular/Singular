LIB "tst.lib";
tst_init();

ring A=0,(x,y),dp;
poly f,f1,f2=x4-y4,x2+y2,x2-y2;
LIB"algebra.lib";
inSubring(f,ideal(f1,f2)); 

ring B = 0,(x,y,u,v,w),(dp(2),dp(1),dp(2));
ideal H=u-imap(A,f),v-imap(A,f1),w-imap(A,f2);
std(H);

ring C=0,(x,y,v,w),(dp(2),dp(2));     
ideal H=v-imap(A,f1), w-imap(A,f2);
poly f=imap(A,f);
reduce(f,std(H));

tst_status(1);$
