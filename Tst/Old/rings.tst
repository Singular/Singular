//declare different rings and show them
ring r0;
r0;
ring r1=0,(x,y,z),ds;
r1;
ring r2=0,(x,y,z),wp(1,2,3);
r2;
ring r3=32003,(x,y,z),(a(1,2,3),ws(4,5,6));
r3;
ring r4=32003,(x,y,z),(a(1,2),ws(4,5,6));
r4;
ring r=32003,(x,y,z),(c,dp);
poly f=x+z;
f;
f+1;
f=f+1;
f;
//
// declare a couple of rings with different characteristics
//
proc ringbundle
{
  // parameters are different characteristics
  if (size(#) == 0)
  {
    "usage:ringbundle(char1, char2, ...)";
    return ();
  }
  string @ex;
  for (int @i=1; @i<=size(#); @i=@i+1)
  {
    @ex = "ring rb(" , string(@i) + ")  = " +  string(#[@i]) + ",(x, y, z),lp;";
    execute @ex;
    @ex = "export rb(" , string(@i) + ");";
    execute @ex;
  }
}
ringbundle();                 // ask for help
ringbundle(3,5,7,11);         // create rb(1) with characteristic 3, etc
listvar(all);
rb(4);
LIB "tst.lib";tst_status(1);$;
