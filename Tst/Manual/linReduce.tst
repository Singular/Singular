LIB "tst.lib"; tst_init();
LIB "bfun.lib";
ring r = 0,(x,y),dp;
ideal I = 1,y,xy;
poly f = 5xy+7y+3;
poly g = 7x+5y+3;
linReduce(g,I);     // reduces tails
linReduce(g,I,0,0); // no reductions of tails
linReduce(f,I,1);   // reduces tails and shows reductions used
f = x3+y2+x2+y+x;
I = x3-y3, y3-x2,x3-y2,x2-y,y2-x;
list l = linReduce(f,I,1);
l;
module M = I;
f - (l[1]-(M*l[2])[1,1]);
tst_status(1);$
