LIB "tst.lib";
tst_init();

option(redSB);         
ring R=0,(x,y),lp;            
ideal a1=x;             
ideal a2=y2+2y+1,x-1;
ideal a3=y2-2y+1,x-1;
ideal I=intersect(a1,a2,a3);
poly h=y4-2y2+1;   

ideal I2=std(I,h);

indepSet(I2);

list fac=factorize(I2[1]);
fac;

ideal J1=std(I2,(y+1)^2);      
ideal J2=std(I2,(y-1)^2);     

J1;
J2;

map phi=R,x,x+y;
map psi=R,x,-x+y;              

ideal K1=std(phi(J1));
ideal K2=std(phi(J2));
factorize(K1[1]);

ideal K11=std(K1,(y+1)^2);      
                                 
ideal K12=std(K1,(y+2)^2);        
factorize(K2[1]);

ideal K21=std(K2,(y-1)^2);       
                               
ideal K22=std(K2,y2);            
K11=std(psi(K11));               
                                   
K12=std(psi(K12));
K21=std(psi(K21));
K22=std(psi(K22));
K11;                           
K12;
K21;
K22;

tst_status(1);$
