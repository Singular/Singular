LIB "tst.lib";
tst_init();


ring A=0,(x,y),dp;
poly f=x4-y4;
poly f1=x2+y2;
poly f2=x2-y2;
LIB"algebra.lib";
algDependent(ideal(f,f1,f2))[1];
ring B=0,(u,v,w),dp;         
setring A;
ideal zero;
map phi=B,f,f1,f2;
setring B;
preimage(A,phi,zero); 
tst_status(1);$
