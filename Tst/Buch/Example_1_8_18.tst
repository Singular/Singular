LIB "tst.lib";
tst_init();


ring A=0,(x,y,z),dp;
ring B=0,(a,b),dp;
map phi=A,a2,ab,b2;
ideal zero;             
setring A;
preimage(B,phi,zero);    
ring C=0,(x,y,z,a,b), dp; 
ideal H=x-a2, y-ab, z-b2;
eliminate(H,ab);

tst_status(1);$
