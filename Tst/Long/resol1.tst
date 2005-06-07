LIB "tst.lib";
tst_init();

LIB"resolve.lib";

ring r = 0,(x(1..2)),dp;                //A2             
ideal J=x(2)^2-x(1)^3;                                   
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                             "//=====A2                   0        1 ";
        
ring r = 0,(x(1..2)),dp;               //A3                  3 sec    4 Karten 
ideal J=x(2)^2-x(1)^4;                                       
list L=resolve(J,1);size(L[1]);kill L;kill r;
                              "//=====A3                   0        1";

ring r = 0,(x(1..2)),dp;               //E8                  5 sec    6 Karten
ideal J=x(2)^3-x(1)^5;                                  
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====E8                    0        1";

ring r = 0,(x(1..2)),dp;                //W#[1,3]            8 sec    9 Karten
ideal J=(x(2)^2-x(1)^3)^2-x(1)^7;                                   
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                             "//=====W#[1,3]                   0        1";

ring r = 0,(x(1..2)),dp;                //W#[1,14]       29201 sec   20 Karten
ideal J=x(1)*x(2)^11+x(2)^6-2*x(1)^2*x(2)^3+x(1)^4;                    
list L=resolve(J,1);size(L[1]);kill L;kill r;
                               "//=====W#[1,14]                   3        1";

ring r = 0,(x(1..2)),dp;                //W#[1,2]                nicht      
ideal J=(x(2)^2-x(1)^3)^2-4*x(1)^5*x(2)-x(1)^7;                         
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====W#[1,2]                    1        1";

ring r = 0,(x(1..2)),dp;               //arti1                   nicht 
ideal J=(x(1)^2-x(2)^3)*((x(1)-1)^2+(x(2)-1)^3); 
list L=resolve(J,1,"E");size(L[1]);kill L;kill r;
                              "//=====arti1                     2       1";

ring r = 0,(x(1..2)),dp;               //arti2                   nicht 
ideal J=(x(1)^2-x(2)^3)*((x(1)-1)^2-(x(2)-1)^3); 
list L=resolve(J,1,"E");size(L[1]);kill L;kill r;
                              "//=====arti2                      1      1";

ring r = 0,(x(1..3)),dp(3);            //Whitney umbrella    11 sec  12 Karten
ideal J=x(3)^2-x(2)*x(1)^2;                                       
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====Whitney umbrella            2       7";
  
ring r = 0,(x(1..3)),dp(3);            //Villamayor           7 sec   9 Karten
ideal J=x(3)^2-x(2)^2*x(1)^2;                                        
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====Villamayor                0       2";

ring r = 0,(x(1..3)),dp(3);            //A10                 23 sec  30 Karten
ideal J=x(3)^2+x(1)^2+x(2)^11;                                        
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====A10                     2       7";

ring r = 0,(x(1..3)),dp(3);            //A15                 20 sec  27 Karten
ideal J=x(3)^2+x(1)^2+x(2)^16;                                       
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====A15                     3      10";

ring r = 0,(x(1..3)),dp(3);            //A19                 25 sec  33 Karten
ideal J=x(3)^2+x(1)^2+x(2)^20;                                             
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====A19                    7      12";

ring r = 0,(x(1..3)),dp(3);            //E6                  17 sec  21 Karten
ideal J=x(1)^3-x(2)^4+x(3)^2;                                           
list L=resolve(J,1);size(L[1]);kill L;kill r;
                             "//=====E6                     2       8";

ring r = 0,(x(1..3)),dp(3);            //E7                  41 sec  41 Karten
ideal J=x(1)^3-x(1)*x(2)^3+x(3)^2;                                          
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====E7                    11      25";


ring r = 0,(x(1..3)),dp(3);            //E8                  49 sec  51 Karten
ideal J=x(1)^3-x(2)^5+x(3)^2;                                            
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====E8                    14      28";

ring r = 0,(x(1..3)),dp(3);            //                    13 sec  15 Karten
ideal J=x(3)^2-x(2)^3*x(1)^3;                                      
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====                     3       6";

