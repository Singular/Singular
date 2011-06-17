LIB "tst.lib";
tst_init();

proc testfactors (list l, poly f)
{
  poly g= 1;
  for (int i= 1; i <= size (l[1]); i++)
  {
    g= g*(l[1][i]^l[2][i]);
  }
  g == f;
  l;
}



// polys from L. Bernardin's thesis
ring r= 17,(x,y),dp;
list l;
poly f;
for (int n=2; n <= 700; n++)
{
  f= x^n*y^n+x^((n div 2)+1)*y^(n div 2)*(y+1)+x^2*y+(n+1)*x*y+(n^2+3)*x-2;
  f;
  l= factorize (f); testfactors (l, f);
}
tst_status();

kill r;

ring r= 31991,(z,w),dp;

list l;

poly f= 10582*w^3*z^4+21325*z^4+29620*w^4*z^3+23697*w*z^3+12439*w^6*z^2+3572*w^3*z^2+5463*z^2+16590*w*z+24885*w^3+31963;

l= factorize (f); testfactors (l, f);

kill r;

ring r= 3, (s,t),dp;
list l;

poly f= s^85+(t)*s^84+(t^2)*s^83+(-1*t^18)*s^67+(-1*t^19)*s^66+(t^20)*s^65
+(t^21)*s^64+(t^22)*s^63+(-1*t^24)*s^61+(-1*t^25)*s^60+(t^29)*s^56
+(t^38)*s^47+(-1*t^39)*s^46+(-1*t^40)*s^45+(-1*t^42)*s^43+(-1*t^43)*s^42
+(-1*t^45)*s^40+(-1*t^46)*s^39+(t^47)*s^38+(t^56)*s^29+(-1*t^60)*s^25
+(-1*t^61)*s^24+(t^63)*s^22+(t^64)*s^21+(t^65)*s^20+(-1*t^66)*s^19
+(-1*t^67)*s^18+(t^83)*s^2+(t^84)*s+(t^85);

l= factorize (f); testfactors (l, f);

kill r;

ring r= 32003, (f,g,v,y,u,x), dp;
list l;
poly h= f^3*u^3-4*f^2*g*v*y*u^2 + (8*f^3*v^2*y+6*f^2*g*v^2*x+4*f*g^2*v^2*y^2)*u
-8*f^3*v^3*x-4*f*g^2*v^3*x*y + g^3*v^3*x^2;

l= factorize (h); testfactors (l, h);

kill r;
ring r= 32003, (a,b,c,f), dp;
list l;
poly h= a^3*b^3*f^4+186*a^2*b^4*c*f^3+11532*a*b^5*c^2*f^2+14307*b^6*c^3*f;

l= factorize (h); testfactors (l, h);

kill r;
ring r= 32003,(x,y,z),dp;
list l;
poly f= x^25+y^23+z^18+x^4*y^10+x^8*y^5+2*x^5*y^8+x^5*y^5*z^3+x^6*y^6;

l= factorize (f); testfactors (l, f);

