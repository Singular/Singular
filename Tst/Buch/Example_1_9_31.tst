LIB "tst.lib";
tst_init();

// =============================== example 1.9.31 ==============================

LIB "ncalg.lib";
def R = makeQso3(3);
setring R;
option(redSB); option(redTail); // for reduced output
ideal K = x+y+z,y+z,z;
module S = syz(K);        // the (left) syzygy module of K
print(S);                       

ideal tst = ideal(transpose(S)*transpose(K));
print(matrix(tst));

K = x,y,z;
S = syz(K);
print(S);

option(returnSB);
S = syz(K);      
print(S);

tst_status(1);$