ring r = 0,(x(1..3)),dp(3);            //                    57 sec  61 Karten
ideal J=x(1)^6*x(2)^2+x(3)^3;                                
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====                    19      35";

ring r = 0,(x(1..3)),dp(3);            //S13                 12 sec  15 Karten
ideal J=x(3)^10+x(1)^2+x(2)^2,x(2)*x(3);                                      
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====S13                     1       2 ";       

ring r = 0,(x(1..3)),dp(3);            //T7                  14 sec  17 Karten
ideal J=x(2)^3+x(3)^3+x(1)^2,x(2)*x(3);                                      
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====T7                     1       2  ";      

ring r = 0,(x(1..3)),dp(3);            //T8                 12 sec   15 Karten
ideal J=x(3)^4+x(2)^3+x(1)^2,x(2)*x(3);                                       
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====T8                    0        2 ";       

ring r = 0,(x(1..3)),dp(3);            //T9                  15 sec  18 Karten
ideal J=x(3)^5+x(2)^3+x(1)^2,x(2)*x(3);                                       
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====T9                     1       2 ";       

ring r = 0,(x(1..3)),dp(3);            //U7                  12 sec  14 Karten
ideal J=x(1)^2+x(2)*x(3),x(3)^3+x(1)*x(2);                                     
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                           "//=====U7                     1       2 ";       

ring r = 0,(x(1..3)),dp(3);            //U8                  10 sec  11 Karten
ideal J=x(3)^3+x(1)^2+x(2)*x(3),x(1)*x(2);                                     
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                           "//=====U8                     0       3  ";      

ring r = 0,(x(1..3)),dp(3);            //U9                  11 sec  12 Karten
ideal J=x(1)^2+x(2)*x(3),x(3)^4+x(1)*x(2);                                     
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                           "//=====U9                     1       2  ";      

ring r = 0,(x(1..3)),dp(3);            //W8                  14 sec  16 Karten
ideal J=x(3)^3+x(1)^2,x(2)^2+x(1)*x(3);                                       
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                           "//=====W8                     0       1  ";      

ring r = 0,(x(1..3)),dp(3);            //W9                   9 sec  10 Karten
ideal J=x(2)*x(3)^2+x(1)^2,x(2)^2+x(1)*x(3);                                   
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                           "//=====W9                     1       2 ";      

ring r = 0,(x(1..3)),dp(3);            // Z9                  6 sec   8 Karten
ideal J=x(3)^3+x(1)^2,x(3)^3+x(2)^2; 
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                               "//=====Z9                   1       2";       

ring r = 0,(x(1..3)),dp(3);            //Z10                 11 sec  12 Karten
ideal J=x(1)^2+x(2)*x(3)^2,x(2)^2+x(3)^2;                                      
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                           "//===== Z10                    0       1";

ring r = 0,(x(1..3)),dp(3);            //E12(2)              17 sec  16 Karten
ideal J=x(3)^2-x(1)^2*x(2),x(3)*x(2)-x(1)^5,x(2)^2-x(3)*x(1)^3;                
list L=resolve(J,1);size(L[1]);kill L;kill r;
                              "//=====E12(2)                 1       1";

ring r = 0,(x(1..3)),dp(3);            //W8*                 11 sec  11 Karten
ideal J=x(3)^2-x(1)*x(2)^2,x(3)*x(2)-x(1)^3,x(2)^3-x(3)*x(1)^2;               
list L=resolve(J,1);size(L[1]);kill L;kill r;
                              "//=====W8*                     0       1";

ring r = 0,(x(1..3)),dp(3);            //                    27 sec  29 Karten
ideal J=(x(1)^2-x(2)^3)^2+x(3)^2;                                      
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                          "//=====                     3       4";

ring r = 0,(x(1..3)),dp(3);            //                    10 sec  13 Karten 
ideal J=x(1)^2*x(2)^4-2*x(1)^3*x(2)^2+x(1)^4+x(3)^2;                           
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                          "//=====                     2       3";

