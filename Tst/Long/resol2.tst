LIB "tst.lib";
tst_init();

LIB"zeta.lib";

ring r = 0,(x(1..3)),dp(3);            //A10                 23 sec  30 Karten
ideal J=x(3)^2+x(1)^2+x(2)^11;                                        
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r; 
                            "//=====A10                     2       7";

ring r = 0,(x(1..3)),dp(3);            //A15                 20 sec  27 Karten
ideal J=x(3)^2+x(1)^2+x(2)^16;                                       
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r; 
                            "//=====A15                     3      10";

ring r = 0,(x(1..3)),dp(3);            //A19                 25 sec  33 Karten
ideal J=x(3)^2+x(1)^2+x(2)^20;                                             
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r; 
                            "//=====A19                    7      12";

ring r = 0,(x(1..3)),dp(3);            //E6                  17 sec  21 Karten
ideal J=x(1)^3-x(2)^4+x(3)^2;                                           
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;
                             "//=====E6                     2       8";

ring r = 0,(x(1..3)),dp(3);            //E7                  41 sec  41 Karten
ideal J=x(1)^3-x(1)*x(2)^3+x(3)^2;                                          
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r; 
                            "//=====E7                    11      25";


ring r = 0,(x(1..3)),dp(3);            //E8                  49 sec  51 Karten
ideal J=x(1)^3-x(2)^5+x(3)^2;                                            
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r; 
                            "//=====E8                    14      28";

ring r = 0,(x(1..3)),dp(3);
ideal J=(x(1)*x(2)-x(3)^2)^2+x(1)^3*x(3)+x(2)^5;
list L=resolve(J);intersectionDiv(L);kill L;kill r;    
                          "//=====Jonny Wahl                   9    10  ";     


ring r = 0,(x(1..3)),dp(3);               //D4               20 sec  20 Karten 
ideal J=x(1)^2*x(2)+x(2)^3+x(3)^2;                                 
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r; 
                               "//=====D4                  4      11";

ring r = 0,(x(1..3)),dp(3);               //D7               26 sec  31 Karten 
ideal J=x(1)^2*x(2)+x(2)^6+x(3)^2;                                        
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;   
                             "//=====D7                  5      16";

ring r = 0,(x(1..3)),dp(3);               //D10              51 sec  53 Karten 
ideal J=x(1)^2*x(2)+x(2)^9+x(3)^2;                                             
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;  
                              "//=====D10                 15      29 ";     

ring r = 0,(x(1..3)),dp(3);               //D11              46 sec  51 Karten 
ideal J=x(1)^2*x(2)+x(2)^10+x(3)^2;                                      
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====D11                 15      28 ";    

ring r = 0,(x(1..3)),dp(3);               //D15              64 sec  71 Karten 
ideal J=x(1)^2*x(2)+x(2)^14+x(3)^2;                                            
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;   
                             "//=====D15                 51      40 ";    

ring r = 0,(x(1..3)),dp(3);               //Cayley cubic      5 sec   6 Karten 
ideal J=x(1)*x(2)+x(1)*x(3)+x(2)*x(3)+x(1)*x(2)*x(3);                          
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;   
                             "//=====Cayley cubic              0       3 ";  

ring r = 0,(x(1..3)),dp(3);               //P8                4 sec   6 Karten
ideal J=x(1)^3+x(2)^3+x(1)*x(2)*x(3)+x(3)^3;                                   
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;     
                           "//=====P8                  0       3       "; 


ring r = 0,(x(1..3)),dp(3);               //X9                  nicht  
ideal J=x(1)^4+x(1)^2*x(2)^2+x(2)^4+x(3)^2;                                  
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//===== X9                 0       2      ";  

ring r = 0,(x(1..3)),dp(3);               //J10                 nicht 
ideal J=x(2)^6+x(1)^2*x(2)^2+x(1)^3+x(3)^2;                                    
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====J10                  2       6       "; 

ring r = 0,(x(1..3)),dp(3);               //T3,4,5           19 sec  17 Karten 
ideal J=x(3)^5+x(2)^4+x(1)^3+x(1)*x(2)*x(3);                              
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====T3,4,5                  1       3      ";

ring r = 0,(x(1..3)),dp(3);               //E12                  Fehler 
ideal J=x(2)^7+x(1)*x(2)^5+x(1)^3+x(3)^2;                                
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;     
                           "//=====E12                   5     10       "; 

ring r = 0,(x(1..3)),dp(3);               //E13                  Fehler 
ideal J=x(2)^8+x(1)*x(2)^5+x(1)^3+x(3)^2;                            
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;     
                           "//=====E13                   2     11        ";

ring r = 0,(x(1..3)),dp(3);               //E14               nicht (memory)  
ideal J=x(2)^8+x(1)*x(2)^6+x(1)^3+x(3)^2;                             
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;     
                           "//===== E14                 4     10        ";

ring r = 0,(x(1..3)),dp(3);               //Z11                  Fehler  
ideal J=x(1)*x(2)^4+x(2)^5+x(1)^3*x(2)+x(3)^2;                            
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====Z11                   3      7        ";

ring r = 0,(x(1..3)),dp(3);               //Z12                  Fehler  
ideal J=x(1)^2*x(2)^3+x(1)*x(2)^4+x(1)^3*x(2)+x(3)^2;                  
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====Z12                   1      7       "; 

ring r = 0,(x(1..3)),dp(3);               //Z13                  nicht 
ideal J=x(1)*x(2)^5+x(2)^6+x(1)^3*x(2)+x(3)^2;                                 
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====Z13                  2       7        ";

ring r = 0,(x(1..3)),dp(3);               //W12                  nicht
ideal J=x(1)^2*x(2)^3+x(2)^5+x(1)^4+x(3)^2;                                    
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//===== W12                 2       6    ";

ring r = 0,(x(1..3)),dp(3);               //W13                  nicht 
ideal J=x(2)^6+x(1)*x(2)^4+x(1)^4+x(3)^2;                                      
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====W13                   1       7        ";

ring r = 0,(x(1..3)),dp(3);               //Q10              34 sec  34 Karten 
ideal J=x(1)*x(2)^3+x(2)^4+x(1)^3+x(2)*x(3)^2;                                 
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;   
                             "//=====Q10                   3      7      ";

ring r = 0,(x(1..3)),dp(3);               //Q11              15 sec  18 Karten 
ideal J=x(3)^5+x(1)*x(3)^3+x(1)^3+x(2)^2*x(3);              
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;   
                             "//=====Q11                  0       8       "; 

ring r = 0,(x(1..3)),dp(3);               //Q12              20 sec  23 Karten 
ideal J=x(1)*x(2)^4+x(2)^5+x(1)^3+x(2)*x(3)^2;          
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;   
                             "//=====Q12                  2       7       ";

ring r = 0,(x(1..3)),dp(3);               //S11             1717 sec 41 Karten 
ideal J=x(1)^4+x(1)^3*x(3)+x(2)^2*x(3)+x(1)*x(3)^2;           
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====S11                    2      7        ";

ring r = 0,(x(1..3)),dp(3);               //S12              22 sec  22 Karten 
ideal J=x(3)^5+x(1)*x(3)^3+x(1)^2*x(2)+x(2)^2*x(3);                
list L=resolve(J);intersectionDiv(L);spectralNeg(L);kill L;kill r;    
                            "//=====S12                   2      8    ";

tst_status(1);$
