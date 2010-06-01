  ring r=32003,(x,y,z),dp;
  ideal I=xy,xz,yz;
  indepSet(std(I));
  indepSet(std(I),1);
  eliminate(I,yz);
  ideal i;
  indepSet(i);
  indepSet(i,1);
LIB "tst.lib";tst_status(1);$
