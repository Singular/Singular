LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..3)),dp;
list flag=identifyvar();
ideal J=x(1)^4*x(2)^2, x(3)^3;
list Lmb=1,list(0,0,0),list(0,0,0),list(3),iniD(3),iniD(3),list(0,0,0),-1;
list L=data(J,2,3);
list LL=determinecenter(L[1],L[2],3,3,0,0,Lmb,flag,0,-1); // Calculate the center
module auxpath=[0,-1];
list infochart=0,0,0,L[2],L[1],flag,0,list(0,0,0),auxpath,list(),list();
list L3=Blowupcenter(LL[1],1,1,infochart,3,3,0); // blowing-up and looking to the x(3) chart
calculateI(L3[2][1][5],L3[2][1][4],3,3,3,L3[2][1][3],3,iniD(3)); //  (I_3)
// looking to the x(1) chart
calculateI(L3[2][2][5],L3[2][2][4],3,3,1,L3[2][2][3],3,iniD(3)); //  (I_3)
tst_status(1);$
