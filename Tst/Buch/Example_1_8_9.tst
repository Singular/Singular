LIB "tst.lib";
tst_init();

ring A =0,(x,y,z),dp;
ideal I=x5,xy3,y7,z3+xyz;
poly f =x+y+z;

ring B =0,(t,x,y,z),dp; 
ideal I=imap(A,I);
poly f =imap(A,f);
I=I,1-t*f;
std(I);

LIB"primdec.lib";
setring A;  
radical(I);

tst_status(1);$
