LIB "tst.lib";
tst_init();

ring A = 0,(a,b,c),lp;
poly f = a+b+ab+c3;

ring B = 0,(x,y,z),dp;
map F  = A, x+y,x-y,z;  
                        
poly g = F(f);          
g;

ring A1 = 0,(x,y,c,b,a,z),dp;
imap(A,f);
fetch(A,f);

tst_status(1);$
