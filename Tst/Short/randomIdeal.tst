LIB "tst.lib";
tst_init();

LIB("randomIdeal.lib");


proc testRandomIdeal()
{
    RandomIdeal::testRandomVariable();
    RandomIdeal::testRandomCoeffGenerator();
    RandomIdeal::testRandomMonomialGenerator();
    RandomIdeal::testRandomTermGenerator();
    RandomIdeal::testRandomPolyGenerator();
    RandomIdeal::testRandomIdealGenerator();
}


testRandomIdeal();


tst_status(1);$
