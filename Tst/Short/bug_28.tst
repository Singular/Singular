LIB "tst.lib";
tst_init();

ring R = (0,a,b,c,d,t),x,dp;
ideal i = b2-6bc-5c2+bt-ct-b-c+t-1;
i;

ring R1=0,(a,b,c,d,t),dp;
ideal i =  imap(R,i);
i;
ring S = 32003,(a,b,c,d,t),dp;
ideal i = imap(R,i);
i;
imap(R1,i);

tst_status(1);$