kill r;
ring r= 32003, (x,y,z,w),dp;
list l;
poly f=
-573002*x^10+240517*x^9*y+396389*x^8*y^2+535082*x^7*y^3+288429*x^6*y^4
+763919*x^5*y^5+3766*x^4*y^6-649376*x^3*y^7-59851*x^2*y^8-542799*x*y^9+20979*y^10
-461020*x^9*z-650459*x^8*y*z-944957*x^7*y^2*z+1677170*x^6*y^3*z+1666154*x^5*y^4*z
-1232710*x^4*y^5*z+862729*x^3*y^6*z-618762*x^2*y^7*z-705768*x*y^8*z-187036*y^9*z-741032*x^8*z^2
+1446654*x^7*y*z^2-643320*x^6*y^2*z^2-1635676*x^5*y^3*z^2-189420*x^4*y^4*z^2+682529*x^3*y^5*z^2
-450284*x^2*y^6*z^2-327970*x*y^7*z^2+66106*y^8*z^2+386688*x^7*z^3+198264*x^6*y*z^3-757313*x^5*y^2*z^3+2008508*x^4*y^3*z^3-675589*x^3*y^4*z^3-2338517*x^2*y^5*z^3+413997*x*y^6*z^3+150157*y^7*z^3
+115299*x^6*z^4+1838075*x^5*y*z^4-821489*x^4*y^2*z^4-772496*x^3*y^3*z^4+94982*x^2*y^4*z^4
+552427*x*y^5*z^4+894534*y^6*z^4-173749*x^5*z^5-1322895*x^4*y*z^5+545850*x^3*y^2*z^5
+1506535*x^2*y^3*z^5+31641*x*y^4*z^5+858761*y^5*z^5+188464*x^4*z^6+676365*x^3*y*z^6
-1175321*x^2*y^2*z^6-94611*x*y^3*z^6+365391*y^4*z^6-300517*x^3*z^7-74262*x^2*y*z^7
+756469*x*y^2*z^7+702099*y^3*z^7+651058*x^2*z^8+261997*x*y*z^8+70610*y^2*z^8-493574*x*z^9
+129784*y*z^9+265065*z^10-532141*x^9*w-285474*x^8*y*w-1300132*x^7*y^2*w+908974*x^6*y^3*w
-1966687*x^5*y^4*w-127968*x^4*y^5*w+58220*x^3*y^6*w+41679*x^2*y^7*w+479035*x*y^8*w+466832*y^9*w
+1566035*x^8*z*w+1817709*x^7*y*z*w-688806*x^6*y^2*z*w-748313*x^5*y^3*z*w-1930577*x^4*y^4*z*w
-957911*x^3*y^5*z*w+81915*x^2*y^6*z*w+133232*x*y^7*z*w+980198*y^8*z*w-110745*x^7*z^2*w
-1370738*x^6*y*z^2*w-1182195*x^5*y^2*z^2*w+3215028*x^4*y^3*z^2*w-2006230*x^3*y^4*z^2*w
+2364130*x^2*y^5*z^2*w+1457270*x*y^6*z^2*w+1484168*y^7*z^2*w+1648545*x^6*z^3*w+772215*x^5*y*z^3*w
+554722*x^4*y^2*z^3*w+667926*x^3*y^3*z^3*w+375356*x^2*y^4*z^3*w+3601160*x*y^5*z^3*w
+625131*y^6*z^3*w-2510503*x^5*z^4*w-2729919*x^4*y*z^4*w+1243946*x^3*y^2*z^4*w
+2503349*x^2*y^3*z^4*w+1909123*x*y^4*z^4*w+588366*y^5*z^4*w+94641*x^4*z^5*w+1274333*x^3*y*z^5*w
+1615834*x^2*y^2*z^5*w-1286036*x*y^3*z^5*w-491438*y^4*z^5*w-864129*x^3*z^6*w+96287*x^2*y*z^6*w
-1641596*x*y^2*z^6*w-479951*y^3*z^6*w+865513*x^2*z^7*w+37697*x*y*z^7*w-425879*y^2*z^7*w
-246899*x*z^8*w-1043521*y*z^8*w-615277*z^9*w-353183*x^8*w^2+1903124*x^7*y*w^2+307847*x^6*y^2*w^2
+153254*x^5*y^3*w^2+4494*x^4*y^4*w^2+775116*x^3*y^5*w^2+84747*x^2*y^6*w^2+1970329*x*y^7*w^2
+248796*y^8*w^2-200676*x^7*z*w^2-1283772*x^6*y*z*w^2+312236*x^5*y^2*z*w^2+258350*x^4*y^3*z*w^2
+899318*x^3*y^4*z*w^2+879951*x^2*y^5*z*w^2-197099*x*y^6*z*w^2+1027373*y^7*z*w^2
-939408*x^6*z^2*w^2+1231368*x^5*y*z^2*w^2+811932*x^4*y^2*z^2*w^2+136753*x^3*y^3*z^2*w^2
-1060314*x^2*y^4*z^2*w^2-373787*x*y^5*z^2*w^2-419717*y^6*z^2*w^2-602528*x^5*z^3*w^2
-279785*x^4*y*z^3*w^2+140803*x^3*y^2*z^3*w^2+1245653*x^2*y^3*z^3*w^2+611906*x*y^4*z^3*w^2
-1166551*y^5*z^3*w^2-878201*x^4*z^4*w^2+597628*x^3*y*z^4*w^2-220458*x^2*y^2*z^4*w^2
-3038116*x*y^3*z^4*w^2-2785259*y^4*z^4*w^2-425903*x^3*z^5*w^2-316206*x^2*y*z^5*w^2
-3764903*x*y^2*z^5*w^2-2194636*y^3*z^5*w^2-185521*x^2*z^6*w^2+419827*x*y*z^6*w^2
-70946*y^2*z^6*w^2+561958*x*z^7*w^2-513594*y*z^7*w^2-487215*z^8*w^2-902070*x^7*w^3
+260239*x^6*y*w^3-1618965*x^5*y^2*w^3+1123916*x^4*y^3*w^3-1107669*x^3*y^4*w^3-1433250*x^2*y^5*w^3
+665446*x*y^6*w^3-1463775*y^7*w^3+778712*x^6*z*w^3-80174*x^5*y*z*w^3-93922*x^4*y^2*z*w^3
+1249290*x^3*y^3*z*w^3+1272307*x^2*y^4*z*w^3+513673*x*y^5*z*w^3-322676*y^6*z*w^3
-634533*x^5*z^2*w^3-1875741*x^4*y*z^2*w^3-2698209*x^3*y^2*z^2*w^3+937176*x^2*y^3*z^2*w^3
-2883756*x*y^4*z^2*w^3-1430757*y^5*z^2*w^3+485806*x^4*z^3*w^3+731831*x^3*y*z^3*w^3
+2196178*x^2*y^2*z^3*w^3-1491104*x*y^3*z^3*w^3-1456373*y^4*z^3*w^3-206031*x^3*z^4*w^3+314576*x^2*y*z^4*w^3-225867*x*y^2*z^4*w^3-2750466*y^3*z^4*w^3-1087247*x^2*z^5*w^3+2172840*x*y*z^5*w^3
-541045*y^2*z^5*w^3+624945*x*z^6*w^3+1085162*y*z^6*w^3+565094*z^7*w^3-457315*x^6*w^4
+342040*x^5*y*w^4-681000*x^4*y^2*w^4+297164*x^3*y^3*w^4+689*x^2*y^4*w^4+529745*x*y^5*w^4
-1310613*y^6*w^4-786298*x^5*z*w^4-1023635*x^4*y*z*w^4-2098764*x^3*y^2*z*w^4+1733995*x^2*y^3*z*w^4
+1561806*x*y^4*z*w^4+133720*y^5*z*w^4-240204*x^4*z^2*w^4+1124674*x^3*y*z^2*w^4
+2411690*x^2*y^2*z^2*w^4-600532*x*y^3*z^2*w^4-71275*y^4*z^2*w^4+128215*x^3*z^3*w^4
+567626*x^2*y*z^3*w^4+973911*x*y^2*z^3*w^4-562780*y^3*z^3*w^4-894759*x^2*z^4*w^4
+1265582*x*y*z^4*w^4+509433*y^2*z^4*w^4-671007*x*z^5*w^4+345729*y*z^5*w^4+80717*z^6*w^4
+1144765*x^5*w^5+137320*x^4*y*w^5+390809*x^3*y^2*w^5+207350*x^2*y^3*w^5+65943*x*y^4*w^5
-1283191*y^5*w^5-109804*x^4*z*w^5+951743*x^3*y*z*w^5-154107*x^2*y^2*z*w^5-394628*x*y^3*z*w^5
-3201737*y^4*z*w^5+2307230*x^3*z^2*w^5-1082386*x^2*y*z^2*w^5-240669*x*y^2*z^2*w^5
-1274548*y^3*z^2*w^5-1768397*x^2*z^3*w^5-2788826*x*y*z^3*w^5+1689141*y^2*z^3*w^5
-1051474*x*z^4*w^5+126201*y*z^4*w^5-789876*z^5*w^5-288558*x^4*w^6-697110*x^3*y*w^6
+659535*x^2*y^2*w^6-1659535*x*y^3*w^6+371290*y^4*w^6-1605798*x^3*z*w^6+63599*x^2*y*z*w^6
+273476*x*y^2*z*w^6-774681*y^3*z*w^6-113162*x^2*z^2*w^6+861980*x*y*z^2*w^6
+1824646*y^2*z^2*w^6-275200*x*z^3*w^6+1083821*y*z^3*w^6+803980*z^4*w^6
+912560*x^3*w^7-373953*x^2*y*w^7+635559*x*y^2*w^7+2023871*y^3*w^7-252773*x^2*z*w^7
-1013176*x*y*z*w^7-356255*y^2*z*w^7+412544*x*z^2*w^7-199680*y*z^2*w^7+190561*z^3*w^7
+33346*x^2*w^8-1189055*x*y*w^8+246983*y^2*w^8-220965*x*z*w^8-273745*y*z*w^8-493392*z^2*w^8
+411602*x*w^9+853400*y*w^9+1110886*z*w^9-580548*w^10;

