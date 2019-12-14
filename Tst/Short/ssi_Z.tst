LIB "tst.lib";
tst_init();

link l1="ssi:w ss1.ssi";
ring r1=ZZ,x,dp;
poly p=x+11;
write(l1,p);
ring r2=ZZ/22,x,dp;
poly p=x+11;
write(l1,p);
close(l1);
link l2="ssi:r ss1.ssi";
def h1=read(l2);
h1;
basering;
def h2=read(l2);
h2;
basering;
listvar();
close(l2);

ring r3=flintQp("bb"),x,dp;
poly p=x+11+bb;
p;
write(l1,p);
ring r4=flintZn(29,"a"),x,dp;
poly p=x+11+a;
p;
write(l1,p);
close(l1);
def h3=read(l2);
h3;
basering;
def h4=read(l2);
h4;
basering;
close(l2);

tst_status(1);$
