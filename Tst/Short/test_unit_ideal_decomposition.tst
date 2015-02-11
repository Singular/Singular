LIB "tst.lib";
tst_init();
echo = 1;

int assumeLevel = 3;

LIB("primdec.lib");

proc testMinAssUnitOrigBehaviour(method, I)
{
  def result = method(I);
  ASSUME(0, size(result)==1);
}


proc testPrimdecUnit(method, I, expectedSize)
{
  def result = method(I);
  ASSUME(0, size(result) == expectedSize);
}


proc testPrimdecUnitOrigBehaviour(method, I)
{
  def result = method(I);
  ASSUME(0, size(result)==1);
}

proc testMinAssUnitNewBehaviour(method, I)
{
  def result = method(I);
  ASSUME(0, size(result)==0);
}

proc testPrimdecUnitNewBehaviour(method, I)
{
  def result = method(I);
  ASSUME(0, size(result)==0);
}


proc wDecompE (I)                    {    return ( Primdec::decompE(I) );                        }
proc wDecomp (I)                     {    return ( Primdec::convList(Primdec::decomp(I) ) );     }


proc primdecGTZEkeepKomp (I)         {    return ( primdecGTZE(I,"keep_comp") );     }
proc primdecGTZkeepKomp (I)          {    return ( primdecGTZ (I,"keep_comp") );     }

proc minAssGTZEFacSTD (I)            {    return ( minAssGTZE(I,"facstd") );         }
proc minAssGTZFacSTD  (I)            {    return ( minAssGTZ (I,"facstd") );         }

proc minAssGTZEnoFacSTD (I)          {    return ( minAssGTZE(I,"noFacstd") );       }
proc minAssGTZnoFacSTD  (I)          {    return ( minAssGTZ (I,"noFacstd") );       }

proc minAssGTZEorigNoFacSTD (I)      {    return ( minAssGTZE(I,"GTZ","noFacstd") ); }
proc minAssGTZorigNoFacSTD (I)       {    return ( minAssGTZ (I,"GTZ","noFacstd") ); }

proc minAssGTZEGTZ (I)               {    return ( minAssGTZE(I,"GTZ") );            }
proc minAssGTZGTZ  (I)               {    return ( minAssGTZ (I,"GTZ") );            }

proc primdecSYENoOptimalOrdering (I) {    return ( primdecSYE(I,0) );                }
proc primdecSYNoOptimalOrdering  (I) {    return ( primdecSY (I,0) );                }

proc minAssCharENoOptimalOrdering(I) {    return ( minAssCharE(I,0) );               }
proc minAssCharNoOptimalOrdering (I) {    return ( minAssChar (I,0) );               }

proc primdecEUsingMinAssChar  (I)    {    return ( Primdec::prim_dec_i( int(1),I,0) );        }
proc primdecUsingMinAssChar   (I)    {    return ( Primdec::prim_dec_i( int(0),I,0) );        }


proc minAssPrimesNew  (I)           {    return ( Primdec::minAssPrimesE(I) );        }
proc minAssPrimesOrig (I)           {    return ( Primdec::minAssPrimes(I) );         }


proc minAssPrimesOldNew  (I)        {    return ( Primdec::minAssPrimesoldE(I) );    }
proc minAssPrimesOldOrig (I)        {    return ( Primdec::minAssPrimesold (I) );    }

proc minAssNew  (I)                 {    return ( Primdec::minAssE(I) );             }
proc minAssOrig (I)                 {    return ( Primdec::minAss (I) );             }


proc minAssPrimCharsetsECurrentOrd(I)    {    return ( Primdec::min_ass_prim_charsets_i( int(1),I,0)) ; }
proc minAssPrimCharsetsCurrentOrd (I)    {    return ( Primdec::min_ass_prim_charsets_i( int(0),I,0)) ; }

proc minAssPrimCharsetsEOptimalOrd (I)    {    return ( Primdec::min_ass_prim_charsets_i( int(1),I,1)) ; }
proc minAssPrimCharsetsOptimalOrd  (I)    {    return ( Primdec::min_ass_prim_charsets_i( int(0),I,1)) ; }


