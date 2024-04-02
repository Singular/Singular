ring F; list L = ringlist(F);
L[1];
L[1] = 0;
def Q = ring(L); setring Q; Q; setring F; kill Q; // Ok
L[1] = list(0); // NOTE: this is a wrong code
L[1];
def Q=ring(L); // Ups...
setring Q; Q;
$
