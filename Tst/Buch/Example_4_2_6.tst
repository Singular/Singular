LIB "tst.lib";
tst_init();

option(redSB);     
ring R=0,(x,y),lp;
ideal I=(y2-1)^2,x2-(y+1)^3;

map phi=R,x,x+y;     
map psi=R,x,-x+y;     
I=std(phi(I));
I;

factorize(I[1]);

ideal Q1=std(I,(y2-2y-7)^2); 
ideal Q2=std(I,(y+1)^3);    
Q1;
Q2;

factorize(Q1[1]);   

factorize(Q2[1]);   

Q1=std(psi(Q1));    
Q2=std(psi(Q2));
Q1;
Q2;

tst_status(1);$
