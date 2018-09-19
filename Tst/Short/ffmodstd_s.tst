//
LIB "tst.lib";
LIB "ffmodstd.lib";
tst_init();

proc tst_test_ffmodstd(ideal I)
{
   ideal Jtst = ffmodStd(I);
   Jtst;
}
ring R = (0,t(1..4)),(x(1..6)),dp;
poly f1 = (96*t(1)+59)/(13*t(1)^2)*x(3)*x(4)^2*x(6)
          +10/(23*t(1)^3+4*t(1))*x(3)*x(5)^2*x(6);
poly f2 = (1165*t(1)^2)/(252*t(2)^2)*x(1)*x(2)^3
          +(89*t(2)^2)/(82*t(1)^3)*x(1)*x(2);
poly f3 = (10*t(2)^3)/(33*t(1)^3)*x(1)^3
          +(5*t(2))/(41*t(3))*x(1);
poly f4 = (3977*t(1)^3+533*t(1)^2*t(2)+2173*t(1)*t(2)^2
          +806*t(1)*t(2)+3286*t(2)^2)/(3977*t(1)^2
          +6014*t(1))*x(1)^3+(22*t(1)*t(2)^2)/(5*t(1)^2+
	      61*t(2)^2)*x(2)^3+(42*t(1)^3
	      +65*t(2)^3)/(65*t(1)^2*t(2)+47*t(1))*x(1)^2;
poly f5 =  (3741*t(1)^4+94)/43*x(2)*x(3)^2
           +(8*t(1)^2)/21*x(3)*x(6)^2+(54*t(1)^2)*x(3)^2;
poly f6 = (96*t(1)^2*t(2)^2+41*t(1)*t(2)^2*t(4))/(95*t(1)^2*t(3)^2
          +32*t(2)^3*t(4))*x(1)*x(2)*x(3)*x(5)
	      +(31*t(1)*t(2)*t(3)^2+11*t(2)^2*t(3)*t(4))/(39*t(1)^4+
	      51*t(1)^3*t(2))*x(3)^3*x(5)+(21*t(1)*t(4)
	      +90*t(3)^2)/(37*t(1)*t(3)+62*t(2)^2)*x(3)*x(4)*x(5)^2
	      +(63*t(1)^2*t(2)*t(3)+27*t(2)^4)/(16*t(1)^3*t(3)
	      +67*t(2)^4)*x(2)*x(3)^2;
poly f7 = (726*t(1)^4+205*t(1)*t(2)^4)/(110*t(2)^3)*x(1)^2
          +(61*t(2)^2)/(36*t(1)^2)*x(1)*x(3);
ideal I = f1,f2,f3,f4,f5,f6,f7;

tst_test_ffmodstd(I);
kill R;
ring Ra=(0,a),(x,y,z),dp;
    ideal I = (a^2+2)*x^2*y+a*y*z^2, x*z^2+(a+1)*x^2-a*y^2;
tst_test_ffmodstd(I);
kill R;
ring R=(0,a,b),(x,y,z),dp;
ideal I = x^2*y^3*z+2*a*x*y*z^2+7*y^3,
              x^2*y^4*z+(a-7b)*x^2*y*z^2-x*y^2*z^2+2*x^2*y*z-12*x+by,
              (a2+b-2)*y^5*z+(a+5b)*x^2*y^2*z-b*x*y^3*z-x*y^3+y^4+2*a2*y^2*z,
              a*x^2*y^2*z-x*y^3*z+3a*x*y*z^3+(-a+4)*y^3*z^2+4*z^2-bx;
