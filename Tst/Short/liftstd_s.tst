LIB "tst.lib";
tst_init();
//
// test script for liftstd command
//
ring r1 = 32003,(x,y,z),(c,ls);
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
ideal i1=maxideal(3);
ideal i2=x6,y4,z5,xy,yz,xz;
i2;
matrix m1[3][3]=x,y,z,xz,yx,12zx,0,y2,3xy2;
pmat(m1);
liftstd(i2,m1);
"---------------------------------";
vector v1=[x4,y2,xz2];
vector v2=[y3,x8,2z4];
module m=v1,v2;
m;
pmat(m1);
liftstd(m,m1);
"--------------------------------";
listvar(all);
kill r1;
ring R;
poly f=x3+y7+z2+xyz;
ideal i=jacob(f);
matrix T;
ideal sm=liftstd(i,T);
matrix(sm)-matrix(i)*T;
kill R;
tst_status(1);$;
