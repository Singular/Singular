LIB "tst.lib";
tst_init();
LIB "ellipticcovers.lib";
ring R=(0,x1,x2,x3,x4),(q1,q2,q3,q4,q5,q6),dp;
graph G = makeGraph(list(1,2,3,4),list(list(1,3),list(1,2),list(1,2),list(2,4),list(3,4),list(3,4)));
G;
propagator(list(x1,x2),0);
propagator(list(x1,x2),2);
propagator(G,list(1,1,1,0,0,0));

number P1= propagator(G,list(1,1,1,0,0,0));
computeConstant(P1,x2);


partitions(3,7);

number P = propagator(G,list(0,2,1,0,0,1));
gromovWitten(P);

partitions(6,2);

ring S=(0,x1,x2),(q1,q2,q3),dp;
graph G2 = makeGraph(list(1,2),list(list(1,2),list(1,2),list(1,2)));
gromovWitten(G2,list(0,2,1));
gromovWitten(G2,3);
generatingFunction(G2,3);

ring T=(0,x1,x2,x3,x4),(q),dp;
permute(list(x1,x2,x3,x4));


tst_status(1);
$

