LIB "tst.lib";
tst_init();
tst_ignore("not provided yet");

/////////////////// dim() test for rings over integers ////////////////////////////////////
ring rng = integer,(x,y),dp;
string rngStr = string(rng);

ideal i = 1;
int expectedDimension;
expectedDimension = -1;
if (not ( dim(i)==expectedDimension ) ) 
  {  ERROR ("dim() test: expected  dim( ideal(1)) = "+string(expectedDimension)+",but got "+string(dim(i))+" for ring " + rngStr );   }

 i = -1;
i=std(i);
expectedDimension = -1;
if ( not ( dim(i) == expectedDimension ) )   
{  ERROR ("dim() test: expected dim( ideal(-1) )="+string(expectedDimension)+" ,but got "+string(dim(i))+"for ring " + rngStr );      }

i = -1;
if (not (std(i)==1) )   
{  ERROR ("dim() test: error : -1 not recognized as unit " );      }


 i = 5;
i=std(i);
expectedDimension = 2;
if (not (dim(i) == expectedDimension) )  
 {  ERROR ("dim() test: expected  dim( ideal(5) ) = "+string(expectedDimension)+" ,but got "+string(dim(i))+" for ring " + rngStr );   }


 i = 5*7;
i=std(i);
expectedDimension = 2;
if (not (dim(i)==expectedDimension) )   
{  ERROR ("dim() test: expected  dim( ideal(5*7) ) = "+string(expectedDimension)+",but got "+string(dim(i))+"  for ring " + rngStr );   }

 i = x,1;
i=std(i);
expectedDimension = -1;
if (not (dim(i)==expectedDimension) ) 
 {  ERROR ("dim() test: expected  dim( ideal(x,1) ) = "+string(expectedDimension)+" ,but got "+string(dim(i))+" for ring " + rngStr );   }


 i = x,3;
i=std(i);
expectedDimension = 1;
if (not (dim(i)==expectedDimension) )  
{  ERROR ("dim() test: expected  dim( ideal(x,3) ) = "+string(expectedDimension)+",but got "+string(dim(i))+"  for ring " + rngStr );   }

 i = x,y,3;
i=std(i);
expectedDimension = 0;
if (not (dim(i)==expectedDimension) ) 
{  ERROR ("dim() test: expected  dim( ideal(x,y,3) ) = "+string(expectedDimension)+" ,but got "+string(dim(i))+" for ring " + rngStr );   }


i = 2, x-y;
i = std(i);
expectedDimension = 1;
if (not (dim(i)==expectedDimension ) ) 
{  ERROR ("dim test: expected  dim( ideal(2,x-y) ) = "+string(expectedDimension)+",but got "+string(dim(i))+"  for ring " + rngStr );   }

tst_status(1);$

