LIB "tst.lib"; tst_init();

ring r=0,(x,y,z),dp;
module m=gen(1),gen(3),[x,y,0,z],[x+y,0,0,0,1];
smatrix M;
print(m);
print(prune_map(m,M)); // pruned module
print(m*M); // apply map
print(M); // map
//  0-module
m=[0];
print(m);
print(prune_map(m,M)); // pruned module
print(m*M); // apply map
print(M); // map
// ------------
ring R;
module m=[1],[x],[0,z],[0,0,1];
smatrix t;
matrix mm;
prune_map (m,t);
prune_map (m,1);
prune_map (m,mm);


tst_status(1);$