tst_test_ffmodstd(I);
kill R;
proc tst_test_polyInterpolation(list l, list m, list #)
{
   def F = polyInterpolation(l,m,#);
   F;
}

ring R = 0,x,dp;
list l, m;
int i,j;
j=5;
poly g = x12+6x10+15x8+21x6+20x4+18x2+9;
g = g**3 + g**2;
for(i=1;i<=deg(g)+1;i++)
{
    l[i] = number(subst(g, var(1), j));
    m[i] = j;
    j = j+2;
}
list G = m,l;
tst_test_polyInterpolation(G[1],G[2]);
kill R;

proc tst_test_fareypoly(poly g, poly f)
{
   def F = fareypoly(g,f);
   F;
}

ring R = 0,x,dp;
poly g = x+2;
poly f = x+5;
poly f1 = g**2*(2*g+1);
poly f2 = f**3*(5*f+3);
poly g1 = f1**2*(2*f1+1);
poly g2 = f2**3*(5*f2+3);

poly V = (x+1)**40;
poly B = extgcd(g2,V)[2];
poly D = B*g1;
D = reduce(D, std(V));
tst_test_fareypoly(D, V);
kill R;

proc tst_test_modrationalInterpolation(list D, list E, int vr)
{
   def F = modrationalInterpolation(D,E,vr);
   F;
}

ring R = 0,x,dp;
poly r = 16/3125*x^9+312/3125*x^8+108/125*x^7+13614/3125*x^6+44092/3125*x^5+95153/3125*x^4
         +136872/3125*x^3+126584/3125*x^2+13664/625*x+656/125;
poly t = x^16+412/5*x^15+79554/25*x^14+9556508/125*x^13+799356506/625*x^12
         +1974684999/125*x^11   +93143851156/625*x^10+3422950835956/3125*x^9
         +3961758888729/625*x^8+3622442847166/125*x^7+2607940162054/25*x^6
         +1462807349256/5*x^5+626672547956*x^4+991121483395*x^3+1091504397900*x^2
         +747836138000*x+240141160000;
list D, E;
int k1,  k2;
k2 = 5;
for(k1 = 1; k1 <= deg(r) + deg(t) +3; k1++)
{
     k2 = random(k2 +1, k2+5);
     D[k1] = k2;
}
for (k1 = 1; k1 <= size(D); k1++)
{
    E[k1] = number(subst(r, x, D[k1]))/number(subst(t, x, D[k1]));
}
tst_test_modrationalInterpolation(D, E, 1);
kill R;

proc tst_test_BerlekampMassey(list L, int i, list #)
{
   def F = BerlekampMassey(L,i, #);
   F;
}

proc tst_test_modberlekampMassey(list L)
{
   def F = modberlekampMassey(L);
   F;
}

proc tst_test_sparseInterpolation(poly Br, list La, list lpr, int n, list #)
{
   def F = sparseInterpolation(Br, La, lpr, n, #);
   F;
}

ring R = 0, (x,y,z), dp;
poly f = xy + yz + x3 + xz + y2 + z2;
f = f*(5*f+3);
list lpr = 3,5,7;
list La;
int i;
int sz = size(f);
for(i=0;i<=2*sz;i++)
{
   La[i+1] = number(subst(f,x,bigint(lpr[1])^i,y,bigint(lpr[2])^i, z, bigint(lpr[3])^i));
}

tst_test_BerlekampMassey(La,1);
tst_test_modberlekampMassey(La);


poly Br = x24-15302x23+108153538x22-469822785006x21+1407508896239640x20-3091943125563504810x19
          +5168112767194393020422x18-6729723784556809568734354x17+6931464551637490752291339929x16
          -5699848400675330822165442096828x15+3760033336187084132321077964447220x14
          -1991661667180376045309477779945538700x13+844842071446573758035869063546129683000x12
          -285178513120947332205887203363460453332500x11
          +75811568321033868929115724018317018412987500x10
          -15635458692849224898228692620395578209262062500x9
          +2451053560315230678380665790179304182681733671875x8
          -284379998009191272424401092954106259377042386718750x7
          +23641079761297766943853576941070123797406993066406250x6
          -1360130446827557647250743113079186076430133454589843750x5
          +52556801545000906318441420070679293954946411621093750000x4
          -1321635241786467719242433527312853738727799653625488281250x3
          +20572945477752454884036085702463299663396845817565917968750x2
          -178934876180957551616320723528162371797337555885314941406250x
          +661370587133842412643287880417356305385582149028778076171875;

tst_test_sparseInterpolation(Br, La, lpr, 0, 0);
tst_status(1);$
