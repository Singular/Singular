LIB "tst.lib";
tst_init();
echo=1;

LIB("primdec.lib");

proc testMinAssUnit(method, I)
{
  def result = method(I);
  ASSUME(0, size(result)==0);
}

proc testPrimdecUnit(method, I)
{
  def result = method(I);
  ASSUME(0, size(result)==0);
}

proc primdecGTZkeepKomp (I)          {    return ( primdecGTZ(I,"keep_comp") );    }
proc minAssGTZFacSTD (I)             {    return ( minAssGTZ(I,"facstd") );    }
proc minAssGTZnoFacSTD (I)           {    return ( minAssGTZ(I,"noFacstd") );  }
proc minAssGTZorigNoFacSTD (I)       {    return ( minAssGTZ(I,"GTZ","noFacstd") );       }
proc minAssGTZorig (I)               {    return ( minAssGTZ(I,"GTZ") );       }

proc primdecSYNoOptimalOrdering (I)  {    return ( primdecSY(I,0) );           }
proc minAssCharNoOptimalOrdering (I) {    return ( minAssChar(I,0) );           }


ring rng = 0,(x,y),dp;
ideal I = x+y,x+1,y;  

testPrimdecUnit( primdecGTZ,I );
testPrimdecUnit( primdecGTZkeepKomp,I );
testMinAssUnit( minAssGTZ, I );
testMinAssUnit( minAssGTZFacSTD, I );
testMinAssUnit( minAssGTZnoFacSTD, I );
testMinAssUnit( minAssGTZorig, I );
testMinAssUnit( minAssGTZorigNoFacSTD, I );
testPrimdecUnit( primdecSY, I );
testPrimdecUnit( primdecSYNoOptimalOrdering, I );
testMinAssUnit( minAssChar,I );
testMinAssUnit( minAssCharNoOptimalOrdering,I );


kill rng;
ring rng = 0,(x,y),ds;
ideal I = x+y,x+1,y;  
testPrimdecUnit( primdecGTZ,I );

testPrimdecUnit( primdecGTZ,I );
testPrimdecUnit( primdecGTZkeepKomp,I );
testMinAssUnit( minAssGTZ, I );
testMinAssUnit( minAssGTZFacSTD, I );
testMinAssUnit( minAssGTZnoFacSTD, I );
testMinAssUnit( minAssGTZorig, I );
testMinAssUnit( minAssGTZorigNoFacSTD, I );
testPrimdecUnit( primdecSY, I );
testPrimdecUnit( primdecSYNoOptimalOrdering, I );
testMinAssUnit( minAssChar,I );
testMinAssUnit( minAssCharNoOptimalOrdering,I );


kill rng;
ring rng = (7,a),(x,y,z),dp; minpoly = a^2+a+3;
ideal I = x+y,x+1,y; 

testPrimdecUnit( primdecGTZ,I );
testPrimdecUnit( primdecGTZkeepKomp,I );
testMinAssUnit( minAssGTZ, I );
testMinAssUnit( minAssGTZFacSTD, I );
testMinAssUnit( minAssGTZnoFacSTD, I );
testMinAssUnit( minAssGTZorig, I );
testMinAssUnit( minAssGTZorigNoFacSTD, I );
testPrimdecUnit( primdecSY, I );
testPrimdecUnit( primdecSYNoOptimalOrdering, I );

//  minAssChar not implemented for...

kill rng;
ring rng = (7,a),(x,y,z),ds; minpoly = a^2 + a+3;
ideal I = x+y,x+1,y; 

testPrimdecUnit( primdecGTZ,I );
testPrimdecUnit( primdecGTZkeepKomp,I );
testMinAssUnit( minAssGTZ, I );
testMinAssUnit( minAssGTZFacSTD, I );
testMinAssUnit( minAssGTZnoFacSTD, I );
testMinAssUnit( minAssGTZorig, I );
testMinAssUnit( minAssGTZorigNoFacSTD, I );
testPrimdecUnit( primdecSY, I );
testPrimdecUnit( primdecSYNoOptimalOrdering, I );

// minAssChar not implemented for...


tst_status(1); $
