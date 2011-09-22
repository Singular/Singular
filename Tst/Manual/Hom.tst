LIB "tst.lib"; tst_init();
LIB "homolog.lib";
int p     = printlevel;
printlevel= 1;   //in 'example proc' printlevel has to be increased by 1
ring r    = 0,(x,y),dp;
ideal i   = x2-y3,xy;
qring q   = std(i);
ideal i   = fetch(r,i);
module M  = [-x,y],[-y2,x],[x3];
module H  = Hom(M,i);
print(H);
printlevel= 2;
list L    = Hom(M,i,1);"";
printlevel=1;
ring s    = 3,(x,y,z),(c,dp);
ideal i   = jacob(ideal(x2+y5+z4));
qring rq=std(i);
matrix M[2][2]=xy,x3,5y,4z,x2;
matrix N[3][2]=x2,x,y3,3xz,x2z,z;
print(M);
print(N);
list l=Hom(M,N,1);
printlevel = p;
tst_status(1);$
