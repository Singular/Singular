LIB "tst.lib";
tst_init();

LIB("ehv.lib");

proc testRemoveReduntandComponents()
{
  ring rng = 0,(x,y),dp;
  def L1 = list(list(std(ideal(x-1)),ideal(x-1)),list(std(ideal(x-1)),ideal(x-1)),list(std(ideal(y-2)),ideal(y-2)));
  def L2 = list(list(std(ideal(x-1)),ideal(x-1)),list(std(ideal(y-2)),ideal(y-2)));
  def LL1 = removeRedundantComponents(L1);
  ASSUME(0, size(LL1)==2);
  ASSUME(0, primDecsAreEquivalent( L1, L2 ) );

  system("random", 1401092447);
  ring rng2 = 23,(x, y, z), dp;
  ideal I = 9*x*y^2-11*x*z^2, 11*x*y*z^2+4*x^2*z, -3*y^5+x*z-10*y*z;

  list L1,L2, LL1;

  def RI = std(radical(I));

  L1 = primdecGTZ(RI);
  L2 = primdecGTZ(RI);
  LL1 = removeRedundantComponents(L1);
  ASSUME(0, size(LL1)==6);
  ASSUME(0, primDecsAreEquivalent( LL1, L1) );
  ASSUME(0, primDecsAreEquivalent( L1 , L2 ) );
}

proc testPrimDecsAreEquivalent()
{
    ring rng = 0 ,x,dp;
    list L1, L2;

    // test1. different sizes
    L1[1] = list( ideal(0), ideal(0)  );
    ASSUME(0, 0 == primDecsAreEquivalent(L1,L2) );
    // test2. same size, but different content
    L2[1] = list( ideal(1), ideal(1)  );

    ASSUME(0, 0 == primDecsAreEquivalent(L1,L2) );

    // test3. equivalent content
    L1[1] = list( ideal(0), ideal(0)  );
    L1[2] = list( ideal(x), ideal(x)  );
    L2[1] = list( ideal(x), ideal(x)  );
    L2[2] = list( ideal(0), ideal(0)  );
    ASSUME(0, 1 == primDecsAreEquivalent(L1,L2) );

  

}

testRemoveReduntandComponents();
testPrimDecsAreEquivalent();

tst_status(1); $;

