LIB "tst.lib"; tst_init();
LIB "resbinomial.lib";
ring r = 0,(x(1..2)),dp;
ideal J=x(1)^2-x(2)^3;
list L=Eresol(J);
list B=salida(5,L[1][5],L[8][6],2,L[1][3][3],2,1,0); // chart 5
def RR=B[1];
setring RR;
BO;
"press return to see next example"; ~;
ring r = 0,(x(1..2)),dp;
ideal J=x(1)^2-x(2)^3;
list L=Eresol(J);
list B=salida(7,L[1][7],L[8][8],0,L[1][5][3],2,1,0); // chart 7
def RR=B[1];
setring RR;
BO;
showBO(BO);
"press return to see next example"; ~;
ring r = 0,(x(1..2)),dp;
ideal J=x(1)^3-x(1)*x(2)^3;
list L=Eresol(J); // 8 charts, rational exponents
list B=salida(1,L[1][1],L[8][2],2,0,2,2,0); // CHART 1
def RR=B[1];
setring RR;
BO;
tst_status(1);$
