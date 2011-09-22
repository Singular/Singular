LIB "tst.lib"; tst_init();
  proc algebraicDep(ideal J, poly g)
  {
    def R=basering;         // give a name to the basering
    int n=size(J);
    int k=nvars(R);
    int i;
    intvec v;

    // construction of the new ring:

    // construct a weight vector
    v[n+k]=0;         // gives a zero vector of length n+k
    for(i=1;i<=k;i++)
    {
      v[i]=1;
    }
    string orde="(a("+string(v)+"),dp);";
    string ri="ring Rhelp=("+charstr(R)+"),
                          ("+varstr(R)+",Y(1.."+string(n)+")),"+orde;
                            // ring definition as a string
    execute(ri);            // execution of the string

    // construction of the new ideal I=(J[1]-Y(1),...,J[n]-Y(n))
    ideal I=imap(R,J);
    for(i=1;i<=n;i++)
    {
      I[i]=I[i]-var(k+i);
    }
    poly g=imap(R,g);
    if(g==0)
    {
      // construction of the ideal of relations by elimination
      poly el=var(1);
      for(i=2;i<=k;i++)
      {
        el=el*var(i);
      }
      ideal KK=eliminate(I,el);
      keepring(Rhelp);
      return(KK);
    }
    // reduction of g with respect to I
    ideal KK=reduce(g,std(I));
    keepring(Rhelp);
    return(KK);
  }

  // applications of the procedure
  ring r=0,(x,y,z),dp;
  ideal i=xz,yz;
  algebraicDep(i,0);
  // Note: after call of algebraicDep(), the basering is Rhelp.
  setring r; kill Rhelp;
  ideal j=xy+z2,z2+y2,x2y2-2xy3+y4;
  algebraicDep(j,0);
  setring r; kill Rhelp;
  poly g=y2z2-xz;
  algebraicDep(i,g);
  // this shows that g is contained in i.
  setring r; kill Rhelp;
  algebraicDep(j,g);
  // this shows that g is contained in j.
tst_status(1);$
