LIB "tst.lib";
tst_init();

ring R = 0,(x,y,z),dp;
poly p =  z4+2z2+1;
LIB "primdec.lib";      
radical(p);                 


ideal I = xyz, x2, y4+y5;   
radical(I);

ideal Is = groebner(I);
int k;
while ( reduce((y2+y)^k,Is) != 0 ){k=k+1;}
k;           


tst_status(1);$
