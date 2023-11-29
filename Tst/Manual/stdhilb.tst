LIB "tst.lib"; tst_init();
ring  r = 0,(x,y,z),lp;
ideal i = y3+x2,x2y+x2z2,x3-z9,z4-y2-xz;
ideal j = stdhilb(i); j;
ring  r1 = 0,(x,y,z),wp(3,2,1);
ideal  i = y3+x2,x2y+x2z2,x3-z9,z4-y2-xz;  //ideal is homogeneous
ideal j = stdhilb(i,"std"); j;
//this is equivalent to:
bigintvec v = hilb(std(i),1);
ideal j1 = std(i,v,intvec(3,2,1)); j1;
size(NF(j,j1))+size(NF(j1,j));            //j and j1 define the same ideal
tst_status(1);$
