LIB "tst.lib";
tst_init();

// std (SB,p)
ring rng = 67,(x,y,z),Dp;
ideal I = x^2-9*z,-9*x*y+x-10;
poly J = -6*x*z-10;
ideal gI   =  std(I);
ideal IJ   = I+J;
ideal gIJ  = std(IJ);
ideal gI_J = std(gI,J);
size( std(reduce (I,gI ) ));
size( std(reduce (IJ,gIJ ) )); 
size( std(reduce (IJ,gI_J ) ));
size( std(reduce (gI_J, gIJ) )); 
gI_J;
gIJ;

tst_status(1);$
