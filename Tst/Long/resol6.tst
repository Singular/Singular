//LIB "tst.lib";
//tst_init();

LIB"resol.lib";


//=====W#[1,2]        
ring r = 0,(x(1..2)),dp;                           
ideal J=(x(2)^2-x(1)^3)^2-4*x(1)^5*x(2)-x(1)^7;                         
list L=resolve(J,1,"L");size(L[1]);kill L;kill r; 
                                      
//=====arti1      
ring r = 0,(x(1..2)),dp;                        
ideal J=(x(1)^2-x(2)^3)*((x(1)-1)^2+(x(2)-1)^3); 
list L=resolve(J,1,"L");size(L[1]);kill L;kill r;
                                        
//=====arti2        
ring r = 0,(x(1..2)),dp;                          
ideal J=(x(1)^2-x(2)^3)*((x(1)-1)^2-(x(2)-1)^3); 
list L=resolve(J,1,"L");size(L[1]);kill L;kill r;
                                        
ring r = 0,(x(1..3)),dp(3);            //Whitney umbrella    11 sec  12 Karten
ideal J=(x(3)^2-x(2)*x(1)^2)*(x(1)-1);  
list L=resolve(J,1,"L");size(L[1]);kill L;kill r;

ring r = 0,(x(1..3)),dp(3);            //Villamayor           7 sec   9 Karten
ideal J=(x(3)^2-x(2)^2*x(1)^2)*(x(1)-1);  
list L=resolve(J,1,"L");size(L[1]);kill L;kill r;

ring r = 0,(x(1..3)),dp(3);            //E6       
ideal J=(x(1)^3-x(2)^4+x(3)^2)*(x(1)-1);   
list L=resolve(J,1,"L");size(L[1]);kill L;kill r;

ring r = 0,(x(1..3)),dp(3);            //E8                  49 sec  51 Karten
ideal J=(x(1)^3-x(2)^5+x(3)^2)*(x(1)-1);   
//list L=resolve(J,1,"L");size(L[1]);kill L;kill r;
//!!! Laeuft noch nicht durch --- zu dick

/* nicht Aequidim.
ring r = 0,(x(1..3)),dp(3);            //U8                  10 sec  11 Karten
ideal J=(x(3)^3+x(1)^2+x(2)*x(3),x(1)*x(2))*(x(1)-1)^2;   
list L=resolve(J,1,"L");size(L[1]);kill L;kill r;
*/

//=====W#[1,2]        
ring r = 0,(x(1..3)),dp;                           
ideal J=(x(2)^2-x(1)^3)^2-4*x(1)^5*x(2)-x(1)^7-x(3)^2;                         
list L=resolve(J,1,"L");size(L[1]);kill L;kill r; 
//tst_status(1);$

//=====arti1      
ring r = 0,(x(1..3)),dp;                        
ideal J=(x(1)^2-x(2)^3)*((x(1)-1)^2+(x(2)-1)^3)-x(3)^2; 
list L=resolve(J,1,"L");size(L[1]);kill L;kill r;
                                        
//=====arti2        
ring r = 0,(x(1..3)),dp;                          
ideal J=(x(1)^2-x(2)^3)*((x(1)-1)^2-(x(2)-1)^3)-x(3)^2; 
list L=resolve(J,1,"L");size(L[1]);kill L;kill r;
                                        

