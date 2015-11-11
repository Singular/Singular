LIB "tst.lib"; tst_init();


// Example 5, tr. #629
ring rng = integer,(x,y,z),lp;
ideal  IJ= 13xyz+6xy+78xz+36x-11y2z-66yz,
168xyz+84xy+1008xz+504x+12y3-154y2z+72y2-924yz,
-168x2yz2-84x2yz-1008x2z2-504x2z+xy3z+6xy3+154xy2z2+6xy2z+36xy2+924xyz2
-11y4z-66y3z;

ideal stdIJ = std(IJ);
stdIJ;
ideal stdstdIJ = std(stdIJ);
stdstdIJ;
  
tst_status(1);$
