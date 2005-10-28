LIB "tst.lib";
tst_init();


//======================  Exercise 4.1 =============================
ring R = 0, (t,w,x,y,z), dp;
ideal I = w2xy+w2xz+w2z2, tx2y+x2yz+x2z2, twy2+ty2z+y2z2, 
          t2wx+t2wz+t2z2;
if(not(defined(primdecGTZ))){ LIB "primdec.lib"; }
list L = primdecGTZ(I);

int s = size(L);   // number of components
int i,j;
for (i=1; i<=s; i++)
{ 
  L[i][1]=std(L[i][1]);
  L[i][2]=std(L[i][2]);
  // test for primeness:
  if ( size( reduce( lead(L[i][2]), std(lead(L[i][1])) ) ) > 0 )
  {
    print(string(i)+"th component is not prime and of dimension "
      +string(dim(L[i][2]))+"."); 
  }
  else
  {
    print(string(i)+"th component is prime and of dimension "
      +string(dim(L[i][2]))+"."); 
  }
}

for (i=1; i<=s; i++)
{ 
  for (j=1; j<=s; j++)
  {
    if (( dim(L[j][2]) > dim(L[i][2]) ) and
       ( size(reduce(L[j][2],L[i][2],1)) == 0 ))
    {
      print(string(i)+"th component is embedded.");
      j=s;
    }
  }
}


kill R,s,i,j;
//======================  Exercise 4.2 =============================
ring R = 0, (b,s,t,u,v,w,x,y,z), dp;
ideal I =  wy-vz, vx-uy, tv-sw, su-bv, tuy-bvz;
if(not(defined(normal))){ LIB "normal.lib"; }
list nor = normal(I);
for (int i=1; i<=size(nor); i++) { def R_nor(i) = nor[i]; } 
setring R_nor(1); 
R_nor(1); norid; normap;
setring R_nor(2); 
R_nor(2); norid; normap;
setring R_nor(3); 
R_nor(3); norid; normap;

setring R;
primdecGTZ(I);


kill R,R_nor(1),R_nor(2),R_nor(3),i,nor;
//======================  Exercise 4.3 =============================
ring R = 32003, (x,y,z,a,b,c,d), dp;
ideal I = a-xy, b-y2, c-yz, d-y3;
I = I, z2-x5-y5;
ideal J = eliminate(I,y);
ring S = 32003, (x,z,a,b,c,d), dp;
ideal J = imap(R,J); 
attrib(J,"isSB",1); 
J;
dim(J);
//-> 2
if(not(defined(normal))){ LIB "normal.lib"; }
list nor = normal(J);
def R_nor = nor[1]; setring R_nor;  
norid; normap;


kill R,S,nor,R_nor;
//======================  Exercise 4.4 =============================
ring R = 0, x(1..3), dp;
matrix D[3][3] = x(1), x(2), x(3)^2-1,
                 x(2), x(3), x(1)*x(2)+x(3)+1,
                 x(3)^2-1, x(1)*x(2)+x(3)+1, 0;
ideal I = det(D), x(1)*x(3)-x(2)^2;
matrix Df = jacob(I);
I = std(I);
int c = nvars(R) - dim(I); c;
//-> 2
if (not(defined(equidimMax))){ LIB "primdec.lib"; }
ideal I_max = equidimMax(I);
size(reduce(I_max,I));
//-> 0

ideal J = minor(Df,c), I;
J = groebner(J);
sat(I,J);
//-> [1]:
//->    _[1]=1
//-> [2]:
//->    2

ideal J_max = equidimMax(J);
size(reduce(J_max,J));
//-> 0
matrix DJ = jacob(J);
ideal SLoc = minor(DJ,c), J;
groebner(SLoc); 
//-> _[1]=1

//---------Alternatively: check sqrt(J)=J----------
ideal sqrtJ = radical(J);
size(reduce(sqrtJ,J));
//-> 0


kill R,c;
//======================  Exercise 4.5 =============================
proc maximaldegree (ideal I)
"USAGE:  maximaldegree(I);   I=ideal
RETURN: integer; the maximum degree of the given 
                 generators for I
NOTE:   return value is -1 if I=0
"
{
  if (size(I)>0)
  {
    int i,dd; 
    int d = deg(I[1]); 
    for (i=2; i<=size(I); i++)
    {
      dd = deg(I[i]);
      if (dd>d) { d = dd; }
    }
    return(d);
  }
  else
  {
    return(-1);
  }
}

proc zeros (poly f)
"USAGE:   zeros(f);   f poly
ASSUME:  the active ring is univariate
RETURN:  ring 
NOTE:    In the output ring, f decomposes into linear factors. The 
         list of solutions of f=0 may be accessed by typing, for 
         instance, 
            def RRR=zeros(f); setring RRR; ZEROS; 
