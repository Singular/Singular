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
m[3,2] = y5x + 4xyz;
m;
string ss;
for (int ii=1; ii<=nrows(m); ii=ii+1)
{
  ss;
  ss = "ideal id(" + string(ii),")=";
  ss;
  ss = ss +  string(mrows(m,ii));
  ss;
  execute(ss);
  id(ii);
}
listvar(all);
"id(1) : ";id(1);
"elem 2 of id(1) : ";id(1)[2];
id(1)[3] = (2*id(1)[2])^4;
id(1);
string(id(1)[3]);
proc formmat
{
  int @elems = 0;
  int @mlen  = 0;
  int @slen  = 0;
  int @c;
  for (int @r=1; @r<=nrows(#[1]); @r=@r+1)
  {
    for (@c=1; @c<=ncols(#[1]); @c=@c+1)
    {
      @elems = @elems + 1;
      string @s(@elems) = string(#[1][@r,@c]);
      @slen = size(@s(@elems));
      if (@slen > @mlen) { @mlen = @slen; }
    }
  }
  @elems = 0;
  string @aus;
  string @fill = "                                                ";
  string @sep = " ";
  pagewidth = 30;
  if (@mlen * ncols(#[1]) >= pagewidth)
  {
    @sep = NL;
  }
  for (@r=1; @r<=nrows(#[1]); @r=@r+1)
  {
    @aus = "";
    for (@c=1; @c<=ncols(#[1]); @c=@c+1)
    {
      @elems = @elems + 1;
      @slen = size(@s(@elems));
      @aus = @aus + @s(@elems) + @fill[1,@mlen-@slen+1] + @sep;
    }
    @aus;
  }
}
formmat(m);
LIB "tst.lib";tst_status(1);$
