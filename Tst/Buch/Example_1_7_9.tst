LIB "tst.lib";
tst_init();

ring A  = 0,(x,y),dp;   
ideal I = x10+x9y2,y8-x2y7;
ideal J = std(I);
J;

ring A1 = 0,(x,y),lp;  
ideal I = fetch(A,I);
ideal J = std(I);
J;

ring B  = 0,(x,y),ds;   
ideal I = fetch(A,I);
ideal J = std(I);
J;

ring B1 = 0,(x,y),ls;   
ideal I = fetch(A,I);
ideal J = std(I);
J;

intmat O[3][3]=1,1,1,0,-1,-1,0,0,-1;
ring  C = 0,(t,x,y),M(O);     
ideal I = homog(imap(A,I),t); 
                              
ideal J = std(I);            
      J = subst(J,t,1);
J;


tst_status(1);$
