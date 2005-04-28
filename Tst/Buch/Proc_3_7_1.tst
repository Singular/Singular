LIB "tst.lib";
tst_init();

proc finitenessTest(ideal I)
"USAGE: finitenessTest(ideal I)
RETURN: A list l, l[1] is 1 or 0 and l[2] is an ideal gener-    
        ated by a subset of the variables. l[1]=1 if the map
        basering/I <-- K[l[2]] is finite and 0 else.
NOTE:   It is assumed that I is a reduced standard basis 
        with respect to the lexicographical ordering lp, 
        sorted w.r.t. increasing leading terms.
"
{
  intvec w=leadexp(I[1]);
  int j,t;
  int s=1;
  ideal k;  
  //----------- check leading exponents ----------------------
  //compute s such that lead(I[1]) depends only on
  //var(s),...,var(n) by inspection of the leading exponents  
  while (w[s]==0) {s++;}
  for (j=1; j<= size(I); j++)
  {
    w=leadexp(I[j]);
    if (size(ideal(w))==1) {t++;}
  }
  //----------------check finiteness -------------------------
  //t is the number of elements of the standard basis which 
  //have pure powers in the variables var(1),...,var(s) as 
  //leading term. The map is finite iff s=t.
  if(s!=t) {return(list(0,k));}
  for (j=s+1; j<= nvars(basering);j++)
  {
    k[j]=var(j);
  }
  return (list(1,k));
}


proc noetherNormal(ideal id)
"USAGE:  noetherNormal(id);  id ideal
RETURN:  two ideals i1,i2, where i2 is given by a subset of 
         the variables and i1 defines a map: 
         map phi=basering,i1 such that 
         k[i2] --> k[var(1),...,var(n)]/phi(id) 
         is a Noether normalisation 
"
{
   def r=basering;
   int n=nvars(r);
//----- change to lexicographical ordering ------------
   //a procedure from ring.lib changing the order to lp 
   //creating a new basering s
   def s=changeord("lp");
   setring s;
//----- make a random coordinate change ----------------
   //creating lower triangular random generators for the 
   //maximal ideal a procedure form random.lib
   ideal m=
   ideal(sparsetriag(n,n,0,100)*transpose(maxideal(1)));

   map phi=r,m;
   ideal i=std(phi(id));
//---------- check finiteness ---------------------------
   //from theoretical point of view Noether normalisation 
   //should be o.k. but we need a test whether the 
   //coordinate change was random enough
   list l=finitenessTest(i);

   setring r;
   list l=imap(s,l);

   if(l[1]==1)
   {
      //the good case, coordinate change was random enough
      return(list(fetch(s,m),l[2]));
   }
   kill s;
   //-------- the bad case, try again ---------------------
   return(noetherNormal(i));
}

LIB"ring.lib";
LIB"random.lib";
ring R=0,(x,y,z),dp;
ideal I = xy,xz;
noetherNormal(I);

tst_status(1);$
