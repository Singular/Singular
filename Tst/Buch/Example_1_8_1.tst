LIB "tst.lib";
tst_init();

ring A  = 0,(x,y),dp;       
ideal I = x10+x9y2,y8-x2y7;
ideal J = std(I);
poly f  = x2y7+y14;
reduce(f,J,1);      
f  = xy13+y12;
reduce(f,J,1); 

ideal K = f,x2y7+y14;
reduce(K,J,1);      



K=f,y14+xy12;
size(reduce(K,J,1));        


tst_status(1);$
