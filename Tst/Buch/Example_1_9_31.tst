LIB "tst.lib";
tst_init();

// =============================== example 1.9.31 ==============================

LIB "ncalg.lib";
def R = makeQso3(3);
setring R; R;
option(redSB); option(redTail); // for reduced output

ideal K = x+y+z,y+z,z;
option(); module S = syz(K);        // the (left) syzygy module of K
print(S); print( size( module(transpose(S)*transpose(K)) ) );

K = x,y,z;
option(); S = syz(K);
print(S); print( size( module(transpose(S)*transpose(K)) ) );

option(returnSB);

option(); S = syz(K);
print(S); print( size( module(transpose(S)*transpose(K)) ) );

K = x+y+z,y+z,z;
option(); S = syz(K);        // the (left) syzygy module of K
print(S); print( size( module(transpose(S)*transpose(K)) ) );

tst_status(1);$

