LIB "tst.lib";
tst_init();

ring A1 = 0,(x,y,z),lp;     
poly f = x3yz + y5 + z4 + x3 + xy2; f;

ring A2 = 0,(x,y,z),dp;      
poly f = imap(A1,f); f;

ring A3 = 0,(x,y,z),Dp;       
poly f = imap(A1,f); f;

ring A4 = 0,(x,y,z),Wp(5,3,2); 
poly f = imap(A1,f); f;

ring A5 = 0,(x,y,z),ls;      
poly f = imap(A1,f); f;

ring A6 = 0,(x,y,z),ds;    
poly f = imap(A1,f); f;

ring A7 = 0,(x,y,z),Ws(5,3,2); 
poly f = imap(A1,f); f;

ring A8 = 0,(x,y,z),(dp(1),ds(2)); 
poly f = imap(A1,f); f;

intmat A[3][3] = -1, -1, -1, 0, 0, 1, 0, 1, 0;
print(A);
ring A9 = 0,(x,y,z),M(A);
poly f = imap(A1,f); f;


tst_status(1);$