ring r = 0,(x(1..3)),dp(3);            //Corina               5 sec   7 Karten
ideal J=x(1)^3-x(1)^2*x(3)-x(2)^2*x(3);                             
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                           "//=====Corina                     0       2";

ring r = 0,(x(1..3)),dp(3);            //coordinate axes      3 sec   3 Karten
ideal J=x(1)*x(2),x(1)*x(3),x(2)*x(3);                                         
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                         "//=====coordinate axes               0       3";

ring r = 0,(x(1..3)),dp(3);            //A2 v L               8 sec  10 Karten
ideal J=x(1)*x(3),x(2)*x(3),x(1)^2+x(2)^3;                                     
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                           "//=====A2 v L                     0       2";

ring r = 0,(x(1..3)),dp(3);            //E7 v L              10 sec  11 Karten
ideal J=x(1)*x(3),x(2)*x(3),x(2)*(x(1)^3-x(2)^2);                              
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                            "//=====E7 v L                     0       3";
ring r = 0,(x(1..3)),dp(3);
ideal J=(x(1)*x(2)-x(3)^2)^2+x(1)^3*x(3)+x(2)^5;
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                          "//=====Jonny Wahl                   9     10 ";     


ring r = 0,(x(1..3)),dp(3);            //Anne                 7 sec  7 Karten
ideal J=x(1)^2+x(2)^3,x(3)^2+x(1)*x(2)^4;                                      
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                          "//=====Anne                     1       1 ";     

ring r = 0,(x(1..3)),dp(3);            //Encinas              3 sec   4 Karten
ideal J=-(x(3)^2-x(1)^3)^2-2*x(1)^7*x(2)-2*x(1)^4*x(2)*x(3)^2;                 
list L=resolve(J,1);size(L[1]);kill L;kill r;
                              "//=====Encinas                  1       1";

ring r = 0,(x(1..3)),dp(3);            //Encinas               nicht
ideal J=-(x(3)^2-x(1))^2-2*x(2)*x(3)^2*x(1)^2-2*x(1)^3*x(2);                   
list L=resolve(J,1);size(L[1]);kill L;kill r;
                              "//=====Encinas                  4       7";

ring r = 0,(x(1..3)),dp(3);               //D4               20 sec  20 Karten 
ideal J=x(1)^2*x(2)+x(2)^3+x(3)^2;                                 
list L=resolve(J,1);size(L[1]);kill L;kill r; 
                               "//=====D4                  4      11";

ring r = 0,(x(1..3)),dp(3);               //D7               26 sec  31 Karten 
ideal J=x(1)^2*x(2)+x(2)^6+x(3)^2;                                        
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====D7                  5      16";

ring r = 0,(x(1..3)),dp(3);               //D10              51 sec  53 Karten 
ideal J=x(1)^2*x(2)+x(2)^9+x(3)^2;                                             
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                              "//=====D10                 15      29 ";     

ring r = 0,(x(1..3)),dp(3);               //D11              46 sec  51 Karten 
ideal J=x(1)^2*x(2)+x(2)^10+x(3)^2;                                      
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====D11                 15      28 ";    

ring r = 0,(x(1..3)),dp(3);               //D15              64 sec  71 Karten 
ideal J=x(1)^2*x(2)+x(2)^14+x(3)^2;                                            
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====D15                 51      40 ";    

ring r = 0,(x(1..3)),dp(3);               //Cayley cubic      5 sec   6 Karten 
ideal J=x(1)*x(2)+x(1)*x(3)+x(2)*x(3)+x(1)*x(2)*x(3);                          
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====Cayley cubic              0       3 ";  

ring r = 0,(x(1..3)),dp(3);               //Cox              20 sec  19 Karten 
ideal J=x(1)*x(2)*x(3)+x(3)^15;                            
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====Cox                  3      20 ";  