l= factorize (f); testfactors (l, f);

kill r;
ring r= 31991,(x,y,z,w),dp;
list l;
poly f= (-15*y^2*z^16+29*w^4*x^12*y^12*z^3+21*x^3*z^2+3*w^15*y^20)*(-1*z^31-w^12*z^20+y^18-y^14+x^2*y^2+x^21+w^2);

l= factorize (f); testfactors (l, f);

f= (w^4*z^3-x*y^2*z^2-w^4*x^5*y^6-w^2*x^3*y)*(-1*x^5-z^3+y*z+x^2*y^3)*(w^4*z^6+y^2*z^3-w^2*x^2*y^2*z^2+x^5*z-x^4*y^2-w^3*x^3*y);

l= factorize (f); testfactors (l, f);

kill r;
ring r= 2,(x,y),dp;
list l;
poly f= x*(x+y)*(x^2+x+y^2)*(x^2+x+y+1)*(x^2+x+y^2+1)*(x^3+x+y^3)*(x^3+x+y+1)*(x^3+x+y^3+1)*(x^3+x+y^3+y+1)*(x^3+x+y^3+y^2+1);

l= factorize (f); testfactors (l, f);

f= x*(x+y)*(x^2+x+y^2)*(x^2+x+y+1)*(x^2+x+y^2+1);

