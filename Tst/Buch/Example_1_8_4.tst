LIB "tst.lib";
tst_init();

ring A =0,(t,x,y,z),dp;
ideal I=t2+x2+y2+z2,t2+2x2-xy-z2,t+y3-z3;

eliminate(I,t);    
                
ring A1=0,(t,x,y,z),(dp(1),dp(3));
ideal I=imap(A,I);
ideal J=std(I);
J;

tst_status(1);$