ring r = 0,(x(1..3)),dp(3);               //Zariski        1026 sec  32 Karten 
ideal J=x(1)^2*x(2)^2+x(1)^2*x(3)^2+x(2)^2*x(3)^2
        -2*x(1)*x(2)*x(3)*(x(1)+x(2)+x(3));                                    
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====Zariski                  3      12  "; 

ring r = 0,(x(1..3)),dp(3);               //Aoo               2 sec   3 Karten 
ideal J=x(2)^2+x(3)^2;                                               
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====Aoo                  0       1";

ring r = 0,(x(1..3)),dp(3);               //Doo               11 sec 13 Karten 
ideal J=x(1)*x(2)^2+x(3)^2;                                           
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====Doo                   2      7  "; 

ring r = 0,(x(1..3)),dp(3);               //Too,5,2           40 sec 42 Karten 
ideal J=x(1)^2*x(2)^2+x(2)^5+x(3)^2;                                  
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====Too,5,2                   2      3   ";    

ring r = 0,(x(1..3)),dp(3);               //W1,oo             73 sec 71 Karten 
ideal J=x(2)^4+x(1)^3*x(2)^2+x(3)^2;                  
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====W1,oo                   2      4  ";      

ring r = 0,(x(1..3)),dp(3);               //T,oo,5,6          21 sec 20 Karten 
ideal J=x(1)*x(2)*x(3)+x(2)^5+x(3)^6;                                          
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====T,oo,5,6                  2     10 ";      

ring r = 0,(x(1..3)),dp(3);               //Q5,oo               nicht
ideal J=x(1)*x(3)^2+x(2)^3+x(1)^5*x(2)^2;                                  
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====Q5,oo                   4     10  ";      

ring r = 0,(x(1..3)),dp(3);               //S1,oo             17 sec 18 Karten 
ideal J=x(2)^2*x(3)+x(1)*x(3)^2+x(1)^2*x(2)^2;                                 
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====S1,oo                   2      9 ";       

ring r = 0,(x(1..3)),dp(3);               // Theo             sec  Karten 
ideal J=-x(1)^6-x(1)^3*x(3)+x(2)^2*x(3);                                       
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====Theo               9      21 ";          

ring r = 0,(x(1..3)),dp(3);               // Theo2             sec  Karten 
ideal J=x(1)^8-x(1)^4*x(3)+x(2)^3*x(3);                                      
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====Theo2            391    46  ";         

ring r = 0,(x(1..3)),dp(3);               //              sec  Karten 
ideal J=x(1)^2*x(2)-x(1)*x(2)^2-x(1)^2*x(3)+x(2)^2*x(3)
        +x(1)*x(3)^2-x(2)*x(3)^2;                                     
list L=resolve(J,1);size(L[1]);kill L;kill r;  
                              "//=====               0     2  ";            

ring r = 0,(x(1..3)),dp(3);               //Theo5              sec  Karten 
ideal J=-x(1)*x(2)^4+x(3)^3;                             
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====Theo5            12     23    ";         

ring r = 0,(x(1..3)),dp(3);               //F2E8                 nicht 
ideal J=x(2)^5+x(1)*x(3)^3+x(2)^2*x(3)^2;              
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====F2E8                  11      21   ";

ring r = 0,(x(1..3)),dp(3);               //J5,oo                nicht 
ideal J=x(2)^3+x(1)^5*x(2)^2+x(3)^2;                 
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====J5,oo                    5     10   ";    

ring r = 0,(x(1..3)),dp(3);               //Z3,oo         Fehler in Procedur 
ideal J=x(1)*x(2)^3+x(1)^5*x(2)^2+x(3)^2;                   
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====Z3,oo                    4      7  ";     


ring r = 0,(x(1..3)),dp(3);               //P8                4 sec   6 Karten
ideal J=x(1)^3+x(2)^3+x(1)*x(2)*x(3)+x(3)^3;                                   
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                           "//=====P8                  0       3       "; 


ring r = 0,(x(1..3)),dp(3);               //X9                  nicht  
ideal J=x(1)^4+x(1)^2*x(2)^2+x(2)^4+x(3)^2;                                  
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//===== X9                 0       2      ";  