l= factorize (f); testfactors (l, f);

f= (x+y^2)*(x^2+y)*(x+y^4)*(x^4+y);

l= factorize (f); testfactors (l, f);

f= (x+y^2)*(x^2+y);

l= factorize (f); testfactors (l, f);

kill r;

ring r= 3,(x,y),dp;
list l;
poly f= (x+y^2)*(x^2+y)*(x+y^4)*(x^4+y);

l= factorize (f); testfactors (l, f);

f= x^3 - y^3;

l= factorize (f); testfactors (l, f);

kill r;

ring r= 5,(x,y,z),dp;

list l;

poly f= (x*z+y+30)*(y*z+x+20)*(z+x*y+10);

l= factorize (f); testfactors (l, f);

f= (x+y^2)*(x^2+y)*(x+y^4)*(x^4+y);

l= factorize (f); testfactors (l, f);

f= x^10 - y^4;

l= factorize (f); testfactors (l, f);

kill r;
ring r= 101, (a,b,c,d), dp;
list l;
poly f= (a-b)*(a^4-7*b* c-4)*(a^5-b+c-11*d^3+1);

l= factorize (f); testfactors (l, f);

kill r;
ring r= 31991, (x,y,z,w,u,v,s,t), dp;
list l;
poly f= x^6*y^3*z^2*(3*z^3+2*w*z-8*x*y^2+14*w^2*y^2-y^2+18*x^3*y)*(w^2*z^3-12*w^2*x*y*z^3+3*x*y^2+29*x-w^2);

l= factorize (f); testfactors (l, f);

f= (22*y^5-18*x^4*y^5-26*x^3*y^4-38*x^2*y^4+29*x^2*y^3-41*x^4*y^2+37*x^4)*(33*x^5*y^6+11*y^2+35*x^3*y-22*x^4);

l= factorize (f); testfactors (l, f);

f= (3*z^3+2*w*z-9*y^3-y^2+45*x^3)*(w^2*z^3+47*x*y-w^2);

l= factorize (f); testfactors (l, f);

f= (z+y+x-3)*(z+y+x-3)*(z+y+x-3);

l= factorize (f); testfactors (l, f);

f= (z+y+x-3)*(z+y+x-3)*(z+y+x-3)*(z+y+x-2)*(z+y+x-2);

l= factorize (f); testfactors (l, f);

f= (z^2+x^3*y^4+u^2)*((y^2+x)*z^2 + 3*u^2*x^3*y^4*z+19*y^2)*(u^2*y^4*z^2+x^2*z+5);

l= factorize (f); testfactors (l, f);

