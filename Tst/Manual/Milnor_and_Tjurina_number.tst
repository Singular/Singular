LIB "tst.lib"; tst_init();
  option(prot);
  ring r1 = 32003,(x,y,z),ds;
  r1;
  int a,b,c,t=11,5,3,0;
  poly f = x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+
           x^(c-2)*y^c*(y^2+t*x)^2;
  f;
  ideal i=jacob(f);
  i;
  ideal j=std(i);
  "The Milnor number of f(11,5,3) for t=0 is", vdim(j);
  j=i+f;    // override j
  j=std(j);
  vdim(j);  // compute the Tjurina number for t=0
  t=1;
  f=x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3
    +x^(c-2)*y^c*(y^2+t*x)^2;
  ideal i1=jacob(f);
  ideal j1=std(i1);
  "The Milnor number of f(11,5,3) for t=1:",vdim(j1);
  vdim(std(j1+f));   // compute the Tjurina number for t=1
  option(noprot);
tst_status(1);$