ring r = 0,(x(1..3)),dp(3);               //J10                 nicht 
ideal J=x(2)^6+x(1)^2*x(2)^2+x(1)^3+x(3)^2;                                    
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====J10                  2       6       "; 

ring r = 0,(x(1..3)),dp(3);               //T3,4,5           19 sec  17 Karten 
ideal J=x(3)^5+x(2)^4+x(1)^3+x(1)*x(2)*x(3);                              
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====T3,4,5                  1       3      ";

ring r = 0,(x(1..3)),dp(3);               //E12                  Fehler 
ideal J=x(2)^7+x(1)*x(2)^5+x(1)^3+x(3)^2;                                
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                           "//=====E12                   5     10       "; 

ring r = 0,(x(1..3)),dp(3);               //E13                  Fehler 
ideal J=x(2)^8+x(1)*x(2)^5+x(1)^3+x(3)^2;                            
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                           "//=====E13                   2     11        ";

ring r = 0,(x(1..3)),dp(3);               //E14               nicht (memory)  
ideal J=x(2)^8+x(1)*x(2)^6+x(1)^3+x(3)^2;                             
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                           "//===== E14                 4     10        ";

ring r = 0,(x(1..3)),dp(3);               //Z11                  Fehler  
ideal J=x(1)*x(2)^4+x(2)^5+x(1)^3*x(2)+x(3)^2;                            
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====Z11                   3      7        ";

ring r = 0,(x(1..3)),dp(3);               //Z12                  Fehler  
ideal J=x(1)^2*x(2)^3+x(1)*x(2)^4+x(1)^3*x(2)+x(3)^2;                  
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====Z12                   1      7       "; 

ring r = 0,(x(1..3)),dp(3);               //Z13                  nicht 
ideal J=x(1)*x(2)^5+x(2)^6+x(1)^3*x(2)+x(3)^2;                                 
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====Z13                  2       7        ";

ring r = 0,(x(1..3)),dp(3);               //W12                  nicht
ideal J=x(1)^2*x(2)^3+x(2)^5+x(1)^4+x(3)^2;                                    
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//===== W12                 2       6    ";

ring r = 0,(x(1..3)),dp(3);               //W13                  nicht 
ideal J=x(2)^6+x(1)*x(2)^4+x(1)^4+x(3)^2;                                      
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====W13                   1       7        ";

ring r = 0,(x(1..3)),dp(3);               //Q10              34 sec  34 Karten 
ideal J=x(1)*x(2)^3+x(2)^4+x(1)^3+x(2)*x(3)^2;                                 
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====Q10                   3      7      ";

ring r = 0,(x(1..3)),dp(3);               //Q11              15 sec  18 Karten 
ideal J=x(3)^5+x(1)*x(3)^3+x(1)^3+x(2)^2*x(3);              
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====Q11                  0       8       "; 

ring r = 0,(x(1..3)),dp(3);               //Q12              20 sec  23 Karten 
ideal J=x(1)*x(2)^4+x(2)^5+x(1)^3+x(2)*x(3)^2;          
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====Q12                  2       7       ";

ring r = 0,(x(1..3)),dp(3);               //S11             1717 sec 41 Karten 
ideal J=x(1)^4+x(1)^3*x(3)+x(2)^2*x(3)+x(1)*x(3)^2;           
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====S11                    2      7        ";

ring r = 0,(x(1..3)),dp(3);               //S12              22 sec  22 Karten 
ideal J=x(3)^5+x(1)*x(3)^3+x(1)^2*x(2)+x(2)^2*x(3);                
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====S12                   2      8    ";

ring r = 0,(x(1..3)),dp(3);               //U12               38 sec 33 Karten 
ideal J=x(1)*x(2)*x(3)^2+x(3)^4+x(1)^3+x(2)^3;             
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                           "//=====U12                   2      7        ";

ring r = 0,(x(1..3)),dp(3);               //J3,0                nicht (memory)
ideal J=x(2)^9+x(1)^2*x(2)^3+x(1)^3+x(3)^2;            
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//===== J3,0                 12     14     ";

