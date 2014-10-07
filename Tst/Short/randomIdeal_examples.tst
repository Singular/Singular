LIB "tst.lib";
tst_init();


LIB("randomIdeal.lib");

example defaultRandomGenerators;            
example createRandomGeneratorsByParams;    
example createRandomGenerators;          
example defaultRandomConstructionParams;   
example defaultRandomFactories;             

example hasCertainlyCoeffOrdering;          
example baseringHasComplexCoeffRing;      
example coeffRingIsTrivial;             
example hasOrdinaryParameters;      
     
example getParIdx;                       
example isParameterVariable;              
example isVariable;                       
example hasWeightedVariables;                

example baseringHasMinpoly;               
example getPrimitiveRootOrd;               
example minpolyProbablyConsistant;         

example getRandomName;                    
example getRandomNameWithPrefix;         

example hasProbablyMemleak;             
example hasProbablyMemleakNoDef;           



tst_status(1);$



