LIB "tst.lib";
tst_init();

LIB "gaussman.lib";


list d=

// simple singularities

"A_{1}","x,y","ds","x2+y2",
"A_{2}","x,y","ds","x3+y2",
"A_{3}","x,y","ds","x4+y2",
"A_{4}","x,y","ds","x5+y2",
"A_{5}","x,y","ds","x6+y2",
"A_{6}","x,y","ds","x7+y2",
"A_{7}","x,y","ds","x8+y2",
"A_{8}","x,y","ds","x9+y2",

"D_{4}","x,y","ds","x2y+y3",
"D_{5}","x,y","ds","x2y+y4",
"D_{6}","x,y","ds","x2y+y5",
"D_{7}","x,y","ds","x2y+y6",
"D_{8}","x,y","ds","x2y+y7",
"D_{9}","x,y","ds","x2y+y8",
"D_{10}","x,y","ds","x2y+y9",

"E_{6}","x,y","ds","x3+y4",
"E_{7}","x,y","ds","x3+xy3",
"E_{8}","x,y","ds","x3+y5",

// unimodal singularities

// parabolic

"P_{8}","x,y,z","ds","x3+y3+z3",
"X_{9}","x,y","ds","x4+y4",
"J_{10}","x,y","ds","x3+y6",

// hyperbolic

"T_{3,4,5}","x,y,z","ds","x3+y4+z5+xyz",
"T_{3,4,6}","x,y,z","ds","x3+y4+z6+xyz",
"T_{4,5,6}","x,y,z","ds","x4+y5+z6+xyz",

// 14 exceptional families

"E_{12}","x,y","ds","x3+y7",
"E_{13}","x,y","ds","x3+xy5",
"E_{14}","x,y","ds","x3+y8",

"Z_{11}","x,y","ds","x3y+y5",
"Z_{12}","x,y","ds","x3y+xy4",
"Z_{13}","x,y","ds","x3y+y6",

"W_{12}","x,y","ds","x4+y5",
"W_{13}","x,y","ds","x4+xy4",

"Q_{10}","x,y,z","ds","x3+y4+yz2",
"Q_{11}","x,y,z","ds","x3+y2z+xz3",
"Q_{12}","x,y,z","ds","x3+y5+yz2",

"S_{11}","x,y,z","ds","x4+y2z+xz2",
"S_{12}","x,y,z","ds","x2y+y2z+xz3",

"U_{12}","x,y,z","ds","x3+y3+z4";


proc tst_gaussm(poly f)
{
  basering;
  f;
  print(monodromy(f));
  list l=vwfilt(f);
  l;
  l=spnf(l[1],l[3],l[4])+list(l[5],l[6]);
  endvfilt(l);
  spgamma(l);
}


string c;
int i;
for(i=1;i<=size(d);i=i+4)
{
  c="ring R=0,("+d[i+1]+"),"+d[i+2]+";";
  execute(c);
  c="poly f="+d[i+3]+";";
  execute(c);

  tst_gaussm(f);

  kill R;
}

tst_status(1);
$
