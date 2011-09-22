LIB "tst.lib"; tst_init();
  ring r=32003,(x,y,z),dp;
  print(koszul(2,3));
  ideal I=xz2+yz2+z3,xyz+y2z+yz2,xy2+y3+y2z;
  print(koszul(1,I));
  print(koszul(2,I));
  print(koszul(2,I)*koszul(3,I));
tst_status(1);$
