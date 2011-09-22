LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
intvec v = option(get);
// binary cyclic [15,7,5] code with defining set (1,3)
list defset=1,3;             // defining set
int n=15;                    // length
int d=5;                     // candidate for the minimum distance
def A=sysCRHTMindist(n,defset,d);
setring A;
A;                           // shows the ring we are working in
print(crht_md);              // the Sala's ideal for mindist
option(redSB);
ideal red_crht_md=std(crht_md);
print(red_crht_md);          // reduced Groebner basis
option(set,v);
tst_status(1);$
