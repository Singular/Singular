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
  // ----------- check leading exponents ----------------------
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

ring R=0,(x,y,z),lp;
ideal I = y2+z3,x3+xyz;
finitenessTest(I);

tst_status(1);$