f= (z^2-x^3*y+3)*(z^2+x^3*y^4)*(y^4*z^2+x^2*z+5);

l= factorize (f); testfactors (l, f);

f= (y*z^3+x*y*z+y^2+x^3)*(x*(z^4+1)+z+x^3*y^2);

l= factorize (f); testfactors (l, f);

f= (x^3*(z+y)+z-11)*(x^2*(z^2+y^2)+y+90);

l= factorize (f); testfactors (l, f);

f= x^10 - y^4;

l= factorize (f); testfactors (l, f);

f= (x*z^2+2*x*z*w+x*w^2+z^2+w+2*z*w)*(x*z^2+2*x*z*w+x*w^2+z);

l= factorize (f); testfactors (l, f);

f= (x^2-x*z+z*y+2)*(x^2+3*x-y^2-15);

l= factorize (f); testfactors (l, f);

f= (z^3+x*y*z+y^2+x^3)*(x*(z^4+1)+z+x^3*y^2);

l= factorize (f); testfactors (l, f);

f= (x^2*y+x*y+1)*(x^2*y+2*x*y+3)*(2*x^2*y+3*x*y+5)*(3*x^2*y+4*x*y+7);

l= factorize (f); testfactors (l, f);

f= w^2*x^7*y^6*12*z^5+31962*z^2*y^5*x^7+87*z^5*y^3*x^7+31967*z^2*y^7*x^8+31759*z^2*y^5*x^8+31988*z^2*y^7*x^7+54*z^2*y^6*x^10+522*z^2*y^4*x^10+9*z^5*y^5*x^7+z^2*y^5*x^6*w^2+3*z^8*y^3*x^6*w^2+2*z^6*y^3*x^6*w^3+31989*z^3*y^3*x^6*w^3+31977*z^2*y^5*x^6*w^4+31988*z^5*y^3*x^6*w^2+414*z^2*y^5*x^7*w^2+42*z^2*y^7*x^7*w^2+14*z^5*y^5*x^6*w^4+31990*z^5*y^5*x^6*w^2+31973*z^2*y^4*x^9*w^2+58*z^3*y^3*x^7*w+31955*z^8*y^4*x^7*w^2+6*z^3*y^5*x^7*w+31967*z^6*y^4*x^7*w^3+96*z^5*y^6*x^8*w^2+31983*z^5*y^5*x^7*w^2+31823*z^5*y^6*x^7*w^4+31775*z^5*y^5*x^10*w^2+18*z^5*y^4*x^9*w^2;

l= factorize (f); testfactors (l, f);

f= (z+x*y+10)*(x*z+y+30)*(y*z+x+20);

l= factorize (f); testfactors (l, f);

f= (x^3*y+x^3*z+z-11)*(x^2*z^2+x^2*y^2+y+90);

l= factorize (f); testfactors (l, f);

f= (x-w)*(y-w)*(x-y)*(z-w)*(x-z)*(y-z);

l= factorize (f); testfactors (l, f);

f= (10*y+x^3+9)*(6*x*y^2+x+y+x^3+15);

l= factorize (f); testfactors (l, f);

f= (3*z^3+2*w*z-10*y^2+45*x^3)*(w^2*z^3+47*x*y-w^2);

l= factorize (f); testfactors (l, f);

f= (x^2-10748*y*x+y^2)*(y^2+x^2)*(x^2+10748*y*x+y^2);

l= factorize (f); testfactors (l, f);

f= (y*z-x*z+x^2+2)*(z^3-y^2+x^2+3*x-15);

l= factorize (f); testfactors (l, f);

f= (x+y+z)*(z-y+x)*(x-z-y);

l= factorize (f); testfactors (l, f);

f= (y*z+x*y^2+9)*(y*z+x*z+x^2*y);

l= factorize (f); testfactors (l, f);

f= (3*z^2+2*y^2+x^2)*(4*z^2-3*y^2+2*x^2);

l= factorize (f); testfactors (l, f);

f= x^4 + y^3 + z^5;

l= factorize (f); testfactors (l, f);

f= -9944*(x^2-5265*x+14343)*(x^2+5943*x-3555)*(x-8995)*(x-15035);

l= factorize (f); testfactors (l, f);

f= x^8 + x +1 ;

l= factorize (f); testfactors (l, f);

f= x^14-1;

l= factorize (f); testfactors (l, f);

tst_status(1);$
