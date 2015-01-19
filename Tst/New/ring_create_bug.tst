ring F; list L = ringlist(F);
L[1];
L[1] = 0;
def Q = ring(L); setring Q; Q; setring F; kill Q; // Ok

L[1] = list(0); // NOTE: this is a wrong code
L[1];
def Q=ring(L); // Ups... Segment fault :(
/*
   ? invald coeff. field description, expecting precision list

Singular : signal 11 (v: 0/$Id: 1a20828 Mon Dec 3 17:39:55 2012 +0100$):
current line:>>def Q=ring(L);<<
Segment fault/Bus error occurred (r:1354651089)
*/
setring Q; Q;
$
