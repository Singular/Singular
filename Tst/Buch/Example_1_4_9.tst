LIB "tst.lib";
tst_init();

ring S = 0,(x,y,z),dp;          
ideal I = y*(x-1), z*(x-1);
ideal J = std(I);   
J;                  

dim(J);         
                          
                                
reduce(y,J);    

ring R = 0,(x,y,z),ds;          
ideal I = fetch(S,I);
ideal J = std(I);  
J;
dim(J);

reduce(y,J);

map trans = S, x+1,y,z;  
ideal I1 = trans(I);
I1;
dim(std(I1));          

setring S;               
map trans = S, x+1,y,z; 
ideal I1 = trans(I);     
I1;
dim(std(I1)); 

kill S,R;

tst_status(1);$
