LIB "tst.lib"; tst_init();
  ring r=0,(x,y,z,a,b,c),dp;
  ideal i=xy2z3a4b5+1,homog(xy2z3a4b5+1,c); i;
  ring r1=0,(a,b,x,y,z),lp;
  ideal j=imap(r,i); j;
  ring r2=(0,a,b),(x,y,z),ls;
  ideal j=imap(r,i); j;
tst_status(1);$
