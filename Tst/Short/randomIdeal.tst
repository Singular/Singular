LIB "tst.lib";
tst_init();


LIB("randomIdeal.lib");


proc testSuiteRandomIdeal()
{
    RandomIdeal::testBaseringHasMinpoly();
    RandomIdeal::testBaseringHasComplexCoeffRing();
    RandomIdeal::testMinpolyProbablyConsistant();
    RandomIdeal::testCoeffRingIsTrivial();
    RandomIdeal::testGetParIdx();
    RandomIdeal::testIsParameterVariable();
    RandomIdeal::testRandomVariable();
    RandomIdeal::testGetPrimitiveElementOrd();
    RandomIdeal::testGetRandomName();
    RandomIdeal::testCreateRandomExCoeffGenerator();
    RandomIdeal::testCreateRandomComplexCoeffGenerator();
    RandomIdeal::testCreateRandomZCoeffGenerator();
    RandomIdeal::testCreateRandomQCoeffGenerator();
    RandomIdeal::testHasCertainlyCoeffOrdering();
    RandomIdeal::testPureRandomCoeffGenerator();
    RandomIdeal::testFullRandomCoeffGenerator();
    RandomIdeal::testHasWeightedVariables();
    RandomIdeal::testRandomMonomialGenerator();
    RandomIdeal::testRandomTermGenerator();
    RandomIdeal::testMonomialExistsInPoly();
    RandomIdeal::testRandomPolyGenerator();
    RandomIdeal::testRandomIdealGenerator();
}

testSuiteRandomIdeal();








tst_status(1);$



