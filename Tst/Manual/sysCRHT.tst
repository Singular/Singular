LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
// binary cyclic [15,7,5] code with defining set (1,3)
intvec v = option(get);
list defset=1,3;           // defining set
int n=15;                  // length
int e=2;                   // error-correcting capacity
int q=2;                   // basefield size
int m=4;                   // degree extension of the splitting field
int sala=1;                // indicator to add additional equations
def A=sysCRHT(n,defset,e,q,m);
setring A;
A;                         // shows the ring we are working in
print(crht);               // the CRHT-ideal
option(redSB);
ideal red_crht=std(crht);  // reduced Groebner basis
print(red_crht);
//============================
A=sysCRHT(n,defset,e,q,m,sala);
setring A;
print(crht);                // CRHT-ideal with additional equations from Sala
option(redSB);
ideal red_crht=std(crht);   // reduced Groebner basis
print(red_crht);
red_crht[5];                // general error-locator polynomial for this code
option(set,v);
tst_status(1);$
