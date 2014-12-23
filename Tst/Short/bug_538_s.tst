LIB "tst.lib";
tst_init();

    LIB("primdecint.lib");

    proc testQuotientZSpecialCase()
    {
       ring r = integer,(x),dp;
       def quotientResult =  Primdecint::quotientZ( ideal(x) , ideal(0) );
       if ( not (size( reduce( quotientResult, std(ideal(1)) )) == 0) )
       {
          ERROR("wrong result for Primdecint::quotientZ(x, ideal(0))");
       }
    }

    proc testQuotientOneZSpecialCase()
    {
       ring r = integer,(x),dp;
       def quotientOneZResult =  Primdecint::quotientOneZ( ideal(x) , poly(0) );
       if ( not (size( reduce( quotientOneZResult, std(ideal(1)) )) == 0) )
       {
          ERROR("wrong result for Primdecint::quotientOneZResult( x, poly(0) )");
       }
    }

    testQuotientZSpecialCase();
    testQuotientOneZSpecialCase();


tst_status(1); $


