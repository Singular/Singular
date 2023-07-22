LIB "tst.lib";
tst_init();


//======================  Exercise 1.1 =============================
ring R;
R;
poly f = x^4+x^3*z+x^2*y^2+y*z^4+z^5;
f;
ring S = 32003, (x,y,z), lp;
poly g = fetch(R,f);
g;


kill R,S;
//======================  Exercise 1.2 =============================
ring R = 32003, x(1..5), lp;
int d = 5;
ideal MId = maxideal(d);
int s = size(MId);
int i,j;
ideal I;
for (i=1; i<=10; i++)
{
  poly f(i);
  for (j=1; j<=s; j++)
  {
    f(i) = f(i)+random(0,32002)*MId[j];
  }
  I = I, f(i);
}

// -------------Alternatively--------------
kill I;
if (not(defined(randomid))) { LIB "random.lib"; }
ideal I = randomid(maxideal(d),10,32002);
// ----------------------------------------

int aa = timer;
ideal II = groebner(I);
size(II);
II;
deg(II[1]);
deg(II[size(II)]);
write (":w lexGB.out",II);
ring R1 = 32003, x(1..5), dp;
ideal I = imap(R,I);
aa = timer;
ideal II = std(I);
size(II);
II;
deg(II[1]);
deg(II[size(II)]);
write (":w dpGB.out",II);


kill R,R1,aa,i,j,s,d;
//======================  Exercise 1.3 =============================
ring R = 0, x(0..4), dp;
matrix M[2][4] = x(0),x(1),x(2),x(3),
                 x(1),x(2),x(3),x(4);
ideal I = minor(M,2);
resolution rI = mres(I,0);
print(betti(rI),"betti");
for (int i=1; i<=3; i++)
{
  i,"th syzygy matrix: ";
  "--------------------- ";
  print(rI[i]);
  "";
  "has data type : ", typeof(rI[i]);
  "";
}
ideal GI = groebner(I);
hilb(GI);
matrix JM = jacob(I);
int codimI = nvars(R) - dim(GI);
ideal singI = minor(JM,codimI) + I;
nvars(R) - dim(groebner(singI));
//-> 5

// -------------Alternatively--------------
if (not(defined(slocus))){ LIB "sing.lib"; }
nvars(R) - dim(groebner(slocus(I)));
//-> 5
// ----------------------------------------


kill R,i,codimI;
//======================  Exercise 1.4 =============================
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

ring R = 32003, x(1..5), lp;
string xxx = "ideal II="+read("lexGB.out")+";";
execute(xxx);
maximaldegree(II);
xxx = "ideal JJ="+read("dpGB.out")+";";
execute(xxx);
maximaldegree(JJ);


kill R,xxx;
//======================  Exercise 1.5 =============================
ring P2 = 0, (u,v,w), dp;
ideal emb = u2, v2, w2, uv, uw, vw;
ideal I0 = ideal(0);
ring P5 = 0, x(0..5), dp;
ideal VP5 = preimage(P2, emb, I0);
print(betti(list(VP5)),"betti");

ideal p = x(0), x(1), x(2), x(3)-x(5), x(4)-x(5);
size(reduce(VP5,groebner(p),1));
ring P4 = 0, x(0..4), dp;
ideal VP4 = preimage(P5,p,VP5);
print(betti(list(VP4)),"betti");
// ---------- Check Smoothness ------------
if(not(defined(slocus))){ LIB "sing.lib"; }
nvars(P4) - dim(groebner(slocus(VP4)));
//-> 5
// ----------------------------------------

if(not(defined(sat))){ LIB "elim.lib"; // loads random.lib, too }
ideal CI1 = randomid(VP4,2,100);
ideal QES = sat(CI1,VP4);
resolution FQES = mres(QES,0);
print(betti(FQES),"betti");
// ---------- Check Smoothness ------------
nvars(P4) - dim(groebner(slocus(QES)));
//-> 5
// ----------------------------------------

setring P5;
ideal q = x(0)-x(1), x(1)-x(2), x(2)-x(3), x(3)-x(4), x(4)-x(5);
size(reduce(VP5,groebner(q),1));
setring P4;
ideal CS = preimage(P5,q,VP5);
print(betti(list(CS)),"betti");
// ---------- Check Smoothness ------------
nvars(P5) - dim(groebner(slocus(CS)));
//-> 6
// ----------------------------------------
ideal CI2 = matrix(CS)*randommat(3,2,maxideal(1),100);
ideal B = sat(CI2,CS);
print(betti(list(B)),"betti");

setring P5;
ideal L = x(0)+x(1)+x(2), x(3), x(4), x(5);
nvars(P5) - dim(groebner(VP5+L));
//-> 6
ring P3 = 0, y(0..3), dp;
ideal SRS = preimage(P5,L,VP5); SRS;
//-> y(1)^2*y(2)^2-y(0)*y(1)*y(2)*y(3)+y(1)^2*y(3)^2+y(2)^2*y(3)^2

tst_status(1);$

