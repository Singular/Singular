LIB "tst.lib";
tst_init();

ring A=0,(x,y,z),dp;
ideal I1=x,y;
ideal I2=y2,z;
intersect(I1,I2);    

ring B=0,(t,x,y,z),dp;
ideal I1=imap(A,I1);
ideal I2=imap(A,I2);
ideal J=t*I1+(1-t)*I2;
eliminate(J,t);

tst_status(1);$
