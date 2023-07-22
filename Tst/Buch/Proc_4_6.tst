LIB "tst.lib";
tst_init();

LIB"ring.lib";
proc PrimaryTest(ideal i, poly p)
"USAGE:   PrimaryTest(i,p); i standard basis with respect to
          lp, p irreducible polynomial in K[var(n)],
          p^a=i[1] for some a;
RETURN:   an ideal, the radical of i if i is primary and in
          general position with respect to lp,
          the zero ideal else.
"
{
   int m,e;
   int n=nvars(basering);
   poly t;
   ideal prm=p;

   for(m=2;m<=size(i);m++)
   {
     if(size(ideal(leadexp(i[m])))==1)
     {
       n--;
//----------------i[m] has a power of var(n) as leading term
       attrib(prm,"isSB",1);
//--- ?? i[m]=(c*var(n)+h)^e modulo prm for h
//     in K[var(n+1),...], c in K ??
       e=deg(lead(i[m]));
       t=leadcoef(i[m])*e*var(n)+(i[m]-lead(i[m]))
       /var(n)^(e-1);
       i[m]=poly(e)^e*leadcoef(i[m])^(e-1)*i[m];
//---if not (0) is returned, else c*var(n)+h is added to prm
       if (reduce(i[m]-t^e,prm,1) !=0)
       {
         return(ideal(0));
       }
       prm = prm,cleardenom(simplify(t,1));
     }
   }
   return(prm);
}

   ring s=(0,x),(d,e,f,g),lp;
   ideal i=g^5,(x*f-g)^3,5*e-g^2,x*d^3;
   PrimaryTest(i,g);
   kill s;

proc zeroDecomp(ideal i)
"USAGE:  zeroDecomp(i); i zerodimensional ideal
RETURN:  list l of lists of two ideals such that the
         intersection(l[j][1], j=1..)=i, the l[i][1] are
         primary and the l[i][2] their radicals
NOTE:    algorithm of Gianni/Trager/Zacharias
"
{
   def  BAS = basering;
//----the ordering is changed to the lexicographical one
   def R=changeord(list(list("lp",1:nvars(basering)))); setring R;
   ideal i=fetch(BAS,i);
   int n=nvars(R);
   int k;
   list result,rest;
   ideal primary,prim;
   option(redSB);

//------the random coordinatechange and its inverse
   ideal m=maxideal(1);
   m[n]=0;
   poly p=(random(100,1,n)*transpose(m))[1,1]+var(n);
   m[n]=p;
   map phi=R,m;
   m[n]=2*var(n)-p;
   map invphi=R,m;
   ideal j=groebner(phi(i));
//-------------factorization of the first element in i
   list fac=factorize(j[1],2);
//-------------computation of the primaries and primes
   for(k=1;k<=size(fac[1]);k++)
   {
     p=fac[1][k]^fac[2][k];
     primary=groebner(j+p);
     prim=PrimaryTest(primary,fac[1][k]);
//---test whether all ideals were primary and in general
//   position
     if(size(prim)==0)
     {
       rest[size(rest)+1]=i+invphi(p);
     }
     else
     {
       result[size(result)+1]=
         list(std(i+invphi(p)),std(invphi(prim)));
     }
   }
//-------treat the bad cases collected in the rest again
   for(k=1;k<=size(rest);k++)
   {
     result=result+zeroDecomp(rest[k]);
   }
   option(noredSB);
   setring BAS;
   list result=imap(R,result);
   kill R;
   return(result);
}

   ring  r = 32003,(x,y,z),dp;
   poly  p = z2+1;
   poly  q = z4+2;
   ideal i = p^2*q^3,(y-z3)^3,(x-yz+z4)^4;
   list pr= zeroDecomp(i);
   pr;
   kill r;