ring r = 0,(x(1..3)),dp(3);               //J3,1                 nicht
ideal J=x(2)^10+x(1)^2*x(2)^3+x(1)^3+x(3)^2;           
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//=====J3,1                     12     22   ";  

ring r = 0,(x(1..3)),dp(3);               //Z1,0        Fehler (nach 15467 sec)
ideal J=x(2)^7+x(1)^2*x(2)^3+x(1)^3*x(2)+x(3)^2;              
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                            "//===== Z1,0                  10      11  ";  

ring r = 0,(x(1..3)),dp(3);               //S1,1                   Fehler 
ideal J=x(2)^6+x(1)^2*x(2)^2+x(1)^2*x(3)+x(2)*x(3)^2;           
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                           "//=====S1,1                       6       13";

ring r = 0,(x(1..3)),dp(3);               //U1,0            4277 sec 38 Karten 
ideal J=x(1)*x(2)^3+x(2)^3*x(3)+x(1)^3+x(1)*x(3)^2;                            
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                             "//=====U1,0                      1        6";

ring r = 0,(x(1..3)),dp(3);               // E18               nicht (memory) 
ideal J=x(2)^10+x(1)*x(2)^7+x(1)^3+x(3)^2;                            
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                           "//=====  E18                     7       14  "; 

ring r = 0,(x(1..3)),dp(3);               //W17                   Fehler 
ideal J=x(2)^7+x(1)*x(2)^5+x(1)^4+x(3)^2;                
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                           "//=====W17                       7      16 ";

ring r = 0,(x(1..3)),dp(3);               //U16              29 sec  26 Karten 
ideal J=x(2)^5+x(1)^2*x(2)^2+x(1)^3-x(1)*x(3)^2;          
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                           "//=====U16                   8      13";

ring r = 0,(x(1..6)),dp;                 //                      Fehler
ideal J=x(1)*x(6),x(2)*x(6),x(3)*x(6),x(1)*x(5),x(2)*x(5),
        x(3)*x(5),x(1)*x(4),x(2)*x(4),x(3)*x(4),x(2)^3-x(1)^2;
list L=resolve(J,1);size(L[1]);kill L;kill r;
                                "//=====                   3      10   "; 

ring r = 0,(x(1..6)),dp;                 //A4oo              31 sec  35 Karten
ideal J=x(1)^5+x(2)^2+x(3)^2+x(4)^2+x(5)^2;
list L=resolve(J,1);size(L[1]);kill L;kill r;
                                "//=====A4oo                4      12    ";    

ring r = 0,(x(1..6)),dp;                 //Theo1a           sec    Karten
ideal J=-x(1)^6-x(1)^3*x(3)+x(2)^2*x(3)+x(4)^2;                                
list L=resolve(J,1,"E");size(L[1]);kill L;kill r;
                                "//=====Theo1a                  250    154";

ring r = 0,(x(1..6)),dp;                 //Omega1            14 sec   6 Karten
ideal J=x(1)*x(6)-x(2)*x(3),x(1)*x(4)-x(3)*x(5),x(2)*x(4)-x(5)*x(6);           
list L=resolve(J,1);size(L[1]);kill L;kill r;
                                "//=====Omega1                   0       6";

ring r = 0,(x(1..6)),dp;                 //Omega3            44 sec  22 Karten
ideal J=x(1)*x(6)-x(2)*x(3),x(1)*(x(1)+x(4)^3)-x(3)*x(5),
                           x(2)*(x(1)+x(4)^3)-x(5)*x(6);                 
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                          "//=====Omega3                   2      16";

ring r = 0,(x(1..6)),dp;                 //Pi3               48 sec  29 Karten
ideal J=x(1)*x(6)-x(2)*x(3),x(1)*(x(2)+x(5)^3)-x(3)^2,
                           x(2)*(x(2)+x(5)^3)-x(3)*x(6);   
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                           "//=====Pi3                   1      10";

