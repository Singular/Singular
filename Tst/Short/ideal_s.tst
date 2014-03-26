LIB "tst.lib";
tst_init();


LIB("ideal.lib");


proc testIsSubset()
{
    ring  r = 0,(x,y),dp;
    ideal j = x;
    ideal i = x, 0 , y;
    ASSUME( 0, isSubset( i, std(j) ) == isSubset( std(i), std(j) ) );
  
    ring rng = 0,(x,y),dp;
    ideal J = x+x,y;
    ideal I = x^2;
    ASSUME(0, 1 == isSubset(I, J) );
    ASSUME(0, 1 == isSubset_assumeSB( I, std(J)) );

    ring s = integer,(x,y),dp;
    ideal mq = x^2 + y^2;
    qring q = std(mq);
    ideal qj = y-x^2-y^ 2, x^2+y^2 ;
    ideal qi = y;
    ASSUME(0, 1==isSubset( qi,qj) );
    ASSUME(0, 1==isSubset_assumeSB( qi, std(qj)) );
}

testIsSubset();


tst_status(1); $;

