LIB "tst.lib";
tst_init();

ring A  = 0,(x,y,z),dp;
poly f  = x2yz+xy2z+y2z+z3+xy;
poly f1 = xy+y2-1;
poly f2 = xy;
ideal G = f1,f2;
ideal S = std(G);
S;

reduce(f,G);

G=f2,f1;
reduce(f,G);

reduce(f,S,1); 

reduce(f,S); 

tst_status(1);$
