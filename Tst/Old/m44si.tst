  ring r2=0,(x(1..12)),ds;
  matrix m[5][5]=maxideal(1),maxideal(1);
  print(m);
  m=bareiss(m);
  print(m);
  m[3,2];
$
