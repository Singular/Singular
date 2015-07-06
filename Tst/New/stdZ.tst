LIB "tst.lib"; tst_init();
ring r = integer,(v,w,t),dp; r;


ideal J = 2v, wt, vt-3v;
std(J);
//_[1]=2v
//_[2]=wt
//_[3]=vt-3v
NF( w*v, _);


groebner(J);
NF( w*v, _);

// fixed BUG: (w*v) is missing  S( wt, vt-3v) = 3wv

tst_status(1);$