"
{
  if (nvars(basering)!=1) 
    { ERROR("The active ring is not univariate"); } 
  if (deg(f)<=0){ ERROR("f is a constant polynomial"); } 
  def R_aux = basering;
  ideal facts_f = factorize(f,1);
  int d = maximaldegree(facts_f);
  int counter=size(facts_f);
  int i;
  while (d>1) 
  {
    if (defined(primitive)==0){ LIB "primitiv.lib"; }
    while (deg(facts_f[counter])<=1) { counter--; }
    if (defined(minpoly)) { poly g = minpoly; }
      else { poly g=0; }
    poly h = facts_f[counter];
    if (g==0)
    {
      ring R_aux2 = (char(basering),a), x, dp;
      map psi = R_aux,a; 
      minpoly = number(psi(h));
      ideal facts_old = imap(R_aux,facts_f);
      ideal facts_f;
      for (i=1; i<=size(facts_old); i++)
      {
        facts_f = facts_f, factorize(facts_old[i],1);
      }  
    }
    else
    {
      ring R_aux1 = char(basering), (a,x), dp;
      poly g = imap(R_aux,g);
      poly h = imap(R_aux,h);
      ideal facts_f = imap(R_aux,facts_f);
      ideal I = g,h;
      ideal Prim = primitive(I);
      poly MP_new = Prim[1];
      poly a_new = Prim[2];
      poly x_new = Prim[3];
      ring R_aux2 = (char(basering),a), x, dp;
      map psi = R_aux1,0,a; 
      minpoly = number(psi(MP_new));
      poly a_new = psi(a_new);
      poly x_new = psi(x_new);
      map phi = R_aux1,a_new,x;
      ideal facts_old = phi(facts_f);
      ideal facts_f;
      for (i=1; i<=size(facts_old); i++)
      {
        facts_f = facts_f, factorize(facts_old[i],1);
      }
      kill R_aux1;
    }
    facts_f = simplify(facts_f,2);
    kill R_aux;
    def R_aux = R_aux2;
    setring R_aux;
    kill R_aux2;
    d = maximaldegree(facts_f);
  }
  ideal ZEROS = -subst(facts_f,x,0);
  export(ZEROS);
  return(R_aux);
}

ring R = 167, x, dp;
def RRR = zeros(x100-13);
setring RRR;
ZEROS;
poly g = 1;
for (int i=1; i<=size(ZEROS); i++) { g = g*(x-ZEROS[i]); }
g;
//-> x100-13


kill i,RRR,R;
//======================  Exercise 4.6 =============================
ring R = 0, (x,y,z), dp;
ideal MI_2 = maxideal(2);
ideal MI_3 = maxideal(3);
ideal MI_4 = maxideal(4);
ring R_ext = 0, (a(1..6),b(1..6),c(1..6),x,y,z), dp;
ideal MI_2 = imap(R,MI_2);
matrix A[6][1] = a(1..6);
matrix B[6][1] = b(1..6);
matrix C[6][1] = c(1..6);
poly F(0) = (matrix(MI_2)*A)[1,1]; 
poly F(1) = (matrix(MI_2)*B)[1,1]; 
poly F(2) = (matrix(MI_2)*C)[1,1]; 
ideal G = x2*F(0), xy*F(0), xz*F(0), yz*F(0), x2*F(1),
          xy*F(1), xz*F(1), y2*F(1), yz*F(1), x2*F(2),
          xy*F(2), xz*F(2), y2*F(2), yz*F(2), z2*F(2);
ideal MI_4 = imap(R,MI_4);
matrix M = coeffs(G,MI_4,xyz);
poly D_0 = det(M);   
deg(D_0); size(D_0);
//-> 15
//-> 37490

ideal G_ext = x2*F(1), x2*F(2), y2*F(2);
ideal MI_ext = x2y2, x2z2, y2z2;
matrix M_ext = coeffs(G_ext,MI_ext,xyz);
poly D_0_ext = det(M_ext);
poly Res = D_0/D_0_ext;
deg(Res); size(Res);
//-> 12
//-> 21894


// session continued.....
//======================  Exercise 4.7 =============================
ring S = 0,(x,y,z,a(1..10)), (dp(2),dp(11));
ideal MI_2 = imap(R,MI_2);
ideal MI_3 = imap(R,MI_3);
matrix A[10][1] = a(10),a(9),a(7),a(4),a(8),a(6),a(3),a(5),a(2),
                  a(1);
poly F = (matrix(MI_3)*A)[1,1]; 
ideal J = diff(F,x), diff(F,y), diff(F,z);
if (not(defined(flatten))){ LIB "matrix.lib"; }
map phi = R_ext, flatten(coeffs(diff(F,x),MI_2,xyz)),
                 flatten(coeffs(diff(F,y),MI_2,xyz)),
                 flatten(coeffs(diff(F,z),MI_2,xyz));
poly D = phi(Res);              
deg(D); size(D);
//-> 12
//-> 2040   

tst_status(1);$