proc prepareQuotientring(ideal i)
"USAGE:   prepareQuotientring(i); i standard basis
RETURN:   a list l of two strings:
          l[1] to define K[x\u,u ], u a maximal independent
          set for i
          l[2] to define K(u)[x\u ], u a maximal independent
          set for i
          both rings with lexicographical ordering
"
{
  string va,pa;
//v describes the independent set u: var(j) is in
//u iff v[j]!=0
  intvec v=indepSet(i);
  int k;

  for(k=1;k<=size(v);k++)
  {
    if(v[k]!=0)
    {
      pa=pa+"var("+string(k)+"),";
    }
    else
    {
      va=va+"var("+string(k)+"),";
    }
  }

  pa=pa[1..size(pa)-1];
  va=va[1..size(va)-1];

  string newring
  ="ring nring=("+charstr(basering)+"),("+va+","+pa+"),lp;";
  string quotring
  ="ring quring=("+charstr(basering)+","+pa+"),("+va+"),lp;";
  return(newring,quotring);
}

   ring s=(0,x),(a,b,c,d,e,f,g),dp;
   ideal i=x*b*c,d^2,f-g;
   i=std(i);
   def Q=basering;
   list l= prepareQuotientring(i);
   l;
   execute (l[1]);
   basering;
   execute (l[2]);
   basering;
   setring Q;

proc prepareSat(ideal i)
{
   int k;
   poly p=leadcoef(i[1]);
   for(k=2;k<=size(i);k++)
   {
     p=p*leadcoef(i[k]);
   }
   return(p);
}

//LIB"elim.lib";
proc decomp(ideal i)
"USAGE:  decomp(i); i ideal
RETURN:  list l of lists of two ideals such that the
         intersection(l[j][1], j=1..)=i, the l[i][1] are
         primary and the l[i][2] their radicals
NOTE:    algorithm of Gianni/Trager/Zacharias
"
{
   if(size(i)==0)
   {
     return(list(i,i));
   }
   def  BAS = basering;
   ideal j;
   int n=nvars(BAS);
   int k;

   ideal SBi=std(i);
   int d=dim(SBi);
//---the trivial case and the zero-dimensional case
   if ((d==0)||(d==-1))
   {
      return(zeroDecomp(i));
   }
//---prepare the quotientring with respect to a maximal
//   independent set
   list quotring=prepareQuotientring(SBi);
   execute (quotring[1]);
//---we use this ring to compute a standardbasis of i*quring
//   which is in i
   ideal i=std(imap(BAS,i));
//---pass to the quotientring with respect to a maximal
//   independent set
   execute(quotring[2]);
   ideal i=imap(nring,i);
   kill nring;
//---computation of the zerodimensional decomposition
   list ra=zeroDecomp(i);
//---preparation for saturation
   list p;
   for(k=1;k<=size(ra);k++)
   {
     p[k]=list(prepareSat(ra[k][1]),prepareSat(ra[k][2]));
   }
   poly q=prepareSat(i);
//---back to the original ring
   setring BAS;
   list p=imap(quring,p);
   list ra=imap(quring,ra);

   poly q=imap(quring,q);

   kill quring;
//---compute the intersection of ra with BAS
   for(k=1;k<=size(ra);k++)
   {
     ra[k]=list(sat(ra[k][1],p[k][1]),
                sat(ra[k][2],p[k][2]));
   }
   q=q^sat_with_exp(i,q)[2];

//---i=intersection((i:q),(i,q)) and ra is the primary
//   decomposition of i:q
   if(deg(q)>0)
   {
     ra=ra+decomp(i+q);
   }
   return(ra);
}

   ring  r = 0,(x,y,z),dp;
   ideal i = intersect(ideal(x,y,z)^3,ideal(x-y-z)^2,
             ideal(x-y,x-z)^2);
   list pr= decomp(i);
   pr;
   kill r;

proc equidimensional(ideal i)
"USAGE:  equidimensional(i); i ideal
RETURN:  list l of two ideals such that intersetion(l[1],
         l[2])=i
         l[1] is equidimensional and dim(l[1])>dim(l[2])
