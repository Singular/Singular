LIB "tst.lib";
tst_init();

LIB"algebra.lib";
proc isCohenMacaulay(ideal I)
{
   def A    = basering;
   list L   = noetherNormal(I);
   map phi  = A,L[1];
   I        = phi(I);
   int s    = nvars(basering)-size(L[2]);
   execute("ring B=("+charstr(A)+"),x(1..s),ds;");
   ideal m  = maxideal(1);
   map psi  = A,m;
   ideal J  = std(psi(I));
   ideal K  = kbase(J);
   setring A;
   execute("ring C=("+charstr(A)+"),("+varstr(A)+"),(dp(s),ds);");
   ideal I  = imap(A,I);
   qring D  = std(I);
   ideal K  = fetch(B,K);
   module N = std(syz(K));
   intvec v = leadexp(N[size(N)]);
   int i=1;
   while((i<s)&&(v[i]==0)){i++;}
   if(!v[i]){return(0);}
   return(1);
}

ring r=0,(x,y,z),ds;
ideal I=xz,yz;
isCohenMacaulay(I);

I=x2-y3;
isCohenMacaulay(I);

tst_status(1);$
