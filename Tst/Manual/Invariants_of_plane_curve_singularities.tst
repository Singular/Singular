LIB "tst.lib"; tst_init();
  LIB "hnoether.lib";
  // ======== The irreducible case ========
  ring s = 0,(x,y),ds;
  poly f = y4-2x3y2-4x5y+x6-x7;
  list hn = develop(f);
  show(hn[1]);     // Hamburger-Noether matrix
  displayHNE(hn);  // Hamburger-Noether development
  setring s;
  displayInvariants(hn);
  // invariants(hn);  returns the invariants as list
  // partial parametrization of f: param takes the first variable
  // as infinite except the ring has more than 2 variables. Then
  // the 3rd variable is chosen.
  param(hn);
  ring extring=0,(x,y,t),ds;
  poly f=x3+2xy2+y2;
  list hn=develop(f,-1);
  param(hn);       // partial parametrization of f
  list hn1=develop(f,6);
  param(hn1);     // a better parametrization
  // instead of recomputing you may extend the development:
  list hn2=extdevelop(hn,12);
  param(hn2);     // a still better parametrization
  //
  // ======== The reducible case ========
  ring r = 0,(x,y),dp;
  poly f=x11-2y2x8-y3x7-y2x6+y4x5+2y4x3+y5x2-y6;
  // = (x5-1y2) * (x6-2x3y2-1x2y3+y4)
  list L=hnexpansion(f);
  show(L[1][1]);     // Hamburger-Noether matrix of 1st branch
  displayInvariants(L);
  param(L[2]);       // parametrization of 2nd branch
tst_status(1);$
