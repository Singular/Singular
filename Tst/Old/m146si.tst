  ring r=0,(x,y,z),dp;
  module m=[1,y],[0,x+z];
  m;
  print(m);  // the columns generate m
  intmat M=betti(mres(m,0));
  print(M,"betti");
$
