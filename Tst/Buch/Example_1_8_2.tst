LIB "tst.lib";
tst_init();

ring A  = 0,(x,y),dp;       
ideal I = x10+x9y2,y8-x2y7;
poly f  = xy13+y12;
matrix M=lift(I,f);      
M;

f-M[1,1]*I[1]-M[2,1]*I[2];

ring R=0,(x,y,z),ds;
poly f=yx2+yx;
ideal I=x-x2,y+x;
list L=division(f,I);
L;
matrix(I)*L[1]-matrix(f)*L[2]-matrix(L[3]);

tst_status(1);$
