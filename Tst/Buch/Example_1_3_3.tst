LIB "tst.lib";
tst_init();


ring S = 0,(a,b,c),lp;
ring R = 0,(x,y,z),dp;
ideal i = x, y, x2-y3;
map phi = S,i;     
LIB "algebra.lib"; 

is_injective(phi,S);

ideal j = x, x+y, z-x2+y3;
map psi = S,j;                  
is_injective(psi,S);

alg_kernel(phi,S);

alg_kernel(psi,S);

ideal Z;                    
setring S;
preimage(R,phi,Z);      

setring R;
is_surjective(psi,S);

is_bijective(psi,S); 

tst_status(1);$
