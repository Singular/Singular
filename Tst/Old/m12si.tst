  int j=1;
  ring r;
  int i=dim(std(ideal(x,y)));// dimension of factor ring defined by (x,y)
                 // (i.e. R/(x,y)) if it is given by a standard basis
  int compare = i<j;
  compare;      // 0 (FALSE) if i >= j, 1 (TRUE) if i<j
LIB "tst.lib";tst_status(1);$
