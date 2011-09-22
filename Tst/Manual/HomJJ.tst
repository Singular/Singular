LIB "tst.lib"; tst_init();
LIB "normal.lib";
ring r   = 0,(x,y),wp(2,3);
ideal id = y^2-x^3;
ideal J  = x,y;
poly p   = x;
list Li  = std(id),id,J,p;
list L   = HomJJ(Li);
def end = L[1];    // defines ring L[1], containing ideals endid, endphi
setring end;       // makes end the basering
end;
endid;             // end/endid is isomorphic to End(r/id) as ring
map psi = r,endphi;// defines the canonical map r/id -> End(r/id)
psi;
L[3];              // contribution to delta
tst_status(1);$
