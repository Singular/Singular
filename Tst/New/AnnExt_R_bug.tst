LIB "tst.lib";
tst_init();

LIB("primdec.lib");

ring rng = 0,(x,y,z),dp;
ideal I = xy,xz,yz;     // 3 coordinate axes in A^3

ideal J = Primdec::equidimMaxEHV(I);   // should be I

ideal temp1=reduce(I,std(J));
temp1;
ideal temp2=reduce(J,std(I));
temp2;

ASSUME(0,size(temp1)==0);
ASSUME(0,size(temp2)==0);

tst_status(1);
$
