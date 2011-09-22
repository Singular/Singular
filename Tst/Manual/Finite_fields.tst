LIB "tst.lib"; tst_init();
  int p=3;  int n=3;  int d=5; int k=2;
  ring rp = p,(x(1..n)),dp;
  int s = size(maxideal(d));
  s;
  // create a dense homogeneous ideal m, all generators of degree d, with
  // generic (random) coefficients:
  ideal m = maxideal(d)*random(p,s,n-2);
  m;
  // look for zeros on the torus by checking all points (with no component 0)
  // of the affine n-space over the field with p elements :
  ideal mt;
  int i(1..n);                    // initialize integers i(1),...,i(n)
  int l;
  s=0;
  for (i(1)=1;i(1)<p;i(1)=i(1)+1)
  {
    for (i(2)=1;i(2)<p;i(2)=i(2)+1)
    {
      for (i(3)=1;i(3)<p;i(3)=i(3)+1)
      {
        mt=m;
        for (l=1;l<=n;l=l+1)
        {
          mt=subst(mt,x(l),i(l));
        }
        if (size(mt)==0)
        {
          "solution:",i(1..n);
          s=s+1;
        }
      }
    }
  }
  "//",s,"solutions over GF("+string(p)+")";
  // Now go to the field with p^3 elements:
  // As long as there is no map from Z/p to the field with p^3 elements
  // implemented, use the following trick: convert the ideal to be mapped
  // to the new ring to a string and then execute this string in the
  // new ring
  string ms="ideal m="+string(m)+";";
  ms;
  // define a ring rpk with p^k elements, call the primitive element z. Hence
  // 'solution exponent: 0 1 5' means that (z^0,z^1,z^5) is a solution
  ring rpk=(p^k,z),(x(1..n)),dp;
  rpk;
  execute(ms);
  s=0;
  ideal mt;
  for (i(1)=0;i(1)<p^k-1;i(1)=i(1)+1)
  {
    for (i(2)=0;i(2)<p^k-1;i(2)=i(2)+1)
    {
      for (i(3)=0;i(3)<p^k-1;i(3)=i(3)+1)
      {
        mt=m;
        for (l=1;l<=n;l=l+1)
        {
          mt=subst(mt,x(l),z^i(l));
        }
        if (size(mt)==0)
        {
          // we show only the first 7 solutions here:
          if (s<5) {"solution exponent:",i(1..n);}
          s=s+1;
        }
      }
    }
  }
  "//",s,"solutions over GF("+string(p^k)+")";
tst_status(1);$
