
LIB "tst.lib";
tst_init();

LIB"zeta.lib";


ring r = 0,(x(1..3)),dp(3);               //U12               38 sec 33 Karten 
ideal J=x(1)*x(2)*x(3)^2+x(3)^4+x(1)^3+x(2)^3;             
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;     
                           "//=====U12                   2      7        ";

ring r = 0,(x(1..3)),dp(3);               //J3,0                nicht (memory)
ideal J=x(2)^9+x(1)^2*x(2)^3+x(1)^3+x(3)^2;            
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//===== J3,0                 12     14     ";

ring r = 0,(x(1..3)),dp(3);               //J3,1                 nicht
ideal J=x(2)^10+x(1)^2*x(2)^3+x(1)^3+x(3)^2;           
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====J3,1                     12     22   ";  

ring r = 0,(x(1..3)),dp(3);               //Z1,0        Fehler (nach 15467 sec)
ideal J=x(2)^7+x(1)^2*x(2)^3+x(1)^3*x(2)+x(3)^2;              
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//===== Z1,0                  10      11  ";  

ring r = 0,(x(1..3)),dp(3);               //S1,1                   Fehler 
ideal J=x(2)^6+x(1)^2*x(2)^2+x(1)^2*x(3)+x(2)*x(3)^2;           
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;     
                           "//=====S1,1                       6       13";

ring r = 0,(x(1..3)),dp(3);               //U1,0            4277 sec 38 Karten 
ideal J=x(1)*x(2)^3+x(2)^3*x(3)+x(1)^3+x(1)*x(3)^2;                            
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;   
                             "//=====U1,0                      1        6";

ring r = 0,(x(1..3)),dp(3);               // E18               nicht (memory) 
ideal J=x(2)^10+x(1)*x(2)^7+x(1)^3+x(3)^2;                            
list L=resolve(J);spectralNeg(L);kill L;kill r;     
                           "//=====  E18                     7       14  "; 

ring r = 0,(x(1..3)),dp(3);               //W17                   Fehler 
ideal J=x(2)^7+x(1)*x(2)^5+x(1)^4+x(3)^2;                
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;     
                           "//=====W17                       7      16 ";

ring r = 0,(x(1..3)),dp(3);               //U16              29 sec  26 Karten 
ideal J=x(2)^5+x(1)^2*x(2)^2+x(1)^3-x(1)*x(3)^2;          
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;     
                           "//=====U16                   8      13";
tst_status(1);$