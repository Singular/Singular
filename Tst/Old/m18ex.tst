  LIB "hnoether.lib";
  // ======== The irreducible case ========
  ring s = 0,(x,y),ds;
  poly f = y4-2x3y2-4x5y+x6-x7;
  list hn = develop(f);
  show(hn[1]);     // Hamburger-Noether matrix
  displayHNE(hn);  // Hamburger-Noether developement 
  setring s;
  displayInvariants(hn);
  // invariants(hn); will return the invariants as list
  param(hn);       // partial parametrization of f
                   // param takes the first variable as 
                   // except the ring has >2 variables
  ring extring=0,(x,y,t),ds;
  poly f=x3+2xy2+y2;
  list hn=develop(f,-1);
  param(hn);       // partial parametrization of f
  list hn1=develop(f,6);  
  param(hn1);     // a better parametrization
  // instead or recomputing you may extend the developement:
  list hn2=extdevelop(hn,12);
  param(hn2);     // a still better parametrization
  // ======== The reducible case ========
  ring r = 0,(x,y),dp;
  poly f=x11-2y2x8-y3x7-y2x6+y4x5+2y4x3+y5x2-y6;
  // = (x5-1y2) * (x6-2x3y2-1x2y3+y4)
  list hn=hnexpansion(f);
  show(hn[1][1]);     // Hamburger-Noether matrix of 1st brach
  displayInvariants(hn);
  param(hn[2]);      // parametrization of 2nd branch
  // extended parametrization of 1st
LIB "tst.lib";tst_status(1);$
