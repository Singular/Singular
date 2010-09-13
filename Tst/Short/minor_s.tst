LIB "tst.lib";
tst_init();
//
// test script for minor command
//
ring r1 = 32003,(x,y,z),(c,dp);
r1;

proc pmat (matrix m,list #)
{
  if ( size(#) == 0)
  {
//------------- main case: input is a matrix, no second argument---------------
    int @elems;
    int @mlen;
    int @slen;
    int @c;
    int @r;
//-------------- count maximal size of each column, and sum up ----------------

    for ( @c=1; @c<=ncols(m); @c=@c+1)
    {  int @len(@c);
      for (@r=1; @r<=nrows(m); @r=@r+1)
      {
        @elems = @elems + 1;
        string @s(@elems) = string(m[@r,@c])+",";
        @slen = size(@s(@elems));
        if (@slen > @len(@c))
        {
          @len(@c) = @slen;
        }
      }
      @mlen = @mlen + @len(@c);
    }
//---------------------- print all - except last - rows -----------------------

    string @aus;
    string @sep = " ";
    if (@mlen >= pagewidth)
    {
      @sep = newline;
    }

    for (@r=1; @r<nrows(m); @r=@r+1)
    {
      @elems = @r;
      @aus = "";
      for (@c=1; @c<=ncols(m); @c=@c+1)
      {
        @aus = @aus + @s(@elems)[1,@len(@c)] + @sep;
        @elems = @elems + nrows(m);
      }
      @aus;
    }
//--------------- print last row (no comma after last entry) ------------------

    @aus = "";
    @elems = nrows(m);
    for (@c=1; @c<ncols(m); @c=@c+1)
    {
      @aus = @aus + @s(@elems)[1,@len(@c)] + @sep;
      @elems = @elems + nrows(m);
    }
    @aus = @aus + string(m[nrows(m),ncols(m)]);
    @aus;
    return();
  }
//--------- second case: input is a matrix, second argument is given ----------

  if ( size(#) == 1 )
  {
    if ( typeof(#[1]) == "int" )
    {
      string @aus;
      string @tmp;
      int @ll;
      int @c;
      int @r;
      for ( @r=1; @r<=nrows(m); @r=@r+1)
      {
        @aus = "";
        for (@c=1; @c<=ncols(m); @c=@c+1)
        {
          @tmp = string(m[@r,@c]);
          @aus = @aus + @tmp[1,#[1]] + " ";
        }
        @aus;
      }
    }
  }
}

"-------------------------------";
matrix m1[3][3]=maxideal(2);
pmat(m1);
minor(m1,3);
"---------------------------------";
ring r2=0,(x(1..9)),(dp);
matrix m2[3][3]=maxideal(1);
pmat(m2);
minor(m2,2);
"---------------------------------";
ring r3=0,(x(1..64)),ds;
matrix m3[8][8]= maxideal(1);
pmat(m3);
minor(m3,1);
"----------------------------------";
matrix m4[8][8]=maxideal(2);
pmat(m4);
minor(m4,1);
"----------------------------------";
listvar(all);
kill r1,r2,r3;
tst_status(1);$;
