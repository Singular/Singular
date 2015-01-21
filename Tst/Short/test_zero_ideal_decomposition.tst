LIB "tst.lib";
tst_init();
echo=1;

LIB("primdec.lib");


proc testMinAssZero(method, I)
{
  def result = method(I);
  ASSUME(0, size(result)==1);
  ASSUME(0, result[1]==0 );
}


proc testPrimdecZero(method, I)
{
  def result = method(I);
  ASSUME(0, size(result)==1);
  ASSUME(0, size(result[1])==2 );
  ASSUME(0, result[1][1]==0 );
  ASSUME(0, result[1][2]==0 );
}

proc primdecGTZkeepKomp (I)          {    return ( primdecGTZ(I,"keep_comp") );    }
proc minAssGTZFacSTD (I)             {    return ( minAssGTZ(I,"facstd") );    }
proc minAssGTZnoFacSTD (I)           {    return ( minAssGTZ(I,"noFacstd") );  }
proc minAssGTZorig (I)               {    return ( minAssGTZ(I,"GTZ") );       }
proc minAssGTZorigNoFacSTD (I)       {    return ( minAssGTZ(I,"GTZ","noFacstd") );       }

proc primdecSYNoOptimalOrdering (I)  {    return ( primdecSY(I,0) );           }
proc minAssCharNoOptimalOrdering (I)  {    return ( minAssChar(I,0) );           }


ring rng = 0,(x,y),dp;
ideal I = x+y,x+1,y;  ideal Z = 0 ;


testPrimdecZero( primdecGTZ,Z );
testPrimdecZero( primdecGTZkeepKomp,Z );
testMinAssZero( minAssGTZ, Z );
testMinAssZero( minAssGTZFacSTD, Z );
testMinAssZero( minAssGTZnoFacSTD, Z );
testMinAssZero( minAssGTZorig, Z );
testMinAssZero( minAssGTZorigNoFacSTD, Z );
testPrimdecZero( primdecSY, Z );
testPrimdecZero( primdecSYNoOptimalOrdering, Z );
testMinAssZero( minAssChar,Z );
testMinAssZero( minAssCharNoOptimalOrdering,Z );


kill rng;
ring rng = 0,(x,y),ds;
ideal I = x+y,x+1,y;    ideal Z = 0 ;


testPrimdecZero( primdecGTZ,Z );
testPrimdecZero( primdecGTZkeepKomp,Z );
testMinAssZero( minAssGTZ, Z );
testMinAssZero( minAssGTZFacSTD, Z );
testMinAssZero( minAssGTZnoFacSTD, Z );
testMinAssZero( minAssGTZorig, Z );
testMinAssZero( minAssGTZorigNoFacSTD, Z );
testPrimdecZero( primdecSY, Z );
testPrimdecZero( primdecSYNoOptimalOrdering, Z );
testMinAssZero( minAssChar,Z );
testMinAssZero( minAssCharNoOptimalOrdering,Z );


kill rng;
ring rng = (7,a),(x,y,z),dp; minpoly = a^2+a+3;
ideal I = x+y,x+1,y; ideal Z = 0 ;


testPrimdecZero( primdecGTZ,Z );
testPrimdecZero( primdecGTZkeepKomp,Z );
testMinAssZero( minAssGTZ, Z );
testMinAssZero( minAssGTZFacSTD, Z );
testMinAssZero( minAssGTZnoFacSTD, Z );
testMinAssZero( minAssGTZorig, Z );
testMinAssZero( minAssGTZorigNoFacSTD, Z );
testPrimdecZero( primdecSY, Z );
testPrimdecZero( primdecSYNoOptimalOrdering, Z );

//  minAssChar not implemented for...

kill rng;
ring rng = (7,a),(x,y,z),ds; minpoly = a^2 + a+3;
ideal I = x+y,x+1,y; ideal Z = 0 ;


testPrimdecZero( primdecGTZ,Z );
testPrimdecZero( primdecGTZkeepKomp,Z );
testMinAssZero( minAssGTZ, Z );
testMinAssZero( minAssGTZFacSTD, Z );
testMinAssZero( minAssGTZnoFacSTD, Z );
testMinAssZero( minAssGTZorig, Z );
testMinAssZero( minAssGTZorigNoFacSTD, Z );
testPrimdecZero( primdecSY, Z );
testPrimdecZero( primdecSYNoOptimalOrdering, Z );

// minAssChar not implemented for...


tst_status(1); $
