proc mrows
{
  string @rc;
  for (int @i = 1; @i<=ncols(#[1]); @i=@i+1)
  {
    @rc = @rc + "m[" + string(#[2]) + "," + string(@i) + "],";
  }
  @rc = @rc[1,size(@rc)-1] + ";";
  return (@rc);
}
ring r= 32003,(x,y,z),lp;
matrix m[3][3] = 0;
m[1,1] = 4x;
m[1,2] = y5x;
m;
for (int ii=1; ii<=nrows(m); ii=ii+1)
{
  execute(mrows(m,ii));
}
LIB "tst.lib";tst_status(1);$;