proc minAssPrimCharsets0New  (I)    {    return ( Primdec::min_ass_prim_charsets0_i( int(1),I,0)) ; }
proc minAssPrimCharsets0Orig (I)    {    return ( Primdec::min_ass_prim_charsets0_i( int(0),I,0)) ; }

proc minAssPrimCharsets1New   (I)   {    return ( Primdec::min_ass_prim_charsets1_i( int(1), I,1)) ; }
proc minAssPrimCharsets1Orig  (I)   {    return ( Primdec::min_ass_prim_charsets1_i( int(0), I,1)) ; }


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
ring rng1 = 0,(x,y),dp;
ideal I = x+y,x+1,y;

list pr = primdecGTZE(I);
ASSUME(0, testPrimaryE(pr,I) );

testPrimdecUnitNewBehaviour( wDecompE,        I );

testPrimdecUnitNewBehaviour( primdecGTZE,        I );
testPrimdecUnitNewBehaviour( primdecGTZEkeepKomp,I );
testMinAssUnitNewBehaviour ( minAssGTZE,         I );
testMinAssUnitNewBehaviour ( minAssGTZEFacSTD,   I );
testMinAssUnitNewBehaviour ( minAssGTZEnoFacSTD, I );
testMinAssUnitNewBehaviour ( minAssGTZEGTZ,      I );
testMinAssUnitNewBehaviour ( minAssGTZEorigNoFacSTD, I );

testPrimdecUnitNewBehaviour( primdecSYE,                   I );
testPrimdecUnitNewBehaviour( primdecSYENoOptimalOrdering,  I );
testMinAssUnitNewBehaviour ( minAssCharE,                  I );
testMinAssUnitNewBehaviour ( minAssCharENoOptimalOrdering, I );
testMinAssUnitNewBehaviour ( minAssNew,                    I );
testMinAssUnitNewBehaviour ( minAssPrimesNew,              I );
testMinAssUnitNewBehaviour ( minAssPrimesOldNew,           I );
testMinAssUnitNewBehaviour ( primdecEUsingMinAssChar,      I );
testMinAssUnitNewBehaviour ( minAssPrimCharsetsECurrentOrd,I );
testMinAssUnitNewBehaviour ( minAssPrimCharsetsEOptimalOrd,I );
testMinAssUnitNewBehaviour ( minAssPrimCharsets0New,       I );
testMinAssUnitNewBehaviour ( minAssPrimCharsets1New,       I );
testMinAssUnitNewBehaviour ( minAssPrimesNew,              I );

//////////////////////////////////////////////////////////////////////////////////////////////
testPrimdecUnitOrigBehaviour( wDecomp,        I );
testPrimdecUnitOrigBehaviour( primdecGTZ,I );
testPrimdecUnitOrigBehaviour( primdecGTZkeepKomp,I );
testMinAssUnitOrigBehaviour ( minAssGTZ, I );
testMinAssUnitOrigBehaviour ( minAssGTZFacSTD, I );
testMinAssUnitOrigBehaviour ( minAssGTZnoFacSTD, I );
testMinAssUnitOrigBehaviour ( minAssGTZGTZ, I );
testMinAssUnitOrigBehaviour ( minAssGTZorigNoFacSTD, I );
testPrimdecUnitOrigBehaviour( primdecSY, I );
testPrimdecUnitOrigBehaviour( primdecSYNoOptimalOrdering, I );
testMinAssUnitOrigBehaviour ( minAssChar,I );
testMinAssUnitOrigBehaviour ( minAssCharNoOptimalOrdering,I );
testMinAssUnitOrigBehaviour ( minAssOrig,I );
testMinAssUnitOrigBehaviour ( minAssPrimesOrig,I );

// do not worry about this one
// testMinAssUnitOrigBehaviour( minAssPrimesOldOrig,I );

testMinAssUnitOrigBehaviour( primdecUsingMinAssChar,I );
testMinAssUnitOrigBehaviour( minAssPrimCharsetsCurrentOrd,I );
testMinAssUnitOrigBehaviour( minAssPrimCharsetsOptimalOrd,I );
testMinAssUnitOrigBehaviour( minAssPrimCharsets0Orig,I );
testMinAssUnitOrigBehaviour( minAssPrimCharsets1Orig,I );
testMinAssUnitOrigBehaviour( minAssPrimesOrig,I );




