proc foo
{
  "show all args for foo";
  for (int @i = 1; @i<=size(#); @i=@i+1)
  {
    #[@i];
  }
  "show computed arg 4";
  #[1000-999+3];
}
foo(3,4,5,6,7,8,9);
LIB "tst.lib";tst_status(1);$;