ring r = 0,(x(1..6)),dp;                 //Lamda             11 sec   5 Karten
ideal J=x(1)*x(6)-x(2)*x(3),x(1)*x(2)-x(3)^2,
                           x(2)^2-x(3)*x(6);  
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                          "//===== Lamda                  0       2";

ring r = 0,(x(1..6)),dp;        //cone over cubic scroll     11 sec   5 Karten
ideal J=-x(2)^2+x(1)*x(4),-x(2)*x(3)+x(1)*x(5),
             -x(3)*x(4)+x(2)*x(5);                                 
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                           "//=====cone over cubic scroll      1       4";

ring r = 0,(x(1..6)),dp;                 //A3#              149 sec  99 Karten
ideal J=x(1)*x(6)-x(2)*x(3),x(1)*(x(1)^4+x(2)^2)-x(3)*x(5),
             x(2)*(x(1)^4+x(2)^2)-x(5)*x(6);                                   
list L=resolve(J,1);size(L[1]);kill L;kill r;   
                            "//=====/A3#                 122      81 //49 ";  

ring r = 0,(x(1..6)),dp;                 //E8#              nicht (memory)
ideal J=x(1)*x(6)-x(2)*x(3),x(1)*(x(1)^3+x(2)^5)-x(3)*x(5),
             x(2)*(x(1)^3+x(2)^5)-x(5)*x(6);                                   
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                           "//=====E8#                   17      16 ";

ring r = 0,(x(1..6)),dp;                 //D4#             1221 sec  98 Karten
ideal J=-x(2)*x(3)+x(1)*x(6),x(1)^4+x(1)^2*x(2)^2-x(3)*x(5),
             x(1)^3*x(2)+x(1)*x(2)^3-x(5)*x(6);                 
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                           "//=====D4#                   2       9   ";     

ring r = 0,(x(1..6)),dp;                 //D5#               nicht (memory)
ideal J=-x(2)*x(3)+x(1)*x(6),x(1)^5+x(1)^2*x(2)^2-x(3)*x(5),
             x(1)^4*x(2)+x(1)*x(2)^3-x(5)*x(6);         
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                          "//=====/D5#                    6      11    "; 

ring r = 0,(x(1..6)),dp;                 //E6#                  nicht
ideal J=x(1)*x(6)-x(2)*x(3),x(1)*(x(1)^3+x(2)^4)-x(3)*x(5),
             x(2)*(x(1)^3+x(2)^4)-x(5)*x(6);    
list L=resolve(J,1);size(L[1]);kill L;kill r;    
                           "//=====E6#                     32      17//16  "; 

ring r = 0,(x(1..6)),dp;                 //E7#                  nicht
ideal J=x(1)*x(6)-x(2)*x(3),x(1)*(x(1)^3+x(1)*x(2)^3)-x(3)*x(5),
             x(2)*(x(1)^3+x(1)*x(2)^3)-x(5)*x(6);   
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                          "//===== E7#                    6      18   ";

ring r = 0,(x(1..6)),dp;                 //cone over Veronese 28 sec  11 Karten
ideal J=x(4)^2-x(3)*x(5),x(2)*x(4)-x(1)*x(5),x(2)*x(3)-x(1)*x(4),
        x(2)^2-x(6)*x(5),x(1)*x(2)-x(6)*x(4),x(1)^2-x(6)*x(3); 
list L=resolve(J,1);size(L[1]);kill L;kill r;     
                          "//=====cone over Veronese        1       3      "; 

ring r = 0,(x(1..9)),dp;                 //                  452 sec 183 Karten
ideal J=x(6)*x(8)-x(5)*x(9),x(5)*x(7)-x(4)*x(8),x(3)*x(5)-x(2)*x(6),
        x(2)*x(4)-x(1)*x(5),x(3)*x(4)*x(8)-x(1)*x(5)*x(9);
list L=resolve(J,1,"E");size(L[1]);kill L;kill r;     
                          "//=====                    14      61    ";  
tst_status(1);$


