  ring r=32003,(x,y,z),dp;
  ideal I=xy,xz,yz;
  indepSet(std(I));
  eliminate(I,yz);
$
