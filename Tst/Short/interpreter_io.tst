LIB "tst.lib";
tst_init();

// Buffered procedure and loop input must retain its source lines for echo,
// tracing and diagnostics, including when execute has no final newline.
proc ioLeaf(int n)
{

  if (n<0)
  {
    return(-n);
  }
  return(n+1);
}
proc ioOuter(int n)
{
  int total;
  for (int i=0;i<n;i++)
  {
    total=total+ioLeaf(i);
  }
  execute("total=total+10;");
  return(total);
}
ASSUME(0,ioOuter(3)==16);
ASSUME(0,ioLeaf(-4)==4);

echo=3;
ioOuter(2);
echo=0;
TRACE=3;
ioOuter(2);
TRACE=0;
TRACE=16;
ioLeaf(-1);
TRACE=0;

// A single buffered line can contain several statements. Empty lines and
// the automatically appended return must keep the same echo behavior.
echo=3;
execute("int ioValue=2; ioValue=ioValue+3; ioValue;");
execute("

;

");
echo=0;
ASSUME(0,ioValue==5);

// Check the diagnostic suffix for a line longer than my_yylinebuf.
proc ioBad()
{
  int ioPadding=1;                                                                                 missingIoIdentifier;
}
ioBad();
ioLeaf();
ioLeaf("not an integer");
execute("                                                                                 missingExecuteIdentifier;");

// Error recovery must leave subsequent procedure input intact.
ASSUME(0,ioOuter(3)==16);
print("buffered interpreter input tests passed");
tst_status(1);$
