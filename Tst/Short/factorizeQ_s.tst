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

//tr. 439
ring rQ=0,(u,v),dp;
poly f = u*(3v+1)^2;
list l= factorize(f);
testfactors (l,f);

//tr. 448
ring r = (0, ah, bh, sh0), (ch2, ch1, sh1, ch0), lp;
poly p= ((bh^4)*ch0^4+(11232*ah^2*bh^2*sh0^2-11232*ah^2*bh^2-24960*ah*bh^3*sh0^2+24960*ah*bh^3+12168*bh^4*sh0^2-13736*bh^4)*ch0^2+(51609856*ah^4*sh0^4-103219712*ah^4*sh0^2+51609856*ah^4-179312640*ah^3*bh*sh0^4+358625280*ah^3*bh*sh0^2-179312640*ah^3*bh+243165312*ah^2*bh^2*sh0^4-475066112*ah^2*bh^2*sh0^2+231900800*ah^2*bh^2-151856640*ah*bh^3*sh0^4+284144640*ah*bh^3*sh0^2-132288000*ah*bh^3+37015056*bh^4*sh0^4-64490400*bh^4*sh0^2+28090000*bh^4))*((bh^4)*ch0^4+(11232*ah^2*bh^2*sh0^2-11232*ah^2*bh^2+24960*ah*bh^3*sh0^2-24960*ah*bh^3+12168*bh^4*sh0^2-13736*bh^4)*ch0^2+(51609856*ah^4*sh0^4-103219712*ah^4*sh0^2+51609856*ah^4+179312640*ah^3*bh*sh0^4-358625280*ah^3*bh*sh0^2+179312640*ah^3*bh+243165312*ah^2*bh^2*sh0^4-475066112*ah^2*bh^2*sh0^2+231900800*ah^2*bh^2+151856640*ah*bh^3*sh0^4-284144640*ah*bh^3*sh0^2+132288000*ah*bh^3+37015056*bh^4*sh0^4-64490400*bh^4*sh0^2+28090000*bh^4))*((bh^4)*ch0^4+(11232*ah^2*bh^2*sh0^2-11232*ah^2*bh^2-24320*ah*bh^3*sh0^2+24320*ah*bh^3+11552*bh^4*sh0^2-13120*bh^4)*ch0^2+(51609856*ah^4*sh0^4-103219712*ah^4*sh0^2+51609856*ah^4-174714880*ah^3*bh*sh0^4+349429760*ah^3*bh*sh0^2-174714880*ah^3*bh+230855168*ah^2*bh^2*sh0^4-450445824*ah^2*bh^2*sh0^2+219590656*ah^2*bh^2-140472320*ah*bh^3*sh0^4+261877760*ah*bh^3*sh0^2-121405440*ah*bh^3+33362176*bh^4*sh0^4-57667584*bh^4*sh0^2+24920064*bh^4));
list l= factorize (p);
testfactors (l,p);

kill r;

ring r=0,(a,b,c,d,e,f),dp;
poly g=(-4*c*d + 8*d^2 + 12*f^2 + 8*a)*(8*b*c - 12*c + 4*f)*(3*a*c + 3*b*c - 
 3*a*d + 33*d^2 - 3*f)*(-4*a*b - 4*b*d + 140*d^2 - 4*d*f - 4*a)*(4*a*b - 
 8*b*d - 12*d^2 - 12*c*f - 8)*(-21*b*c - 3*b*d - 222*d^2 - 6*d*f - 
 39*f)*(16*a*d + 128*b*d - 2)*(8*b*c + 4*c^2 - 4*b*f + 12*f)*(4*a*b + 4*b*c 
 - 2*a*f + 6*f^2 - 2*b)*(-48*b^2 + 8*c*d + 144*c)*(-3*b^2 + 4*c^2 - b*d + 
 f^2 + 3*a)*(4*c^2 + 4*b*d - 4*d^2 + 12*a*f);

system ("--random",12345643);
list l=factorize (g);
testfactors (l, g);

tst_status(1);$