"
{
   def  BAS = basering;

   ideal SBi=std(i);
   int d=dim(SBi);
   int n=nvars(BAS);
   int k;
   list result;

//-----the trivial cases
   if ((d==-1)||(n==d)||(n==1)||(d==0))
   {
      result=i,ideal(1);
      return(result);
   }
//-----prepare the quotientring with respect to a maximal
//     independent set
   list quotring=prepareQuotientring(SBi);
   execute (quotring[1]);
//-----we use this ring to compute a standardbasis of
//     i*quring which is in i
   ideal eq=std(imap(BAS,i));
//-----pass to the quotientring with respect to a maximal
//     independent set
   execute (quotring[2]);
   ideal eq=imap(nring,eq);
   kill nring;
//-----preparation for saturation
   poly p=prepareSat(eq);
//-----back to the original ring
   setring BAS;
   poly p=imap(quring,p);
   ideal eq=imap(quring,eq);
   kill quring;
//-----compute the intersection of eq with BAS
   eq=sat(eq,p);

   SBi=std(quotient(i,eq));

   if(d>dim(SBi))
   {
     result=eq,SBi;
     return(result);
   }
   result=equidimensional(i);
   result=intersect(result[1],eq),result[2];
   return(result);
}

   ring  r = 0,(x,y,z),dp;
   ideal i = intersect(ideal(x,y,z)^3,ideal(x-y-z)^2,
             ideal(x-y,x-z)^2);
   list pr= equidimensional(i);
   pr;
   dim(std(pr[1]));
   dim(std(pr[2]));
   option(redSB);
   std(i);
   std(intersect(pr[1],pr[2]));
   kill r;

proc squarefree(poly f, int i)
{
   poly h=gcd(f,diff(f,var(i)));
   poly g=lift(h,f)[1][1];    //  f/h
   return(g);
}

proc radical(ideal i)
"USAGE:  radical(i); i ideal
RETURN:  ideal = the radical of i
NOTE:    algorithm of Krick/Logar
"
{
   def  BAS = basering;
   ideal j;
   int n=nvars(BAS);
   int k;

   option(redSB);
   ideal SBi=std(i);
   option(noredSB);
   int d=dim(SBi);

//-----the trivial cases
   if ((d==-1)||(n==d)||(n==1))
   {
      return(ideal(squarefree(SBi[1],1)));
   }
//-----the zero-dimensional case
   if (d==0)
   {
      j=finduni(SBi);
      for(k=1;k<=size(j);k++)
      {
         i=i,squarefree(cleardenom(j[k]),k);
      }
      return(std(i));
   }
//-----prepare the quotientring with respect to a maximal
//     independent set
   list quotring=prepareQuotientring(SBi);
   execute (quotring[1]);
//-----we use this ring to compute a standardbasis of
//     i*quring which is in i
   ideal i=std(imap(BAS,i));
//-----pass to the quotientring with respect to a maximal
//     independent set
   execute( quotring[2]);
   ideal i=imap(nring,i);
   kill nring;
//-----computation of the zerodimensional radical
   ideal ra=radical(i);
//-----preparation for saturation
   poly p=prepareSat(ra);
   poly q=prepareSat(i);
//-----back to the original ring
   setring BAS;
   poly p=imap(quring,p);
   poly q=imap(quring,q);
   ideal ra=imap(quring,ra);
   kill quring;
//-----compute the intersection of ra with BAS
   ra=sat(ra,p);
//----now we have radical(i)=intersection(ra,radical((i,q)))
   return(intersect(ra,radical(i+q)));
}

   ring  r = 0,(x,y,z),dp;
   ideal i =
   intersect(ideal(x,y,z)^3,ideal(x-y-z)^2,ideal(x-y,x-z)^2);
   ideal j = radical(i);
   j;

tst_status(1);$
