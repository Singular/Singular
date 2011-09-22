LIB "tst.lib"; tst_init();
LIB "homolog.lib";
ring A = 0,(x,y),dp;
matrix M[3][3] = x-1,y,x,x,x+1,y,x2,xy+x+1,x2+y;
print(M);
isFlat(M);             // coker(M) is not flat over A=Q[x,y]
qring B = std(x2+x-y);   // the ring B=Q[x,y]/<x2+x-y>
matrix M = fetch(A,M);
isFlat(M);             // coker(M) is flat over B
setring A;
qring C = std(x2+x+y);   // the ring C=Q[x,y]/<x2+x+y>
matrix M = fetch(A,M);
isFlat(M);             // coker(M) is not flat over C
tst_status(1);$