// will not hanld unit ideal case // testMinAssUnitNewBehaviour( absPrimdecGTZE,I );
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
ring rng2 = 0,(x,y),dp;
ideal I = x+y,x+1,y;

if (defined(pr)) {kill pr;}
list pr = primdecGTZE(I);

testPrimdecUnitOrigBehaviour( primdecGTZ,I );
testPrimdecUnitOrigBehaviour( primdecGTZkeepKomp,I );
testMinAssUnitOrigBehaviour( minAssGTZ, I );
testMinAssUnitOrigBehaviour( minAssGTZFacSTD, I );
testMinAssUnitOrigBehaviour( minAssGTZnoFacSTD, I );
testMinAssUnitOrigBehaviour( minAssGTZGTZ, I );
testMinAssUnitOrigBehaviour( minAssGTZorigNoFacSTD, I );
testPrimdecUnitOrigBehaviour( primdecSY, I );
testPrimdecUnitOrigBehaviour( primdecSYNoOptimalOrdering, I );
testMinAssUnitOrigBehaviour( minAssChar,I );
testMinAssUnitOrigBehaviour( minAssCharNoOptimalOrdering,I );
testMinAssUnitOrigBehaviour( minAssPrimesOrig,I );


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


ring rng3 = 0,(x,y),ds;
ideal I = x+y,x+1,y;

// illegal ASSUME(0, testPrimaryE(pr,I) );
testPrimdecUnit( primdecGTZE,I ,0 );
testPrimdecUnit( primdecGTZEkeepKomp,I ,0);
testPrimdecUnit( minAssGTZE, I,0 );
testPrimdecUnit( minAssGTZEFacSTD, I,0 );
testPrimdecUnit( minAssGTZEnoFacSTD, I ,0);
testPrimdecUnit( minAssGTZEGTZ, I ,0);

testPrimdecUnit( minAssGTZEorigNoFacSTD, I ,0);
testPrimdecUnit( primdecSYE, I ,0);
testPrimdecUnit( primdecSYENoOptimalOrdering, I ,0);
testPrimdecUnit( minAssCharE,I,0 );
testPrimdecUnit( minAssCharENoOptimalOrdering,I ,0);
//illgal testPrimdecUnit( minAssPrimesNew,I ,0);
// will not handle unit ideal case // testPrimdecUnit( absPrimdecGTZE,I ,0);


testPrimdecUnit( primdecGTZ,I ,0);

testPrimdecUnit( primdecGTZ,I ,0);
//testPrimdecUnit( primdecGTZkeepKomp,I ,0);
testPrimdecUnit( minAssGTZ, I ,0);
// testPrimdecUnit( minAssGTZFacSTD, I ,0);
// testPrimdecUnit( minAssGTZnoFacSTD, I ,0);
// testPrimdecUnit( minAssGTZGTZ, I ,0);
// testPrimdecUnit( minAssGTZorigNoFacSTD, I ,0);
testPrimdecUnit( primdecSY, I ,0);
testPrimdecUnit( primdecSYNoOptimalOrdering, I ,0);
testPrimdecUnit( minAssChar,I ,0);
testPrimdecUnit( minAssCharNoOptimalOrdering,I ,0);
// invalid: testPrimdecUnit( minAssPrimesOrig,I ,0);
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

ring rng4 = (7,a),(x,y,z),dp;
minpoly = 1*a^2+6*a^1+3*a^0;
ideal I = x+y,x+1,y;

testPrimdecUnitNewBehaviour( wDecompE,        I );

testPrimdecUnitNewBehaviour( primdecGTZE,        I );
testPrimdecUnitNewBehaviour( primdecGTZEkeepKomp,I );
testMinAssUnitNewBehaviour ( minAssGTZE,         I );
testMinAssUnitNewBehaviour ( minAssGTZEFacSTD,   I );
testMinAssUnitNewBehaviour ( minAssGTZEnoFacSTD, I );
testMinAssUnitNewBehaviour ( minAssGTZEGTZ,     I );
testMinAssUnitNewBehaviour ( minAssGTZEorigNoFacSTD, I );

