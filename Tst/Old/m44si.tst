  ring r2=0,(x(1..12)),ds;
  matrix m[5][5]=maxideal(1),maxideal(1);
  print(m);
  list l =bareiss(m);
  print(l[1]);
  l[2];
$
