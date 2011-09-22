LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..4)),dp;
list flag=identifyvar();
ideal J=x(1)^2-x(2)^2*x(3)^5, x(1)*x(3)^3+x(4)^6;
list Lmb=1,list(0,0,0,0),list(0,0,0,0),list(0,0,0,0),iniD(4),iniD(4),list(0,0,0,0),-1;
list L=data(J,2,4);
list LL=determinecenter(L[1],L[2],2,4,0,0,Lmb,flag,0,-1); // Compute the first center
LL[1];  // index of variables in the center
LL[2];  // exponents of ideals J_4,J_3,J_2,J_1
LL[3];  // list of orders of J_4,J_3,J_2,J_1
LL[4];  // list of critical values
LL[5];  // components of the resolution function t
LL[6];  // list of D_4,D_3,D_2,D_1
LL[7];  // list of H_4,H_3,H_2,H_1 (exceptional divisors)
LL[8];  // list of all exceptional divisors acumulated
LL[9];  // auxiliary invariant
LL[10]; // intvec pointing out the last step where the function t has dropped
ring r= 0,(x(1..4)),dp;
list flag=identifyvar();
ideal J=x(1)^3-x(2)^2*x(3)^5, x(1)*x(3)^3+x(4)^5;
list Lmb=2,list(0,0,0,0),list(0,0,0,0),list(0,0,0,0),iniD(4),iniD(4),list(0,0,0,0),-1;
list L2=data(J,2,4);
list L3=determinecenter(L2[1],L2[2],2,4,0,0,Lmb,flag,0,-1); // Example with rational exponents in E-Coeff
L3[1]; // index of variables in the center
L3[2]; // exponents of ideals J_4,J_3,J_2,J_1
L3[3]; // list of orders of J_4,J_3,J_2,J_1
L3[4]; // list of critical values
L3[5]; // components of the resolution function
tst_status(1);$