testPrimdecUnitNewBehaviour( primdecSYE,                  I );
testPrimdecUnitNewBehaviour( primdecSYENoOptimalOrdering, I );
//testMinAssUnitNewBehaviour ( minAssCharE,                 I );
//testMinAssUnitNewBehaviour ( minAssCharENoOptimalOrdering,I );
testMinAssUnitNewBehaviour ( minAssNew,                   I );
testMinAssUnitNewBehaviour ( minAssPrimesNew,             I );
testMinAssUnitNewBehaviour ( minAssPrimesOldNew,          I );
testMinAssUnitNewBehaviour ( primdecEUsingMinAssChar,     I );
//testMinAssUnitNewBehaviour ( minAssPrimCharsetsECurrentOrd,I );
//testMinAssUnitNewBehaviour ( minAssPrimCharsetsEOptimalOrd,I );
//testMinAssUnitNewBehaviour ( minAssPrimCharsets0New,      I );
//testMinAssUnitNewBehaviour ( minAssPrimCharsets1New,      I );
testMinAssUnitNewBehaviour ( minAssPrimesNew,             I );

//////////////////////////////////////////////////////////////////////////////////////////////
testPrimdecUnitOrigBehaviour( wDecomp,        I );
testPrimdecUnitOrigBehaviour( primdecGTZ,I );
testPrimdecUnitOrigBehaviour( primdecGTZkeepKomp,I );
testMinAssUnitOrigBehaviour ( minAssGTZ, I );
testMinAssUnitOrigBehaviour ( minAssGTZFacSTD, I );
testMinAssUnitOrigBehaviour ( minAssGTZnoFacSTD, I );
testMinAssUnitOrigBehaviour ( minAssGTZGTZ, I );
testMinAssUnitOrigBehaviour ( minAssGTZorigNoFacSTD, I );
testPrimdecUnitOrigBehaviour( primdecSY, I );
testPrimdecUnitOrigBehaviour( primdecSYNoOptimalOrdering, I );

//testMinAssUnitOrigBehaviour ( minAssChar,I );
//testMinAssUnitOrigBehaviour ( minAssCharNoOptimalOrdering,I );
testMinAssUnitOrigBehaviour ( minAssOrig,I );
testMinAssUnitOrigBehaviour ( minAssPrimesOrig,I );

// do not worry about this one
// testMinAssUnitOrigBehaviour( minAssPrimesOldOrig,I );

testMinAssUnitOrigBehaviour( primdecUsingMinAssChar,I );
//testMinAssUnitOrigBehaviour( minAssPrimCharsetsCurrentOrd,I );
//testMinAssUnitOrigBehaviour( minAssPrimCharsetsOptimalOrd,I );
//testMinAssUnitOrigBehaviour( minAssPrimCharsets0Orig,I );
//testMinAssUnitOrigBehaviour( minAssPrimCharsets1Orig,I );
testMinAssUnitOrigBehaviour( minAssPrimesOrig,I );

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

ring rng5 = (7,a),(x,y,z),ds; minpoly = a^2 + a+3;
ideal I = x+y,x+1,y;

testPrimdecUnitNewBehaviour( primdecGTZE,I );
testPrimdecUnitNewBehaviour( primdecGTZEkeepKomp,I );
testMinAssUnitNewBehaviour( minAssGTZE, I );
testMinAssUnitNewBehaviour( minAssGTZEFacSTD, I );
testMinAssUnitNewBehaviour( minAssGTZEnoFacSTD, I );
testMinAssUnitNewBehaviour( minAssGTZEGTZ, I );
testMinAssUnitNewBehaviour( minAssGTZEorigNoFacSTD, I );
testPrimdecUnitNewBehaviour( primdecSYE, I );
testPrimdecUnitNewBehaviour( primdecSYENoOptimalOrdering, I );
// illegal testMinAssUnitNewBehaviour( minAssPrimesNew,I );



tst_status(1); $
