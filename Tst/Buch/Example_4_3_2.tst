LIB "tst.lib";
tst_init();

option(redSB);         
ring R=0,(x,y),lp;            
ideal a1=x;             
ideal a2=y2+2y+1,x-1;
ideal a3=y2-2y+1,x-1;
ideal I=intersect(a1,a2,a3);
I;

ideal G=std(I);
indepSet(G);

ring S=(0,y),(x),lp; 
ideal G=imap(R,G);
G;

setring R;
poly h=y4-2y2+1;   

ideal I1=quotient(I,h);
I1;

tst_status(1);$
