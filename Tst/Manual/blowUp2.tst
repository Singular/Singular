LIB "tst.lib"; tst_init();
LIB "resolve.lib";
ring r=0,(x,y,z),dp;
ideal I=z2-x^3*y^2;
ideal C=z,xy;
list li=blowUp2(I,C);
size(li);                  // number of charts
def S1=li[1];
setring S1;                // chart 1
basering;
Jnew;
eD;
bM;
def S2=li[2];
setring S2;                // chart 2
basering;
Jnew;
eD;
bM;
tst_status(1);$
