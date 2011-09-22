LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1),y(2),x(3),y(4),x(5..7)),dp;
list flag=identifyvar();
ideal J=x(1)^3-x(3)^2*y(4)^2,x(1)*x(7)*y(2)-x(6)^3*x(5)*y(4)^3,x(5)^3-x(5)^3*y(2)^2;
list Lmb=2,list(0,0,0,0,0,0,0),list(0,0,0,0,0,0,0),list(0,0,0,0,0,0,0),iniD(7),iniD(7),list(0,0,0,0,0,0,0),-1;
list L=data(J,3,7);
list L2=determinecenter(L[1],L[2],2,7,0,0,Lmb,flag,0,-1); // Computing the center
module auxpath=[0,-1];
list infochart=0,0,0,L[2],L[1],flag,0,list(0,0,0,0,0,0,0),auxpath,list(),list();
list L3=Blowupcenter(L2[1],1,1,infochart,2,7,0);
L3[1]; // current chart (parent,Y,center,expJ,Coef,flag,Hhist,blwhist,path,hipercoef,hiperexp) with sons: [12],...,[16]
L3[2][1]; // information of its first son, write L3[2][2],...,L3[2][5] to see the other sons
L3[3];    // current number of charts
L3[4];    // step/level associated to each son
L3[5];    // number of variables in the center
tst_status(1);$
