LIB "tst.lib";
tst_init();

LIB"random.lib";
ring R=0,(x,y,z),lp;
ideal I=xy,xz;
dim(std(I));

ideal M=ideal(sparsetriag(3,3,0,100)
       *transpose(maxideal(1)));
M;

map phi=R,M;
ideal J=phi(I); 
J;

tst_status(1);$
