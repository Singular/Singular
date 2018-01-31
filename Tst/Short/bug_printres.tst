LIB "tst.lib";
tst_init();

// printing a resolution not minimzed cached this even if it will later be minimized
ring R=QQ,(x,y,z),(c,dp);
ideal I=yz+z2,y2+xz,xy+z2,xz2,x2z;
resolution Se=sres(std(I),0);
Se;
resolution mSe=minres(Se);
mSe;
print(Se);
Se;

tst_status(1);$
