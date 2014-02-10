LIB "tst.lib";
tst_init();

LIB("ehv.lib");

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

testPrimDecsAreEquivalent();

tst_status(1); $;

