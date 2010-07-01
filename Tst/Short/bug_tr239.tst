LIB "tst.lib";
tst_init();

// finduni required id, should now also take data:
ring  r=0,(x,y,z), dp;
ideal i=y3+x2,x2y+x2,z4-x2-y;
option(redSB);  // force computation of reduced basis
i=std(i);
ideal k=finduni(i);
print(k);
print(finduni(std(i)));

tst_status(1);$
