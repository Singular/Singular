LIB "tst.lib";
tst_init();

option(noredefine);option(redSB);
LIB "matrix.lib";LIB "involut.lib";LIB "nctools.lib";LIB "polylib.lib";LIB "finvar.lib";
int homalg_variable_i; int homalg_variable_j; int homalg_variable_k; list homalg_variable_l;


proc IsMemberOfList (int i, list l)
{
  int k = size(l);

  for (int p=1; p<=k; p++)
  {
    if (l[p]==i)
    {
      return(1); // this is not a mistake
    }
  }
  return(0);
}


proc Difference (list a, list b)
{
  list c;
  int s=size(a);
  int l = 1;

  for (int p=1; p<=s; p++)
  {
    if (IsMemberOfList(a[p],b)==0)
    {
      c[l] = a[p]; l++;
    }
  }
  return(c);
}


proc GetSparseListOfHomalgMatrixAsString (M)
{
  list l;int k;
  k = 1;
  for(int i=1; i<=ncols(M); i++){
    for(int j=1; j<=nrows(M); j++){
      def p=M[j,i]; // remark: matrices are saved transposed in Singular
      if(p!=0){l[k]=list(i,j,p); k++;};
    };
  };
  return(string(l));
}


proc CreateListListOfIntegers (degrees,m,n)
{
  list l;
  for (int i=m; i>=1; i--)
  {
    l[i]=intvec(degrees[(i-1)*n+1..i*n]);
  }
  return(l);
}


proc IsZeroMatrix (matrix m)
{
  matrix z[nrows(m)][ncols(m)];
  return(m==z);
}


proc IsIdentityMatrix (matrix m)
{
  return(m==unitmat(nrows(m)));
}


proc IsDiagonalMatrix (matrix m)
{
  int min=nrows(m);
  if (min>ncols(m))
  {
    min=ncols(m);
  }
  matrix z[nrows(m)][ncols(m)];
  matrix c = m;
  for (int i=1; i<=min; i++)
  {
    c[i,i]=0;
  }
  return(c==z);
}


proc ZeroRows (module m)
{
  list l;
  int s = 1;
  for (int i=1;i<=ncols(m);i++)
  {
    if (m[i]==0)
    {
      l[s]=i; s++;
    }
  }
  if (size(l)==0)
  {
    return("[]"));
  }
  return(string(l));
}


proc ZeroColumns (matrix n)
{
  matrix m=module(transpose(n));
  list l;
  int s = 1;
  for (int i=1;i<=ncols(m);i++)
  {
    if (m[i]==0)
    {
      l[s]=i; s++;
    }
  }
  if (size(l)==0)
  {
    return("[]"));
  }
  return(string(l));
}


proc GetColumnIndependentUnitPositions (matrix M, list pos_list)
{
  int m = nrows(M);
  int n = ncols(M);

  list rest;
  for (int o=m; o>=1; o--)
  {
    rest[o] = o;
  }
  int r = m;
  list e;
  list rest2;
  list pos;
  int i; int k; int a; int s = 1;

  for (int j=1; j<=n; j++)
  {
    for (i=1; i<=r; i++)
    {
      k = rest[r-i+1];
      if (deg(leadmonom(M[k,j])) == 0) //IsUnit
      {
        rest2 = e;
        pos[s] = list(j,k); s++;
        for (a=1; a<=r; a++)
        {
          if (M[rest[a],j] == 0)
          {
            rest2[size(rest2)+1] = rest[a];
          }
        }
        rest = rest2;
        r = size(rest);
        break;
      }
    }
  }
  return(string(pos));
}


proc GetColumnIndependentUnitPositionsLocal (matrix M, list pos_list, matrix max_ideal)
{
  int m = nrows(M);
  int n = ncols(M);

  list rest;
  for (int o=m; o>=1; o--)
  {
    rest[o] = o;
  }
  int r = m;
  list e;
  list rest2;
  list pos;
  int i; int k; int a; int s = 1;

  for (int j=1; j<=n; j++)
  {
    for (i=1; i<=r; i++)
    {
      k = rest[r-i+1];
      if (reduce(M[k,j],max_ideal) != 0) //IsUnit
      {
        rest2 = e;
        pos[s] = list(j,k); s++;
        for (a=1; a<=r; a++)
        {
          if (M[rest[a],j] == 0)
          {
            rest2[size(rest2)+1] = rest[a];
          }
        }
        rest = rest2;
        r = size(rest);
        break;
      }
    }
  }
  return(string(pos));
}


proc GetRowIndependentUnitPositions (matrix M, list pos_list)
{
  int m = nrows(M);
  int n = ncols(M);

  list rest;
  for (int o=n; o>=1; o--)
  {
    rest[o] = o;
  }
  int r = n;
  list e;
  list rest2;
  list pos;
  int j; int k; int a; int s = 1;

  for (int i=1; i<=m; i++)
  {
    for (j=1; j<=r; j++)
    {
      k = rest[r-j+1];
      if (deg(leadmonom(M[i,k])) == 0) //IsUnit
      {
        rest2 = e;
        pos[s] = list(i,k); s++;
        for (a=1; a<=r; a++)
        {
          if (M[i,rest[a]] == 0)
          {
            rest2[size(rest2)+1] = rest[a];
          }
        }
        rest = rest2;
        r = size(rest);
        break;
      }
    }
  }
  return(string(pos));
}


proc GetRowIndependentUnitPositionsLocal (matrix M, list pos_list, matrix max_ideal)
{
  int m = nrows(M);
  int n = ncols(M);

  list rest;
  for (int o=n; o>=1; o--)
  {
    rest[o] = o;
  }
  int r = n;
  list e;
  list rest2;
  list pos;
  int j; int k; int a; int s = 1;

  for (int i=1; i<=m; i++)
  {
    for (j=1; j<=r; j++)
    {
      k = rest[r-j+1];
      if (reduce(M[i,k],max_ideal) != 0) //IsUnit
      {
        rest2 = e;
        pos[s] = list(i,k); s++;
        for (a=1; a<=r; a++)
        {
          if (M[i,rest[a]] == 0)
          {
            rest2[size(rest2)+1] = rest[a];
          }
        }
        rest = rest2;
        r = size(rest);
        break;
      }
    }
  }
  return(string(pos));
}


proc GetUnitPosition (matrix M, list pos_list)
{
  int m = nrows(M);
  int n = ncols(M);
  int r;
  list rest;
  for (int o=m; o>=1; o--)
  {
    rest[o] = o;
  }
  rest=Difference(rest,pos_list);
  r=size(rest);
  for (int j=1; j<=n; j++)
  {
    for (int i=1; i<=r; i++)
    {
      if (deg(leadmonom(M[rest[i],j])) == 0)//IsUnit
      {
        return(string(j,",",rest[i])); // this is not a mistake
      }
    }
  }
  return("fail");
}


proc GetUnitPositionLocal (matrix M, list pos_list, matrix max_ideal)
{
  int m = nrows(M);
  int n = ncols(M);
  int r;
  list rest;
  for (int o=m; o>=1; o--)
  {
    rest[o] = o;
  }
  rest=Difference(rest,pos_list);
  r=size(rest);
  for (int j=1; j<=n; j++)
  {
    for (int i=1; i<=r; i++)
    {
      if (reduce(M[rest[i],j],max_ideal) != 0)//IsUnit
      {
        return(string(j,",",rest[i])); // this is not a mistake
      }
    }
  }
  return("fail");
}


proc GetCleanRowsPositions (matrix m, list l)
{
  list rows;
  int s = 1;
  for (int i=1;i<=size(l);i++)
  {
    for (int j=1;j<=ncols(m);j++)
    {
      if (m[l[i],j]==1)
      {
        rows[s] = j; s++;
        break;
      }
    }
  }
  if (s==0)
  {
    return("[]"));
  }
  return(string(rows));
}


proc BasisOfRowModule (matrix M)
{
  return(std(M));
}


proc BasisOfColumnModule (matrix M)
{
  return(Involution(BasisOfRowModule(Involution(M))));
}


proc ReducedBasisOfRowModule (matrix M)
{
  return(mstd(M)[2]);
}


proc ReducedBasisOfColumnModule (matrix M)
{
  return(Involution(ReducedBasisOfRowModule(Involution(M))));
}


proc BasisOfRowsCoeff (matrix M)
{
  matrix B = std(M);
  matrix T = lift(M,B);
  list l = B,T;
  return(l)
}


proc BasisOfColumnsCoeff (matrix M)
{
  list l = BasisOfRowsCoeff(Involution(M));
  matrix B = l[1];
  matrix T = l[2];
  l = Involution(B),Involution(T);
  return(l);
}


proc DecideZeroRows (matrix A, matrix B)
{
  return(reduce(A,B));
}


proc DecideZeroColumns (matrix A, matrix B)
{
  return(Involution(reduce(Involution(A),Involution(B))));
}


proc DecideZeroRowsEffectively (matrix A, matrix B)
{
  matrix M = reduce(A,B);
  matrix T = lift(B,M-A);
  list l = M,T;
  return(l);
}


proc DecideZeroColumnsEffectively (matrix A, matrix B)
{
  list l = DecideZeroRowsEffectively(Involution(A),Involution(B));
  matrix B = l[1];
  matrix T = l[2];
  l = Involution(B),Involution(T);
  return(l);
}


proc SyzForHomalg (matrix M)
{
  return(syz(M));
}


proc SyzygiesGeneratorsOfRows (matrix M)
{
  return(SyzForHomalg(M));
}


proc SyzygiesGeneratorsOfColumns (matrix M)
{
  return(Involution(SyzForHomalg(Involution(M))));
}


proc RelativeSyzygiesGeneratorsOfRows (matrix M1, matrix M2)
{
  return(std(modulo(M1, M2)));
}


proc RelativeSyzygiesGeneratorsOfColumns (matrix M1, matrix M2)
{
  return(Involution(RelativeSyzygiesGeneratorsOfRows(Involution(M1),Involution(M2))));
}


proc ReducedSyzForHomalg (matrix M)
{
  return(matrix(nres(M,2)[2]));
}


proc ReducedSyzygiesGeneratorsOfRows (matrix M)
{
  return(ReducedSyzForHomalg(M));
}


proc ReducedSyzygiesGeneratorsOfColumns (matrix M)
{
  return(Involution(ReducedSyzForHomalg(Involution(M))));
}


ring r;
if ( deg(0,(1,1,1)) > 0 )  // this is a workaround for a bug in the 64 bit versions of Singular 3-0-4
{ proc Deg (pol,weights)
  {
    if ( pol == 0 )
    {
      return(deg(0));
    }
    return(deg(pol,weights));
  }
}
else
{ proc Deg (pol,weights)
  {
    return(deg(pol,weights));
  }
}
kill r;


proc MultiDeg (pol,weights)
{
  int mul=size(weights);
  intmat m[1][mul];
  for (int i=1; i<=mul; i++)
  {
    m[1,i]=Deg(pol,weights[i]);
  }
  return(m);
}


proc DegreesOfEntries (matrix M)
{
  intmat m[ncols(M)][nrows(M)];
  for (int i=1; i<=ncols(M); i++)
  {
    for (int j=1; j<=nrows(M); j++)
    {
      m[i,j] = deg(M[j,i]);
    }
  }
  return(m);
}


proc WeightedDegreesOfEntries (matrix M, weights)
{
  intmat m[ncols(M)][nrows(M)];
  for (int i=1; i<=ncols(M); i++)
  {
    for (int j=1; j<=nrows(M); j++)
    {
      m[i,j] = Deg(M[j,i],weights);
    }
  }
  return(m);
}


proc DecideZeroRowsEffectivelyLocal (matrix A, matrix B)
{
  list l = division(A,B);
  matrix U=l[3];
  for (int i=1; i<=ncols(U); i++)
  {
    if(U[i,i]==0){U[i,i]=1;};
  }
  l[3]=U;
  l[2] = A * l[3] - B * l[1];
  list l2 = CreateInputForLocalMatrixRows(l[2],l[3]);
  list l3 = CreateInputForLocalMatrixRows(-l[1],l[3]);
  list l = l2[1],l2[2],l3[1],l3[2];
  return(l);
}


proc DecideZeroColumnsEffectivelyLocal (matrix A, matrix B)
{
  list l = DecideZeroRowsEffectivelyLocal(Involution(A),Involution(B));
  matrix B = l[1];
  matrix T = l[3];
  l = Involution(B),l[2],Involution(T),l[4];
  return(l);
}


proc DecideZeroRowsLocal (matrix A, matrix B)
{
  list l=DecideZeroRowsEffectivelyLocal(A,B);
  l=l[1],l[2];
  return(l);
}


proc DecideZeroColumnsLocal (matrix A, matrix B)
{
  list l=DecideZeroColumnsEffectivelyLocal(A,B);
  l=l[1],l[2];
  return(l);
}


proc BasisOfRowsCoeffLocal (matrix M)
{
  matrix B = std(M);
  matrix U;
  matrix T = lift(M,B,U); //never use stdlift, also because it might differ from std!!!
  list l = CreateInputForLocalMatrixRows(T,U);
  l = B,l[1],l[2];
  return(l)
}


proc BasisOfColumnsCoeffLocal (matrix M)
{
  list l = BasisOfRowsCoeffLocal(Involution(M));
  matrix B = l[1];
  matrix T = l[2];
  l = Involution(B),Involution(T),l[3];
  return(l);
}


ring r= 0,(x),ds;
matrix M[2][2]=[1,0,0,0];
matrix NN[2][1]=[1,0];
module N=std(NN);
list l=division(M,N);
if( l[3][2,2]==0 ) // this is a workaround for a bug in the 64 bit versions of Singular 3-1-0
{ proc CreateInputForLocalMatrixRows (matrix A, matrix U)
  {
    poly u=1;
    matrix A2=A;
    for (int i=1; i<=ncols(U); i++)
    {
      if(U[i,i]!=0){u=lcm(u,U[i,i]);};
    }
    for (int i=1; i<=ncols(U); i++)
    {
      if(U[i,i]==0){
        poly gg=1;
      } else {
        poly uu=U[i,i];
        poly gg=u/uu;
      };
      if(gg!=1)
      {
        for(int k=1;k<=nrows(A2);k++){A2[k,i]=A2[k,i]*gg;};
      }
    }
    list l=A2,u;
    return(l);
  }
}
else
{ proc CreateInputForLocalMatrixRows (matrix A, matrix U)
  {
    poly u=1;
    matrix A2=A;
    for (int i=1; i<=ncols(U); i++)
    {
      u=lcm(u,U[i,i]);
    }
    for (int i=1; i<=ncols(U); i++)
    {
      poly uu=U[i,i];
      poly gg=u/uu;
      if(gg!=1)
      {
        for(int k=1;k<=nrows(A2);k++){A2[k,i]=A2[k,i]*gg;};
      }
    }
    list l=A2,u;
    return(l);
  }
}
kill r;

proc PrimaryDecomposition (matrix m)
{
  return(primdecSY(m))
}


proc NonTrivialDegreePerRow (matrix M)
{
  int b = 1;
  intmat m[1][ncols(M)];
  int d = deg(0);
  for (int i=1; i<=ncols(M); i++)
  {
    for (int j=1; j<=nrows(M); j++)
    {
      if ( deg(M[j,i]) <> d ) { m[1,i] = deg(M[j,i]); break; }
    }
    if ( b && i > 1 ) { if ( m[1,i] <> m[1,i-1] ) { b = 0; } } // Singular is strange
  }
  if ( b ) { return(m[1,1]); } else { return(m); }
}


proc NonTrivialWeightedDegreePerRow (matrix M, weights)
{
  int b = 1;
  intmat m[1][ncols(M)];
  int d = Deg(0,weights);
  for (int i=1; i<=ncols(M); i++)
  {
    for (int j=1; j<=nrows(M); j++)
    {
      if ( Deg(M[j,i],weights) <> d ) { m[1,i] = Deg(M[j,i],weights); break; }
    }
    if ( b && i > 1 ) { if ( m[1,i] <> m[1,i-1] ) { b = 0; } } // Singular is strange
  }
  if ( b ) { return(m[1,1]); } else { return(m); }
}


proc NonTrivialDegreePerRowWithColPosition(matrix M)
{
  intmat m[2][ncols(M)];
  int d = deg(0);
  for (int i=1; i<=ncols(M); i++)
  {
    for (int j=1; j<=nrows(M); j++)
    {
      if ( deg(M[j,i]) <> d ) { m[1,i] = deg(M[j,i]); m[2,i] = j; break; }
    }
  }
  return(m);
}


proc NonTrivialWeightedDegreePerRowWithColPosition(matrix M, weights)
{
  intmat m[2][ncols(M)];
  int d = Deg(0,weights);
  for (int i=1; i<=ncols(M); i++)
  {
    for (int j=1; j<=nrows(M); j++)
    {
      if ( Deg(M[j,i],weights) <> d ) { m[1,i] = Deg(M[j,i],weights); m[2,i] = j; break; }
    }
  }
  return(m);
}


proc NonTrivialDegreePerColumn (matrix M)
{
  int b = 1;
  intmat m[1][nrows(M)];
  int d = deg(0);
  for (int j=1; j<=nrows(M); j++)
  {
    for (int i=1; i<=ncols(M); i++)
    {
      if ( deg(M[j,i]) <> d ) { m[1,j] = deg(M[j,i]); break; }
    }
    if ( b && j > 1 ) { if ( m[1,j] <> m[1,j-1] ) { b = 0; } } // Singular is strange
  }
  if ( b ) { return(m[1,1]); } else { return(m); }
}


proc NonTrivialWeightedDegreePerColumn (matrix M, weights)
{
  int b = 1;
  intmat m[1][nrows(M)];
  int d = Deg(0,weights);
  for (int j=1; j<=nrows(M); j++)
  {
    for (int i=1; i<=ncols(M); i++)
    {
      if ( Deg(M[j,i],weights) <> d ) { m[1,j] = Deg(M[j,i],weights); break; }
    }
    if ( b && j > 1 ) { if ( m[1,j] <> m[1,j-1] ) { b = 0; } } // Singular is strange
  }
  if ( b ) { return(m[1,1]); } else { return(m); }
}


proc NonTrivialDegreePerColumnWithRowPosition (matrix M)
{
  intmat m[2][nrows(M)];
  int d = deg(0);
  for (int j=1; j<=nrows(M); j++)
  {
    for (int i=1; i<=ncols(M); i++)
    {
      if ( deg(M[j,i]) <> d ) { m[1,j] = deg(M[j,i]); m[2,j] = i; break; }
    }
  }
  return(m);
}


proc NonTrivialWeightedDegreePerColumnWithRowPosition (matrix M, weights)
{
  intmat m[2][nrows(M)];
  int d = Deg(0,weights);
  for (int j=1; j<=nrows(M); j++)
  {
    for (int i=1; i<=ncols(M); i++)
    {
      if ( Deg(M[j,i],weights) <> d ) { m[1,j] = Deg(M[j,i],weights); m[2,j] = i; break; }
    }
  }
  return(m);
}


proc Diff (matrix m, matrix n) // following the Macaulay2 convention
{
  int f = nrows(m);
  int p = ncols(m);
  int g = nrows(n);
  int q = ncols(n);
  matrix h[f*g][p*q]=0;
  for (int i=1; i<=f; i=i+1)
    {
    for (int j=1; j<=g; j=j+1)
      {
      for (int k=1; k<=p; k=k+1)
        {
        for (int l=1; l<=q; l=l+1)
          {
            h[g*(i-1)+j,q*(k-1)+l] = diff( ideal(m[i,k]), ideal(n[j,l]) )[1,1];
          }
        }
      }
    }
  return(h)
}

ring homalg_variable_1 = 0,dummy_variable,dp;
setring homalg_variable_1;
short=0;option(redTail);

proc Involution (matrix m)
{
  return(transpose(m));
}

matrix homalg_variable_2[120][8] = 1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,1,0,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,0,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1,1,1,1,0,0,0,1,1,2,1,0,0,0,0,1,1,1,1,1,0,0,0,1,0,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,2,1,0,0,0,1,1,1,1,1,1,0,0,1,0,1,1,1,1,1,0,0,1,1,2,1,1,0,0,0,1,1,1,1,1,1,0,0,1,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,2,2,1,0,0,0,1,1,1,2,1,1,0,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,1,2,2,1,0,0,0,1,1,2,1,1,1,0,0,1,1,1,1,1,1,1,1,1,2,2,1,1,0,0,1,1,1,2,2,1,0,0,1,1,1,2,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,2,2,1,1,0,0,1,1,2,1,1,1,1,1,1,2,2,2,1,0,0,1,1,2,2,1,1,1,0,1,1,1,2,2,1,1,0,1,1,1,2,1,1,1,1,0,1,1,2,2,2,1,0,0,1,1,2,2,1,1,1,1,1,2,3,2,1,0,0,1,1,2,2,2,1,1,0,1,1,2,2,1,1,1,1,1,1,1,2,2,2,1,0,1,1,1,2,2,1,1,1,0,1,1,2,2,2,1,1,1,2,2,3,2,1,0,0,1,1,2,3,2,1,1,0,1,1,2,2,2,2,1,0,1,1,2,2,2,1,1,1,1,1,1,2,2,2,1,1,0,1,1,2,2,2,2,1,1,2,2,3,2,1,1,0,1,1,2,3,2,2,1,0,1,1,2,3,2,1,1,1,1,1,2,2,2,2,1,1,1,1,1,2,2,2,2,1,1,2,2,3,2,2,1,0,1,2,2,3,2,1,1,1,1,1,2,3,3,2,1,0,1,1,2,3,2,2,1,1,1,1,2,2,2,2,2,1,1,2,2,3,3,2,1,0,1,2,2,3,2,2,1,1,1,1,2,3,3,2,1,1,1,1,2,3,2,2,2,1,1,2,2,4,3,2,1,0,1,2,2,3,3,2,1,1,1,2,2,3,2,2,2,1,1,1,2,3,3,2,2,1,1,2,3,4,3,2,1,0,1,2,2,4,3,2,1,1,1,2,2,3,3,2,2,1,1,1,2,3,3,3,2,1,2,2,3,4,3,2,1,0,1,2,3,4,3,2,1,1,1,2,2,4,3,2,2,1,1,2,2,3,3,3,2,1,2,2,3,4,3,2,1,1,1,2,3,4,3,2,2,1,1,2,2,4,3,3,2,1,2,2,3,4,3,2,2,1,1,2,3,4,3,3,2,1,1,2,2,4,4,3,2,1,2,2,3,4,3,3,2,1,1,2,3,4,4,3,2,1,2,2,3,4,4,3,2,1,1,2,3,5,4,3,2,1,2,2,3,5,4,3,2,1,1,3,3,5,4,3,2,1,2,3,3,5,4,3,2,1,2,2,4,5,4,3,2,1,2,3,4,5,4,3,2,1,2,3,4,6,4,3,2,1,2,3,4,6,5,3,2,1,2,3,4,6,5,4,2,1,2,3,4,6,5,4,3,1,2,3,4,6,5,4,3,2;
homalg_variable_2 = transpose(homalg_variable_2);
ring homalg_variable_3 = (0),(x1,x2,x3,x4,x5,x6,x7,x8),dp;
setring homalg_variable_3;
short=0;option(redTail);

proc Involution (matrix m)
{
  return(transpose(m));
}

setring homalg_variable_1;
short=0;option(redTail);

proc Involution (matrix m)
{
  return(transpose(m));
}

matrix homalg_variable_4 = Involution(homalg_variable_2);
setring homalg_variable_3;
short=0;option(redTail);

proc Involution (matrix m)
{
  return(transpose(m));
}

matrix homalg_variable_5 = imap(homalg_variable_1,homalg_variable_4);
matrix homalg_variable_6[1][8] = x1,x2,x3,x4,x5,x6,x7,x8;
homalg_variable_6 = transpose(homalg_variable_6);
matrix homalg_variable_7 = submat(homalg_variable_5,intvec( 1 ),1..8);
matrix homalg_variable_8 = homalg_variable_7*homalg_variable_6;
IsZeroMatrix(homalg_variable_8);
matrix homalg_variable_9 = submat(homalg_variable_5,intvec( 2 ),1..8);
matrix homalg_variable_10 = homalg_variable_9*homalg_variable_6;
IsZeroMatrix(homalg_variable_10);
matrix homalg_variable_11 = submat(homalg_variable_5,intvec( 3 ),1..8);
matrix homalg_variable_12 = homalg_variable_11*homalg_variable_6;
IsZeroMatrix(homalg_variable_12);
matrix homalg_variable_13 = submat(homalg_variable_5,intvec( 4 ),1..8);
matrix homalg_variable_14 = homalg_variable_13*homalg_variable_6;
IsZeroMatrix(homalg_variable_14);
matrix homalg_variable_15 = submat(homalg_variable_5,intvec( 5 ),1..8);
matrix homalg_variable_16 = homalg_variable_15*homalg_variable_6;
IsZeroMatrix(homalg_variable_16);
matrix homalg_variable_17 = submat(homalg_variable_5,intvec( 6 ),1..8);
matrix homalg_variable_18 = homalg_variable_17*homalg_variable_6;
IsZeroMatrix(homalg_variable_18);
matrix homalg_variable_19 = submat(homalg_variable_5,intvec( 7 ),1..8);
matrix homalg_variable_20 = homalg_variable_19*homalg_variable_6;
IsZeroMatrix(homalg_variable_20);
matrix homalg_variable_21 = submat(homalg_variable_5,intvec( 8 ),1..8);
matrix homalg_variable_22 = homalg_variable_21*homalg_variable_6;
IsZeroMatrix(homalg_variable_22);
matrix homalg_variable_23 = submat(homalg_variable_5,intvec( 9 ),1..8);
matrix homalg_variable_24 = homalg_variable_23*homalg_variable_6;
IsZeroMatrix(homalg_variable_24);
matrix homalg_variable_25 = submat(homalg_variable_5,intvec( 10 ),1..8);
matrix homalg_variable_26 = homalg_variable_25*homalg_variable_6;
IsZeroMatrix(homalg_variable_26);
matrix homalg_variable_27 = submat(homalg_variable_5,intvec( 11 ),1..8);
matrix homalg_variable_28 = homalg_variable_27*homalg_variable_6;
IsZeroMatrix(homalg_variable_28);
matrix homalg_variable_29 = submat(homalg_variable_5,intvec( 12 ),1..8);
matrix homalg_variable_30 = homalg_variable_29*homalg_variable_6;
IsZeroMatrix(homalg_variable_30);
matrix homalg_variable_31 = submat(homalg_variable_5,intvec( 13 ),1..8);
matrix homalg_variable_32 = homalg_variable_31*homalg_variable_6;
IsZeroMatrix(homalg_variable_32);
matrix homalg_variable_33 = submat(homalg_variable_5,intvec( 14 ),1..8);
matrix homalg_variable_34 = homalg_variable_33*homalg_variable_6;
IsZeroMatrix(homalg_variable_34);
matrix homalg_variable_35 = submat(homalg_variable_5,intvec( 15 ),1..8);
matrix homalg_variable_36 = homalg_variable_35*homalg_variable_6;
IsZeroMatrix(homalg_variable_36);
matrix homalg_variable_37 = submat(homalg_variable_5,intvec( 16 ),1..8);
matrix homalg_variable_38 = homalg_variable_37*homalg_variable_6;
IsZeroMatrix(homalg_variable_38);
matrix homalg_variable_39 = submat(homalg_variable_5,intvec( 17 ),1..8);
matrix homalg_variable_40 = homalg_variable_39*homalg_variable_6;
IsZeroMatrix(homalg_variable_40);
matrix homalg_variable_41 = submat(homalg_variable_5,intvec( 18 ),1..8);
matrix homalg_variable_42 = homalg_variable_41*homalg_variable_6;
IsZeroMatrix(homalg_variable_42);
matrix homalg_variable_43 = submat(homalg_variable_5,intvec( 19 ),1..8);
matrix homalg_variable_44 = homalg_variable_43*homalg_variable_6;
IsZeroMatrix(homalg_variable_44);
matrix homalg_variable_45 = submat(homalg_variable_5,intvec( 20 ),1..8);
matrix homalg_variable_46 = homalg_variable_45*homalg_variable_6;
IsZeroMatrix(homalg_variable_46);
matrix homalg_variable_47 = submat(homalg_variable_5,intvec( 21 ),1..8);
matrix homalg_variable_48 = homalg_variable_47*homalg_variable_6;
IsZeroMatrix(homalg_variable_48);
matrix homalg_variable_49 = submat(homalg_variable_5,intvec( 22 ),1..8);
matrix homalg_variable_50 = homalg_variable_49*homalg_variable_6;
IsZeroMatrix(homalg_variable_50);
matrix homalg_variable_51 = submat(homalg_variable_5,intvec( 23 ),1..8);
matrix homalg_variable_52 = homalg_variable_51*homalg_variable_6;
IsZeroMatrix(homalg_variable_52);
matrix homalg_variable_53 = submat(homalg_variable_5,intvec( 24 ),1..8);
matrix homalg_variable_54 = homalg_variable_53*homalg_variable_6;
IsZeroMatrix(homalg_variable_54);
matrix homalg_variable_55 = submat(homalg_variable_5,intvec( 25 ),1..8);
matrix homalg_variable_56 = homalg_variable_55*homalg_variable_6;
IsZeroMatrix(homalg_variable_56);
matrix homalg_variable_57 = submat(homalg_variable_5,intvec( 26 ),1..8);
matrix homalg_variable_58 = homalg_variable_57*homalg_variable_6;
IsZeroMatrix(homalg_variable_58);
matrix homalg_variable_59 = submat(homalg_variable_5,intvec( 27 ),1..8);
matrix homalg_variable_60 = homalg_variable_59*homalg_variable_6;
IsZeroMatrix(homalg_variable_60);
matrix homalg_variable_61 = submat(homalg_variable_5,intvec( 28 ),1..8);
matrix homalg_variable_62 = homalg_variable_61*homalg_variable_6;
IsZeroMatrix(homalg_variable_62);
matrix homalg_variable_63 = submat(homalg_variable_5,intvec( 29 ),1..8);
matrix homalg_variable_64 = homalg_variable_63*homalg_variable_6;
IsZeroMatrix(homalg_variable_64);
matrix homalg_variable_65 = submat(homalg_variable_5,intvec( 30 ),1..8);
matrix homalg_variable_66 = homalg_variable_65*homalg_variable_6;
IsZeroMatrix(homalg_variable_66);
matrix homalg_variable_67 = submat(homalg_variable_5,intvec( 31 ),1..8);
matrix homalg_variable_68 = homalg_variable_67*homalg_variable_6;
IsZeroMatrix(homalg_variable_68);
matrix homalg_variable_69 = submat(homalg_variable_5,intvec( 32 ),1..8);
matrix homalg_variable_70 = homalg_variable_69*homalg_variable_6;
IsZeroMatrix(homalg_variable_70);
matrix homalg_variable_71 = submat(homalg_variable_5,intvec( 33 ),1..8);
matrix homalg_variable_72 = homalg_variable_71*homalg_variable_6;
IsZeroMatrix(homalg_variable_72);
matrix homalg_variable_73 = submat(homalg_variable_5,intvec( 34 ),1..8);
matrix homalg_variable_74 = homalg_variable_73*homalg_variable_6;
IsZeroMatrix(homalg_variable_74);
matrix homalg_variable_75 = submat(homalg_variable_5,intvec( 35 ),1..8);
matrix homalg_variable_76 = homalg_variable_75*homalg_variable_6;
IsZeroMatrix(homalg_variable_76);
matrix homalg_variable_77 = submat(homalg_variable_5,intvec( 36 ),1..8);
matrix homalg_variable_78 = homalg_variable_77*homalg_variable_6;
IsZeroMatrix(homalg_variable_78);
matrix homalg_variable_79 = submat(homalg_variable_5,intvec( 37 ),1..8);
matrix homalg_variable_80 = homalg_variable_79*homalg_variable_6;
IsZeroMatrix(homalg_variable_80);
matrix homalg_variable_81 = submat(homalg_variable_5,intvec( 38 ),1..8);
matrix homalg_variable_82 = homalg_variable_81*homalg_variable_6;
IsZeroMatrix(homalg_variable_82);
matrix homalg_variable_83 = submat(homalg_variable_5,intvec( 39 ),1..8);
matrix homalg_variable_84 = homalg_variable_83*homalg_variable_6;
IsZeroMatrix(homalg_variable_84);
matrix homalg_variable_85 = submat(homalg_variable_5,intvec( 40 ),1..8);
matrix homalg_variable_86 = homalg_variable_85*homalg_variable_6;
IsZeroMatrix(homalg_variable_86);
matrix homalg_variable_87 = submat(homalg_variable_5,intvec( 41 ),1..8);
matrix homalg_variable_88 = homalg_variable_87*homalg_variable_6;
IsZeroMatrix(homalg_variable_88);
matrix homalg_variable_89 = submat(homalg_variable_5,intvec( 42 ),1..8);
matrix homalg_variable_90 = homalg_variable_89*homalg_variable_6;
IsZeroMatrix(homalg_variable_90);
matrix homalg_variable_91 = submat(homalg_variable_5,intvec( 43 ),1..8);
matrix homalg_variable_92 = homalg_variable_91*homalg_variable_6;
IsZeroMatrix(homalg_variable_92);
matrix homalg_variable_93 = submat(homalg_variable_5,intvec( 44 ),1..8);
matrix homalg_variable_94 = homalg_variable_93*homalg_variable_6;
IsZeroMatrix(homalg_variable_94);
matrix homalg_variable_95 = submat(homalg_variable_5,intvec( 45 ),1..8);
matrix homalg_variable_96 = homalg_variable_95*homalg_variable_6;
IsZeroMatrix(homalg_variable_96);
matrix homalg_variable_97 = submat(homalg_variable_5,intvec( 46 ),1..8);
matrix homalg_variable_98 = homalg_variable_97*homalg_variable_6;
IsZeroMatrix(homalg_variable_98);
matrix homalg_variable_99 = submat(homalg_variable_5,intvec( 47 ),1..8);
matrix homalg_variable_100 = homalg_variable_99*homalg_variable_6;
IsZeroMatrix(homalg_variable_100);
matrix homalg_variable_101 = submat(homalg_variable_5,intvec( 48 ),1..8);
matrix homalg_variable_102 = homalg_variable_101*homalg_variable_6;
IsZeroMatrix(homalg_variable_102);
matrix homalg_variable_103 = submat(homalg_variable_5,intvec( 49 ),1..8);
matrix homalg_variable_104 = homalg_variable_103*homalg_variable_6;
IsZeroMatrix(homalg_variable_104);
matrix homalg_variable_105 = submat(homalg_variable_5,intvec( 50 ),1..8);
matrix homalg_variable_106 = homalg_variable_105*homalg_variable_6;
IsZeroMatrix(homalg_variable_106);
matrix homalg_variable_107 = submat(homalg_variable_5,intvec( 51 ),1..8);
matrix homalg_variable_108 = homalg_variable_107*homalg_variable_6;
IsZeroMatrix(homalg_variable_108);
matrix homalg_variable_109 = submat(homalg_variable_5,intvec( 52 ),1..8);
matrix homalg_variable_110 = homalg_variable_109*homalg_variable_6;
IsZeroMatrix(homalg_variable_110);
matrix homalg_variable_111 = submat(homalg_variable_5,intvec( 53 ),1..8);
matrix homalg_variable_112 = homalg_variable_111*homalg_variable_6;
IsZeroMatrix(homalg_variable_112);
matrix homalg_variable_113 = submat(homalg_variable_5,intvec( 54 ),1..8);
matrix homalg_variable_114 = homalg_variable_113*homalg_variable_6;
IsZeroMatrix(homalg_variable_114);
matrix homalg_variable_115 = submat(homalg_variable_5,intvec( 55 ),1..8);
matrix homalg_variable_116 = homalg_variable_115*homalg_variable_6;
IsZeroMatrix(homalg_variable_116);
matrix homalg_variable_117 = submat(homalg_variable_5,intvec( 56 ),1..8);
matrix homalg_variable_118 = homalg_variable_117*homalg_variable_6;
IsZeroMatrix(homalg_variable_118);
matrix homalg_variable_119 = submat(homalg_variable_5,intvec( 57 ),1..8);
matrix homalg_variable_120 = homalg_variable_119*homalg_variable_6;
IsZeroMatrix(homalg_variable_120);
matrix homalg_variable_121 = submat(homalg_variable_5,intvec( 58 ),1..8);
matrix homalg_variable_122 = homalg_variable_121*homalg_variable_6;
IsZeroMatrix(homalg_variable_122);
matrix homalg_variable_123 = submat(homalg_variable_5,intvec( 59 ),1..8);
matrix homalg_variable_124 = homalg_variable_123*homalg_variable_6;
IsZeroMatrix(homalg_variable_124);
matrix homalg_variable_125 = submat(homalg_variable_5,intvec( 60 ),1..8);
matrix homalg_variable_126 = homalg_variable_125*homalg_variable_6;
IsZeroMatrix(homalg_variable_126);
matrix homalg_variable_127 = submat(homalg_variable_5,intvec( 61 ),1..8);
matrix homalg_variable_128 = homalg_variable_127*homalg_variable_6;
IsZeroMatrix(homalg_variable_128);
matrix homalg_variable_129 = submat(homalg_variable_5,intvec( 62 ),1..8);
matrix homalg_variable_130 = homalg_variable_129*homalg_variable_6;
IsZeroMatrix(homalg_variable_130);
matrix homalg_variable_131 = submat(homalg_variable_5,intvec( 63 ),1..8);
matrix homalg_variable_132 = homalg_variable_131*homalg_variable_6;
IsZeroMatrix(homalg_variable_132);
matrix homalg_variable_133 = submat(homalg_variable_5,intvec( 64 ),1..8);
matrix homalg_variable_134 = homalg_variable_133*homalg_variable_6;
IsZeroMatrix(homalg_variable_134);
matrix homalg_variable_135 = submat(homalg_variable_5,intvec( 65 ),1..8);
matrix homalg_variable_136 = homalg_variable_135*homalg_variable_6;
IsZeroMatrix(homalg_variable_136);
matrix homalg_variable_137 = submat(homalg_variable_5,intvec( 66 ),1..8);
matrix homalg_variable_138 = homalg_variable_137*homalg_variable_6;
IsZeroMatrix(homalg_variable_138);
matrix homalg_variable_139 = submat(homalg_variable_5,intvec( 67 ),1..8);
matrix homalg_variable_140 = homalg_variable_139*homalg_variable_6;
IsZeroMatrix(homalg_variable_140);
matrix homalg_variable_141 = submat(homalg_variable_5,intvec( 68 ),1..8);
matrix homalg_variable_142 = homalg_variable_141*homalg_variable_6;
IsZeroMatrix(homalg_variable_142);
matrix homalg_variable_143 = submat(homalg_variable_5,intvec( 69 ),1..8);
matrix homalg_variable_144 = homalg_variable_143*homalg_variable_6;
IsZeroMatrix(homalg_variable_144);
matrix homalg_variable_145 = submat(homalg_variable_5,intvec( 70 ),1..8);
matrix homalg_variable_146 = homalg_variable_145*homalg_variable_6;
IsZeroMatrix(homalg_variable_146);
matrix homalg_variable_147 = submat(homalg_variable_5,intvec( 71 ),1..8);
matrix homalg_variable_148 = homalg_variable_147*homalg_variable_6;
IsZeroMatrix(homalg_variable_148);
matrix homalg_variable_149 = submat(homalg_variable_5,intvec( 72 ),1..8);
matrix homalg_variable_150 = homalg_variable_149*homalg_variable_6;
IsZeroMatrix(homalg_variable_150);
matrix homalg_variable_151 = submat(homalg_variable_5,intvec( 73 ),1..8);
matrix homalg_variable_152 = homalg_variable_151*homalg_variable_6;
IsZeroMatrix(homalg_variable_152);
matrix homalg_variable_153 = submat(homalg_variable_5,intvec( 74 ),1..8);
matrix homalg_variable_154 = homalg_variable_153*homalg_variable_6;
IsZeroMatrix(homalg_variable_154);
matrix homalg_variable_155 = submat(homalg_variable_5,intvec( 75 ),1..8);
matrix homalg_variable_156 = homalg_variable_155*homalg_variable_6;
IsZeroMatrix(homalg_variable_156);
matrix homalg_variable_157 = submat(homalg_variable_5,intvec( 76 ),1..8);
matrix homalg_variable_158 = homalg_variable_157*homalg_variable_6;
IsZeroMatrix(homalg_variable_158);
matrix homalg_variable_159 = submat(homalg_variable_5,intvec( 77 ),1..8);
matrix homalg_variable_160 = homalg_variable_159*homalg_variable_6;
IsZeroMatrix(homalg_variable_160);
matrix homalg_variable_161 = submat(homalg_variable_5,intvec( 78 ),1..8);
matrix homalg_variable_162 = homalg_variable_161*homalg_variable_6;
IsZeroMatrix(homalg_variable_162);
matrix homalg_variable_163 = submat(homalg_variable_5,intvec( 79 ),1..8);
matrix homalg_variable_164 = homalg_variable_163*homalg_variable_6;
IsZeroMatrix(homalg_variable_164);
matrix homalg_variable_165 = submat(homalg_variable_5,intvec( 80 ),1..8);
matrix homalg_variable_166 = homalg_variable_165*homalg_variable_6;
IsZeroMatrix(homalg_variable_166);
matrix homalg_variable_167 = submat(homalg_variable_5,intvec( 81 ),1..8);
matrix homalg_variable_168 = homalg_variable_167*homalg_variable_6;
IsZeroMatrix(homalg_variable_168);
matrix homalg_variable_169 = submat(homalg_variable_5,intvec( 82 ),1..8);
matrix homalg_variable_170 = homalg_variable_169*homalg_variable_6;
IsZeroMatrix(homalg_variable_170);
matrix homalg_variable_171 = submat(homalg_variable_5,intvec( 83 ),1..8);
matrix homalg_variable_172 = homalg_variable_171*homalg_variable_6;
IsZeroMatrix(homalg_variable_172);
matrix homalg_variable_173 = submat(homalg_variable_5,intvec( 84 ),1..8);
matrix homalg_variable_174 = homalg_variable_173*homalg_variable_6;
IsZeroMatrix(homalg_variable_174);
matrix homalg_variable_175 = submat(homalg_variable_5,intvec( 85 ),1..8);
matrix homalg_variable_176 = homalg_variable_175*homalg_variable_6;
IsZeroMatrix(homalg_variable_176);
matrix homalg_variable_177 = submat(homalg_variable_5,intvec( 86 ),1..8);
matrix homalg_variable_178 = homalg_variable_177*homalg_variable_6;
IsZeroMatrix(homalg_variable_178);
matrix homalg_variable_179 = submat(homalg_variable_5,intvec( 87 ),1..8);
matrix homalg_variable_180 = homalg_variable_179*homalg_variable_6;
IsZeroMatrix(homalg_variable_180);
matrix homalg_variable_181 = submat(homalg_variable_5,intvec( 88 ),1..8);
matrix homalg_variable_182 = homalg_variable_181*homalg_variable_6;
IsZeroMatrix(homalg_variable_182);
matrix homalg_variable_183 = submat(homalg_variable_5,intvec( 89 ),1..8);
matrix homalg_variable_184 = homalg_variable_183*homalg_variable_6;
IsZeroMatrix(homalg_variable_184);
matrix homalg_variable_185 = submat(homalg_variable_5,intvec( 90 ),1..8);
matrix homalg_variable_186 = homalg_variable_185*homalg_variable_6;
IsZeroMatrix(homalg_variable_186);
matrix homalg_variable_187 = submat(homalg_variable_5,intvec( 91 ),1..8);
matrix homalg_variable_188 = homalg_variable_187*homalg_variable_6;
IsZeroMatrix(homalg_variable_188);
matrix homalg_variable_189 = submat(homalg_variable_5,intvec( 92 ),1..8);
matrix homalg_variable_190 = homalg_variable_189*homalg_variable_6;
IsZeroMatrix(homalg_variable_190);
matrix homalg_variable_191 = submat(homalg_variable_5,intvec( 93 ),1..8);
matrix homalg_variable_192 = homalg_variable_191*homalg_variable_6;
IsZeroMatrix(homalg_variable_192);
matrix homalg_variable_193 = submat(homalg_variable_5,intvec( 94 ),1..8);
matrix homalg_variable_194 = homalg_variable_193*homalg_variable_6;
IsZeroMatrix(homalg_variable_194);
matrix homalg_variable_195 = submat(homalg_variable_5,intvec( 95 ),1..8);
matrix homalg_variable_196 = homalg_variable_195*homalg_variable_6;
IsZeroMatrix(homalg_variable_196);
matrix homalg_variable_197 = submat(homalg_variable_5,intvec( 96 ),1..8);
matrix homalg_variable_198 = homalg_variable_197*homalg_variable_6;
IsZeroMatrix(homalg_variable_198);
matrix homalg_variable_199 = submat(homalg_variable_5,intvec( 97 ),1..8);
matrix homalg_variable_200 = homalg_variable_199*homalg_variable_6;
IsZeroMatrix(homalg_variable_200);
matrix homalg_variable_201 = submat(homalg_variable_5,intvec( 98 ),1..8);
matrix homalg_variable_202 = homalg_variable_201*homalg_variable_6;
IsZeroMatrix(homalg_variable_202);
matrix homalg_variable_203 = submat(homalg_variable_5,intvec( 99 ),1..8);
matrix homalg_variable_204 = homalg_variable_203*homalg_variable_6;
IsZeroMatrix(homalg_variable_204);
matrix homalg_variable_205 = submat(homalg_variable_5,intvec( 100 ),1..8);
matrix homalg_variable_206 = homalg_variable_205*homalg_variable_6;
IsZeroMatrix(homalg_variable_206);
matrix homalg_variable_207 = submat(homalg_variable_5,intvec( 101 ),1..8);
matrix homalg_variable_208 = homalg_variable_207*homalg_variable_6;
IsZeroMatrix(homalg_variable_208);
matrix homalg_variable_209 = submat(homalg_variable_5,intvec( 102 ),1..8);
matrix homalg_variable_210 = homalg_variable_209*homalg_variable_6;
IsZeroMatrix(homalg_variable_210);
matrix homalg_variable_211 = submat(homalg_variable_5,intvec( 103 ),1..8);
matrix homalg_variable_212 = homalg_variable_211*homalg_variable_6;
IsZeroMatrix(homalg_variable_212);
matrix homalg_variable_213 = submat(homalg_variable_5,intvec( 104 ),1..8);
matrix homalg_variable_214 = homalg_variable_213*homalg_variable_6;
IsZeroMatrix(homalg_variable_214);
matrix homalg_variable_215 = submat(homalg_variable_5,intvec( 105 ),1..8);
matrix homalg_variable_216 = homalg_variable_215*homalg_variable_6;
IsZeroMatrix(homalg_variable_216);
matrix homalg_variable_217 = submat(homalg_variable_5,intvec( 106 ),1..8);
matrix homalg_variable_218 = homalg_variable_217*homalg_variable_6;
IsZeroMatrix(homalg_variable_218);
matrix homalg_variable_219 = submat(homalg_variable_5,intvec( 107 ),1..8);
matrix homalg_variable_220 = homalg_variable_219*homalg_variable_6;
IsZeroMatrix(homalg_variable_220);
matrix homalg_variable_221 = submat(homalg_variable_5,intvec( 108 ),1..8);
matrix homalg_variable_222 = homalg_variable_221*homalg_variable_6;
IsZeroMatrix(homalg_variable_222);
matrix homalg_variable_223 = submat(homalg_variable_5,intvec( 109 ),1..8);
matrix homalg_variable_224 = homalg_variable_223*homalg_variable_6;
IsZeroMatrix(homalg_variable_224);
matrix homalg_variable_225 = submat(homalg_variable_5,intvec( 110 ),1..8);
matrix homalg_variable_226 = homalg_variable_225*homalg_variable_6;
IsZeroMatrix(homalg_variable_226);
matrix homalg_variable_227 = submat(homalg_variable_5,intvec( 111 ),1..8);
matrix homalg_variable_228 = homalg_variable_227*homalg_variable_6;
IsZeroMatrix(homalg_variable_228);
matrix homalg_variable_229 = submat(homalg_variable_5,intvec( 112 ),1..8);
matrix homalg_variable_230 = homalg_variable_229*homalg_variable_6;
IsZeroMatrix(homalg_variable_230);
matrix homalg_variable_231 = submat(homalg_variable_5,intvec( 113 ),1..8);
matrix homalg_variable_232 = homalg_variable_231*homalg_variable_6;
IsZeroMatrix(homalg_variable_232);
matrix homalg_variable_233 = submat(homalg_variable_5,intvec( 114 ),1..8);
matrix homalg_variable_234 = homalg_variable_233*homalg_variable_6;
IsZeroMatrix(homalg_variable_234);
matrix homalg_variable_235 = submat(homalg_variable_5,intvec( 115 ),1..8);
matrix homalg_variable_236 = homalg_variable_235*homalg_variable_6;
IsZeroMatrix(homalg_variable_236);
matrix homalg_variable_237 = submat(homalg_variable_5,intvec( 116 ),1..8);
matrix homalg_variable_238 = homalg_variable_237*homalg_variable_6;
IsZeroMatrix(homalg_variable_238);
matrix homalg_variable_239 = submat(homalg_variable_5,intvec( 117 ),1..8);
matrix homalg_variable_240 = homalg_variable_239*homalg_variable_6;
IsZeroMatrix(homalg_variable_240);
matrix homalg_variable_241 = submat(homalg_variable_5,intvec( 118 ),1..8);
matrix homalg_variable_242 = homalg_variable_241*homalg_variable_6;
IsZeroMatrix(homalg_variable_242);
matrix homalg_variable_243 = submat(homalg_variable_5,intvec( 119 ),1..8);
matrix homalg_variable_244 = homalg_variable_243*homalg_variable_6;
IsZeroMatrix(homalg_variable_244);
matrix homalg_variable_245 = submat(homalg_variable_5,intvec( 120 ),1..8);
matrix homalg_variable_246 = homalg_variable_245*homalg_variable_6;
IsZeroMatrix(homalg_variable_246);
matrix homalg_variable_247 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_7,homalg_variable_8);
IsZeroMatrix(homalg_variable_247);
ncols(homalg_variable_247);
matrix homalg_variable_248 = SyzygiesGeneratorsOfRows(homalg_variable_247);
IsZeroMatrix(homalg_variable_248);
ZeroRows(homalg_variable_247);
IsIdentityMatrix(homalg_variable_247);
NonTrivialDegreePerRow( homalg_variable_247 );
print(transpose(homalg_variable_247));
matrix homalg_variable_250 = homalg_variable_9*homalg_variable_247;
matrix homalg_variable_249 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_250,homalg_variable_10);
IsZeroMatrix(homalg_variable_249);
ncols(homalg_variable_249);
matrix homalg_variable_251 = SyzygiesGeneratorsOfRows(homalg_variable_249);
IsZeroMatrix(homalg_variable_251);
ZeroRows(homalg_variable_249);
IsIdentityMatrix(homalg_variable_249);
NonTrivialDegreePerRowWithColPosition( homalg_variable_249 );
print(transpose(homalg_variable_249));
tst_ignore(string(memory(0)));
matrix homalg_variable_253 = homalg_variable_11*homalg_variable_247;
matrix homalg_variable_254 = homalg_variable_253*homalg_variable_249;
matrix homalg_variable_252 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_254,homalg_variable_12);
IsZeroMatrix(homalg_variable_252);
ncols(homalg_variable_252);
matrix homalg_variable_255 = SyzygiesGeneratorsOfRows(homalg_variable_252);
IsZeroMatrix(homalg_variable_255);
ZeroRows(homalg_variable_252);
IsIdentityMatrix(homalg_variable_252);
NonTrivialDegreePerRowWithColPosition( homalg_variable_252 );
print(transpose(homalg_variable_252));
tst_ignore(string(memory(0)));
matrix homalg_variable_257 = homalg_variable_247*homalg_variable_249;
matrix homalg_variable_258 = homalg_variable_13*homalg_variable_257;
matrix homalg_variable_259 = homalg_variable_258*homalg_variable_252;
matrix homalg_variable_256 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_259,homalg_variable_14);
IsZeroMatrix(homalg_variable_256);
ncols(homalg_variable_256);
matrix homalg_variable_260 = SyzygiesGeneratorsOfRows(homalg_variable_256);
IsZeroMatrix(homalg_variable_260);
ZeroRows(homalg_variable_256);
IsIdentityMatrix(homalg_variable_256);
NonTrivialDegreePerRowWithColPosition( homalg_variable_256 );
print(transpose(homalg_variable_256));
tst_ignore(string(memory(0)));
matrix homalg_variable_262 = homalg_variable_257*homalg_variable_252;
matrix homalg_variable_263 = homalg_variable_15*homalg_variable_262;
matrix homalg_variable_264 = homalg_variable_263*homalg_variable_256;
matrix homalg_variable_261 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_264,homalg_variable_16);
IsZeroMatrix(homalg_variable_261);
ncols(homalg_variable_261);
matrix homalg_variable_265 = SyzygiesGeneratorsOfRows(homalg_variable_261);
IsZeroMatrix(homalg_variable_265);
ZeroRows(homalg_variable_261);
IsIdentityMatrix(homalg_variable_261);
NonTrivialDegreePerRowWithColPosition( homalg_variable_261 );
print(transpose(homalg_variable_261));
tst_ignore(string(memory(0)));
matrix homalg_variable_267 = homalg_variable_262*homalg_variable_256;
matrix homalg_variable_268 = homalg_variable_17*homalg_variable_267;
matrix homalg_variable_269 = homalg_variable_268*homalg_variable_261;
matrix homalg_variable_266 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_269,homalg_variable_18);
IsZeroMatrix(homalg_variable_266);
ncols(homalg_variable_266);
matrix homalg_variable_270 = SyzygiesGeneratorsOfRows(homalg_variable_266);
IsZeroMatrix(homalg_variable_270);
ZeroRows(homalg_variable_266);
IsIdentityMatrix(homalg_variable_266);
NonTrivialDegreePerRowWithColPosition( homalg_variable_266 );
print(transpose(homalg_variable_266));
tst_ignore(string(memory(0)));
matrix homalg_variable_272 = homalg_variable_267*homalg_variable_261;
matrix homalg_variable_273 = homalg_variable_19*homalg_variable_272;
matrix homalg_variable_274 = homalg_variable_273*homalg_variable_266;
matrix homalg_variable_271 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_274,homalg_variable_20);
IsZeroMatrix(homalg_variable_271);
ncols(homalg_variable_271);
matrix homalg_variable_275 = SyzygiesGeneratorsOfRows(homalg_variable_271);
IsZeroMatrix(homalg_variable_275);
ZeroRows(homalg_variable_271);
IsIdentityMatrix(homalg_variable_271);
NonTrivialDegreePerRowWithColPosition( homalg_variable_271 );
print(transpose(homalg_variable_271));
tst_ignore(string(memory(0)));
kill homalg_variable_248;kill homalg_variable_250;kill homalg_variable_251;kill homalg_variable_253;kill homalg_variable_254;kill homalg_variable_255;kill homalg_variable_258;kill homalg_variable_259;kill homalg_variable_260;kill homalg_variable_263;kill homalg_variable_264;kill homalg_variable_265;kill homalg_variable_268;kill homalg_variable_269;kill homalg_variable_270;kill homalg_variable_275;
matrix homalg_variable_277 = homalg_variable_272*homalg_variable_266;
matrix homalg_variable_278 = homalg_variable_21*homalg_variable_277;
matrix homalg_variable_279 = homalg_variable_278*homalg_variable_271;
matrix homalg_variable_276 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_279,homalg_variable_22);
IsZeroMatrix(homalg_variable_276);
ncols(homalg_variable_276);
matrix homalg_variable_280 = SyzygiesGeneratorsOfRows(homalg_variable_276);
IsZeroMatrix(homalg_variable_280);
ZeroRows(homalg_variable_276);
IsIdentityMatrix(homalg_variable_276);
NonTrivialDegreePerRowWithColPosition( homalg_variable_276 );
print(transpose(homalg_variable_276));
tst_ignore(string(memory(0)));
matrix homalg_variable_282 = homalg_variable_277*homalg_variable_271;
matrix homalg_variable_283 = homalg_variable_23*homalg_variable_282;
matrix homalg_variable_284 = homalg_variable_283*homalg_variable_276;
matrix homalg_variable_281 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_284,homalg_variable_24);
IsZeroMatrix(homalg_variable_281);
ncols(homalg_variable_281);
matrix homalg_variable_285 = SyzygiesGeneratorsOfRows(homalg_variable_281);
IsZeroMatrix(homalg_variable_285);
ZeroRows(homalg_variable_281);
IsIdentityMatrix(homalg_variable_281);
NonTrivialDegreePerRow( homalg_variable_281 );
print(transpose(homalg_variable_281));
tst_ignore(string(memory(0)));
matrix homalg_variable_287 = homalg_variable_282*homalg_variable_276;
matrix homalg_variable_288 = homalg_variable_25*homalg_variable_287;
matrix homalg_variable_289 = homalg_variable_288*homalg_variable_281;
matrix homalg_variable_286 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_289,homalg_variable_26);
IsZeroMatrix(homalg_variable_286);
ncols(homalg_variable_286);
matrix homalg_variable_290 = SyzygiesGeneratorsOfRows(homalg_variable_286);
IsZeroMatrix(homalg_variable_290);
ZeroRows(homalg_variable_286);
IsIdentityMatrix(homalg_variable_286);
NonTrivialDegreePerRowWithColPosition( homalg_variable_286 );
print(transpose(homalg_variable_286));
tst_ignore(string(memory(0)));
matrix homalg_variable_292 = homalg_variable_287*homalg_variable_281;
matrix homalg_variable_293 = homalg_variable_27*homalg_variable_292;
matrix homalg_variable_294 = homalg_variable_293*homalg_variable_286;
matrix homalg_variable_291 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_294,homalg_variable_28);
IsZeroMatrix(homalg_variable_291);
ncols(homalg_variable_291);
matrix homalg_variable_295 = SyzygiesGeneratorsOfRows(homalg_variable_291);
IsZeroMatrix(homalg_variable_295);
ZeroRows(homalg_variable_291);
IsIdentityMatrix(homalg_variable_291);
NonTrivialDegreePerRowWithColPosition( homalg_variable_291 );
print(transpose(homalg_variable_291));
tst_ignore(string(memory(0)));
matrix homalg_variable_297 = homalg_variable_292*homalg_variable_286;
matrix homalg_variable_298 = homalg_variable_29*homalg_variable_297;
matrix homalg_variable_299 = homalg_variable_298*homalg_variable_291;
matrix homalg_variable_296 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_299,homalg_variable_30);
IsZeroMatrix(homalg_variable_296);
ncols(homalg_variable_296);
matrix homalg_variable_300 = SyzygiesGeneratorsOfRows(homalg_variable_296);
IsZeroMatrix(homalg_variable_300);
ZeroRows(homalg_variable_296);
IsIdentityMatrix(homalg_variable_296);
NonTrivialDegreePerRowWithColPosition( homalg_variable_296 );
print(transpose(homalg_variable_296));
tst_ignore(string(memory(0)));
matrix homalg_variable_302 = homalg_variable_297*homalg_variable_291;
matrix homalg_variable_303 = homalg_variable_31*homalg_variable_302;
matrix homalg_variable_304 = homalg_variable_303*homalg_variable_296;
matrix homalg_variable_301 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_304,homalg_variable_32);
IsZeroMatrix(homalg_variable_301);
ncols(homalg_variable_301);
matrix homalg_variable_305 = SyzygiesGeneratorsOfRows(homalg_variable_301);
IsZeroMatrix(homalg_variable_305);
ZeroRows(homalg_variable_301);
IsIdentityMatrix(homalg_variable_301);
NonTrivialDegreePerRowWithColPosition( homalg_variable_301 );
print(transpose(homalg_variable_301));
tst_ignore(string(memory(0)));
matrix homalg_variable_307 = homalg_variable_302*homalg_variable_296;
matrix homalg_variable_308 = homalg_variable_33*homalg_variable_307;
matrix homalg_variable_309 = homalg_variable_308*homalg_variable_301;
matrix homalg_variable_306 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_309,homalg_variable_34);
IsZeroMatrix(homalg_variable_306);
ncols(homalg_variable_306);
matrix homalg_variable_310 = SyzygiesGeneratorsOfRows(homalg_variable_306);
IsZeroMatrix(homalg_variable_310);
ZeroRows(homalg_variable_306);
IsIdentityMatrix(homalg_variable_306);
NonTrivialDegreePerRowWithColPosition( homalg_variable_306 );
print(transpose(homalg_variable_306));
tst_ignore(string(memory(0)));
kill homalg_variable_278;kill homalg_variable_279;kill homalg_variable_280;kill homalg_variable_283;kill homalg_variable_284;kill homalg_variable_285;kill homalg_variable_288;kill homalg_variable_289;kill homalg_variable_290;kill homalg_variable_293;kill homalg_variable_294;kill homalg_variable_295;kill homalg_variable_298;kill homalg_variable_299;kill homalg_variable_300;kill homalg_variable_304;kill homalg_variable_305;kill homalg_variable_310;
matrix homalg_variable_312 = homalg_variable_307*homalg_variable_301;
matrix homalg_variable_313 = homalg_variable_35*homalg_variable_312;
matrix homalg_variable_314 = homalg_variable_313*homalg_variable_306;
matrix homalg_variable_311 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_314,homalg_variable_36);
IsZeroMatrix(homalg_variable_311);
ncols(homalg_variable_311);
matrix homalg_variable_315 = SyzygiesGeneratorsOfRows(homalg_variable_311);
IsZeroMatrix(homalg_variable_315);
ZeroRows(homalg_variable_311);
IsIdentityMatrix(homalg_variable_311);
NonTrivialDegreePerRowWithColPosition( homalg_variable_311 );
print(transpose(homalg_variable_311));
tst_ignore(string(memory(0)));
matrix homalg_variable_317 = homalg_variable_312*homalg_variable_306;
matrix homalg_variable_318 = homalg_variable_37*homalg_variable_317;
matrix homalg_variable_319 = homalg_variable_318*homalg_variable_311;
matrix homalg_variable_316 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_319,homalg_variable_38);
IsZeroMatrix(homalg_variable_316);
ncols(homalg_variable_316);
matrix homalg_variable_320 = SyzygiesGeneratorsOfRows(homalg_variable_316);
IsZeroMatrix(homalg_variable_320);
ZeroRows(homalg_variable_316);
IsIdentityMatrix(homalg_variable_316);
NonTrivialDegreePerRowWithColPosition( homalg_variable_316 );
print(transpose(homalg_variable_316));
tst_ignore(string(memory(0)));
matrix homalg_variable_322 = homalg_variable_317*homalg_variable_311;
matrix homalg_variable_323 = homalg_variable_39*homalg_variable_322;
matrix homalg_variable_324 = homalg_variable_323*homalg_variable_316;
matrix homalg_variable_321 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_324,homalg_variable_40);
IsZeroMatrix(homalg_variable_321);
ncols(homalg_variable_321);
matrix homalg_variable_325 = SyzygiesGeneratorsOfRows(homalg_variable_321);
IsZeroMatrix(homalg_variable_325);
ZeroRows(homalg_variable_321);
IsIdentityMatrix(homalg_variable_321);
NonTrivialDegreePerRowWithColPosition( homalg_variable_321 );
print(transpose(homalg_variable_321));
tst_ignore(string(memory(0)));
matrix homalg_variable_327 = homalg_variable_322*homalg_variable_316;
matrix homalg_variable_328 = homalg_variable_41*homalg_variable_327;
matrix homalg_variable_329 = homalg_variable_328*homalg_variable_321;
matrix homalg_variable_326 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_329,homalg_variable_42);
IsZeroMatrix(homalg_variable_326);
ncols(homalg_variable_326);
matrix homalg_variable_330 = SyzygiesGeneratorsOfRows(homalg_variable_326);
IsZeroMatrix(homalg_variable_330);
ncols(homalg_variable_330);
GetColumnIndependentUnitPositions(homalg_variable_330, list ( 0 ));
matrix homalg_variable_332 = submat(homalg_variable_326,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_331 = SyzygiesGeneratorsOfRows(homalg_variable_332);
IsZeroMatrix(homalg_variable_331);
ZeroRows(homalg_variable_332);
IsIdentityMatrix(homalg_variable_332);
NonTrivialDegreePerRowWithColPosition( homalg_variable_332 );
print(transpose(homalg_variable_332));
tst_ignore(string(memory(0)));
matrix homalg_variable_334 = homalg_variable_327*homalg_variable_321;
matrix homalg_variable_335 = homalg_variable_43*homalg_variable_334;
matrix homalg_variable_336 = homalg_variable_335*homalg_variable_332;
matrix homalg_variable_333 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_336,homalg_variable_44);
IsZeroMatrix(homalg_variable_333);
ncols(homalg_variable_333);
matrix homalg_variable_337 = SyzygiesGeneratorsOfRows(homalg_variable_333);
IsZeroMatrix(homalg_variable_337);
ncols(homalg_variable_337);
GetColumnIndependentUnitPositions(homalg_variable_337, list ( 0 ));
matrix homalg_variable_339 = submat(homalg_variable_333,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 10 ));
matrix homalg_variable_338 = SyzygiesGeneratorsOfRows(homalg_variable_339);
IsZeroMatrix(homalg_variable_338);
ZeroRows(homalg_variable_339);
IsIdentityMatrix(homalg_variable_339);
NonTrivialDegreePerRowWithColPosition( homalg_variable_339 );
print(transpose(homalg_variable_339));
tst_ignore(string(memory(0)));
matrix homalg_variable_341 = homalg_variable_334*homalg_variable_332;
matrix homalg_variable_342 = homalg_variable_45*homalg_variable_341;
matrix homalg_variable_343 = homalg_variable_342*homalg_variable_339;
matrix homalg_variable_340 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_343,homalg_variable_46);
IsZeroMatrix(homalg_variable_340);
ncols(homalg_variable_340);
matrix homalg_variable_344 = SyzygiesGeneratorsOfRows(homalg_variable_340);
IsZeroMatrix(homalg_variable_344);
ncols(homalg_variable_344);
GetColumnIndependentUnitPositions(homalg_variable_344, list ( 0 ));
matrix homalg_variable_346 = submat(homalg_variable_340,1..8,intvec( 1, 2, 3, 4, 5, 6, 10, 12 ));
matrix homalg_variable_345 = SyzygiesGeneratorsOfRows(homalg_variable_346);
IsZeroMatrix(homalg_variable_345);
ZeroRows(homalg_variable_346);
IsIdentityMatrix(homalg_variable_346);
NonTrivialDegreePerRowWithColPosition( homalg_variable_346 );
print(transpose(homalg_variable_346));
tst_ignore(string(memory(0)));
kill homalg_variable_314;kill homalg_variable_315;kill homalg_variable_318;kill homalg_variable_319;kill homalg_variable_320;kill homalg_variable_323;kill homalg_variable_324;kill homalg_variable_325;kill homalg_variable_328;kill homalg_variable_329;kill homalg_variable_331;kill homalg_variable_335;kill homalg_variable_336;kill homalg_variable_338;kill homalg_variable_345;
matrix homalg_variable_348 = homalg_variable_341*homalg_variable_339;
matrix homalg_variable_349 = homalg_variable_47*homalg_variable_348;
matrix homalg_variable_350 = homalg_variable_349*homalg_variable_346;
matrix homalg_variable_347 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_350,homalg_variable_48);
IsZeroMatrix(homalg_variable_347);
ncols(homalg_variable_347);
matrix homalg_variable_351 = SyzygiesGeneratorsOfRows(homalg_variable_347);
IsZeroMatrix(homalg_variable_351);
ncols(homalg_variable_351);
GetColumnIndependentUnitPositions(homalg_variable_351, list ( 0 ));
matrix homalg_variable_353 = submat(homalg_variable_347,1..8,intvec( 1, 2, 3, 4, 5, 10, 12, 15 ));
matrix homalg_variable_352 = SyzygiesGeneratorsOfRows(homalg_variable_353);
IsZeroMatrix(homalg_variable_352);
ZeroRows(homalg_variable_353);
IsIdentityMatrix(homalg_variable_353);
NonTrivialDegreePerRowWithColPosition( homalg_variable_353 );
print(transpose(homalg_variable_353));
tst_ignore(string(memory(0)));
matrix homalg_variable_355 = homalg_variable_348*homalg_variable_346;
matrix homalg_variable_356 = homalg_variable_49*homalg_variable_355;
matrix homalg_variable_357 = homalg_variable_356*homalg_variable_353;
matrix homalg_variable_354 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_357,homalg_variable_50);
IsZeroMatrix(homalg_variable_354);
ncols(homalg_variable_354);
matrix homalg_variable_358 = SyzygiesGeneratorsOfRows(homalg_variable_354);
IsZeroMatrix(homalg_variable_358);
ncols(homalg_variable_358);
GetColumnIndependentUnitPositions(homalg_variable_358, list ( 0 ));
matrix homalg_variable_360 = submat(homalg_variable_354,1..8,intvec( 1, 2, 3, 4, 10, 12, 15, 19 ));
matrix homalg_variable_359 = SyzygiesGeneratorsOfRows(homalg_variable_360);
IsZeroMatrix(homalg_variable_359);
ZeroRows(homalg_variable_360);
IsIdentityMatrix(homalg_variable_360);
NonTrivialDegreePerRowWithColPosition( homalg_variable_360 );
print(transpose(homalg_variable_360));
tst_ignore(string(memory(0)));
matrix homalg_variable_362 = homalg_variable_355*homalg_variable_353;
matrix homalg_variable_363 = homalg_variable_51*homalg_variable_362;
matrix homalg_variable_364 = homalg_variable_363*homalg_variable_360;
matrix homalg_variable_361 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_364,homalg_variable_52);
IsZeroMatrix(homalg_variable_361);
ncols(homalg_variable_361);
matrix homalg_variable_365 = SyzygiesGeneratorsOfRows(homalg_variable_361);
IsZeroMatrix(homalg_variable_365);
ZeroRows(homalg_variable_361);
IsIdentityMatrix(homalg_variable_361);
NonTrivialDegreePerRowWithColPosition( homalg_variable_361 );
print(transpose(homalg_variable_361));
tst_ignore(string(memory(0)));
matrix homalg_variable_367 = homalg_variable_362*homalg_variable_360;
matrix homalg_variable_368 = homalg_variable_53*homalg_variable_367;
matrix homalg_variable_369 = homalg_variable_368*homalg_variable_361;
matrix homalg_variable_366 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_369,homalg_variable_54);
IsZeroMatrix(homalg_variable_366);
ncols(homalg_variable_366);
matrix homalg_variable_370 = SyzygiesGeneratorsOfRows(homalg_variable_366);
IsZeroMatrix(homalg_variable_370);
ncols(homalg_variable_370);
GetColumnIndependentUnitPositions(homalg_variable_370, list ( 0 ));
matrix homalg_variable_372 = submat(homalg_variable_366,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_371 = SyzygiesGeneratorsOfRows(homalg_variable_372);
IsZeroMatrix(homalg_variable_371);
ZeroRows(homalg_variable_372);
IsIdentityMatrix(homalg_variable_372);
NonTrivialDegreePerRowWithColPosition( homalg_variable_372 );
print(transpose(homalg_variable_372));
tst_ignore(string(memory(0)));
matrix homalg_variable_374 = homalg_variable_367*homalg_variable_361;
matrix homalg_variable_375 = homalg_variable_55*homalg_variable_374;
matrix homalg_variable_376 = homalg_variable_375*homalg_variable_372;
matrix homalg_variable_373 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_376,homalg_variable_56);
IsZeroMatrix(homalg_variable_373);
ncols(homalg_variable_373);
matrix homalg_variable_377 = SyzygiesGeneratorsOfRows(homalg_variable_373);
IsZeroMatrix(homalg_variable_377);
ncols(homalg_variable_377);
GetColumnIndependentUnitPositions(homalg_variable_377, list ( 0 ));
matrix homalg_variable_379 = submat(homalg_variable_373,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 10 ));
matrix homalg_variable_378 = SyzygiesGeneratorsOfRows(homalg_variable_379);
IsZeroMatrix(homalg_variable_378);
ZeroRows(homalg_variable_379);
IsIdentityMatrix(homalg_variable_379);
NonTrivialDegreePerRowWithColPosition( homalg_variable_379 );
print(transpose(homalg_variable_379));
tst_ignore(string(memory(0)));
kill homalg_variable_350;kill homalg_variable_352;kill homalg_variable_357;kill homalg_variable_359;kill homalg_variable_363;kill homalg_variable_364;kill homalg_variable_365;kill homalg_variable_368;kill homalg_variable_369;kill homalg_variable_371;kill homalg_variable_378;
matrix homalg_variable_381 = homalg_variable_374*homalg_variable_372;
matrix homalg_variable_382 = homalg_variable_57*homalg_variable_381;
matrix homalg_variable_383 = homalg_variable_382*homalg_variable_379;
matrix homalg_variable_380 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_383,homalg_variable_58);
IsZeroMatrix(homalg_variable_380);
ncols(homalg_variable_380);
matrix homalg_variable_384 = SyzygiesGeneratorsOfRows(homalg_variable_380);
IsZeroMatrix(homalg_variable_384);
ncols(homalg_variable_384);
GetColumnIndependentUnitPositions(homalg_variable_384, list ( 0 ));
matrix homalg_variable_386 = submat(homalg_variable_380,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_385 = SyzygiesGeneratorsOfRows(homalg_variable_386);
IsZeroMatrix(homalg_variable_385);
ZeroRows(homalg_variable_386);
IsIdentityMatrix(homalg_variable_386);
NonTrivialDegreePerRowWithColPosition( homalg_variable_386 );
print(transpose(homalg_variable_386));
tst_ignore(string(memory(0)));
matrix homalg_variable_388 = homalg_variable_381*homalg_variable_379;
matrix homalg_variable_389 = homalg_variable_59*homalg_variable_388;
matrix homalg_variable_390 = homalg_variable_389*homalg_variable_386;
matrix homalg_variable_387 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_390,homalg_variable_60);
IsZeroMatrix(homalg_variable_387);
ncols(homalg_variable_387);
matrix homalg_variable_391 = SyzygiesGeneratorsOfRows(homalg_variable_387);
IsZeroMatrix(homalg_variable_391);
ncols(homalg_variable_391);
GetColumnIndependentUnitPositions(homalg_variable_391, list ( 0 ));
matrix homalg_variable_393 = submat(homalg_variable_387,1..8,intvec( 1, 2, 3, 4, 5, 6, 10, 12 ));
matrix homalg_variable_392 = SyzygiesGeneratorsOfRows(homalg_variable_393);
IsZeroMatrix(homalg_variable_392);
ZeroRows(homalg_variable_393);
IsIdentityMatrix(homalg_variable_393);
NonTrivialDegreePerRowWithColPosition( homalg_variable_393 );
print(transpose(homalg_variable_393));
tst_ignore(string(memory(0)));
matrix homalg_variable_395 = homalg_variable_388*homalg_variable_386;
matrix homalg_variable_396 = homalg_variable_61*homalg_variable_395;
matrix homalg_variable_397 = homalg_variable_396*homalg_variable_393;
matrix homalg_variable_394 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_397,homalg_variable_62);
IsZeroMatrix(homalg_variable_394);
ncols(homalg_variable_394);
matrix homalg_variable_398 = SyzygiesGeneratorsOfRows(homalg_variable_394);
IsZeroMatrix(homalg_variable_398);
ncols(homalg_variable_398);
GetColumnIndependentUnitPositions(homalg_variable_398, list ( 0 ));
matrix homalg_variable_400 = submat(homalg_variable_394,1..8,intvec( 1, 2, 3, 4, 6, 7, 8, 9, 12, 13, 15, 16, 17, 19, 20, 21, 22 ));
matrix homalg_variable_399 = SyzygiesGeneratorsOfRows(homalg_variable_400);
IsZeroMatrix(homalg_variable_399);
ncols(homalg_variable_399);
GetColumnIndependentUnitPositions(homalg_variable_399, list ( 0 ));
matrix homalg_variable_402 = submat(homalg_variable_400,1..8,intvec( 1, 2, 3, 4, 5, 6, 9, 11, 14, 15 ));
matrix homalg_variable_401 = SyzygiesGeneratorsOfRows(homalg_variable_402);
IsZeroMatrix(homalg_variable_401);
ncols(homalg_variable_401);
GetColumnIndependentUnitPositions(homalg_variable_401, list ( 0 ));
matrix homalg_variable_404 = submat(homalg_variable_402,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 9 ));
matrix homalg_variable_403 = SyzygiesGeneratorsOfRows(homalg_variable_404);
IsZeroMatrix(homalg_variable_403);
ZeroRows(homalg_variable_404);
IsIdentityMatrix(homalg_variable_404);
NonTrivialDegreePerRowWithColPosition( homalg_variable_404 );
print(transpose(homalg_variable_404));
tst_ignore(string(memory(0)));
matrix homalg_variable_406 = homalg_variable_395*homalg_variable_393;
matrix homalg_variable_407 = homalg_variable_63*homalg_variable_406;
matrix homalg_variable_408 = homalg_variable_407*homalg_variable_404;
matrix homalg_variable_405 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_408,homalg_variable_64);
IsZeroMatrix(homalg_variable_405);
ncols(homalg_variable_405);
matrix homalg_variable_409 = SyzygiesGeneratorsOfRows(homalg_variable_405);
IsZeroMatrix(homalg_variable_409);
ncols(homalg_variable_409);
GetColumnIndependentUnitPositions(homalg_variable_409, list ( 0 ));
matrix homalg_variable_411 = submat(homalg_variable_405,1..8,intvec( 1, 2, 3, 5, 6, 7, 8, 9, 12, 13, 15, 16, 17, 19, 20, 21, 22, 24, 25, 26, 27, 28 ));
matrix homalg_variable_410 = SyzygiesGeneratorsOfRows(homalg_variable_411);
IsZeroMatrix(homalg_variable_410);
ncols(homalg_variable_410);
GetColumnIndependentUnitPositions(homalg_variable_410, list ( 0 ));
matrix homalg_variable_413 = submat(homalg_variable_411,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 11, 12, 14, 15, 16, 18, 19, 20, 21 ));
matrix homalg_variable_412 = SyzygiesGeneratorsOfRows(homalg_variable_413);
IsZeroMatrix(homalg_variable_412);
ncols(homalg_variable_412);
GetColumnIndependentUnitPositions(homalg_variable_412, list ( 0 ));
matrix homalg_variable_415 = submat(homalg_variable_413,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 11, 12, 14, 15, 16 ));
matrix homalg_variable_414 = SyzygiesGeneratorsOfRows(homalg_variable_415);
IsZeroMatrix(homalg_variable_414);
ncols(homalg_variable_414);
GetColumnIndependentUnitPositions(homalg_variable_414, list ( 0 ));
matrix homalg_variable_417 = submat(homalg_variable_415,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 9, 11, 12 ));
matrix homalg_variable_416 = SyzygiesGeneratorsOfRows(homalg_variable_417);
IsZeroMatrix(homalg_variable_416);
ncols(homalg_variable_416);
GetColumnIndependentUnitPositions(homalg_variable_416, list ( 0 ));
matrix homalg_variable_419 = submat(homalg_variable_417,1..8,intvec( 1, 2, 3, 4, 6, 7, 8, 9 ));
matrix homalg_variable_418 = SyzygiesGeneratorsOfRows(homalg_variable_419);
IsZeroMatrix(homalg_variable_418);
ZeroRows(homalg_variable_419);
IsIdentityMatrix(homalg_variable_419);
NonTrivialDegreePerRowWithColPosition( homalg_variable_419 );
print(transpose(homalg_variable_419));
tst_ignore(string(memory(0)));
matrix homalg_variable_421 = homalg_variable_406*homalg_variable_404;
matrix homalg_variable_422 = homalg_variable_65*homalg_variable_421;
kill homalg_variable_385;kill homalg_variable_389;kill homalg_variable_390;kill homalg_variable_392;kill homalg_variable_396;kill homalg_variable_397;kill homalg_variable_403;kill homalg_variable_407;kill homalg_variable_408;kill homalg_variable_418;
matrix homalg_variable_423 = homalg_variable_422*homalg_variable_419;
matrix homalg_variable_420 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_423,homalg_variable_66);
IsZeroMatrix(homalg_variable_420);
ncols(homalg_variable_420);
matrix homalg_variable_424 = SyzygiesGeneratorsOfRows(homalg_variable_420);
IsZeroMatrix(homalg_variable_424);
ZeroRows(homalg_variable_420);
IsIdentityMatrix(homalg_variable_420);
NonTrivialDegreePerRowWithColPosition( homalg_variable_420 );
print(transpose(homalg_variable_420));
tst_ignore(string(memory(0)));
matrix homalg_variable_426 = homalg_variable_421*homalg_variable_419;
matrix homalg_variable_427 = homalg_variable_67*homalg_variable_426;
matrix homalg_variable_428 = homalg_variable_427*homalg_variable_420;
matrix homalg_variable_425 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_428,homalg_variable_68);
IsZeroMatrix(homalg_variable_425);
ncols(homalg_variable_425);
matrix homalg_variable_429 = SyzygiesGeneratorsOfRows(homalg_variable_425);
IsZeroMatrix(homalg_variable_429);
ncols(homalg_variable_429);
GetColumnIndependentUnitPositions(homalg_variable_429, list ( 0 ));
matrix homalg_variable_431 = submat(homalg_variable_425,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_430 = SyzygiesGeneratorsOfRows(homalg_variable_431);
IsZeroMatrix(homalg_variable_430);
ZeroRows(homalg_variable_431);
IsIdentityMatrix(homalg_variable_431);
NonTrivialDegreePerRowWithColPosition( homalg_variable_431 );
print(transpose(homalg_variable_431));
tst_ignore(string(memory(0)));
matrix homalg_variable_433 = homalg_variable_426*homalg_variable_420;
matrix homalg_variable_434 = homalg_variable_69*homalg_variable_433;
matrix homalg_variable_435 = homalg_variable_434*homalg_variable_431;
matrix homalg_variable_432 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_435,homalg_variable_70);
IsZeroMatrix(homalg_variable_432);
ncols(homalg_variable_432);
matrix homalg_variable_436 = SyzygiesGeneratorsOfRows(homalg_variable_432);
IsZeroMatrix(homalg_variable_436);
ncols(homalg_variable_436);
GetColumnIndependentUnitPositions(homalg_variable_436, list ( 0 ));
matrix homalg_variable_438 = submat(homalg_variable_432,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_437 = SyzygiesGeneratorsOfRows(homalg_variable_438);
IsZeroMatrix(homalg_variable_437);
ZeroRows(homalg_variable_438);
IsIdentityMatrix(homalg_variable_438);
NonTrivialDegreePerRowWithColPosition( homalg_variable_438 );
print(transpose(homalg_variable_438));
tst_ignore(string(memory(0)));
matrix homalg_variable_440 = homalg_variable_433*homalg_variable_431;
matrix homalg_variable_441 = homalg_variable_71*homalg_variable_440;
matrix homalg_variable_442 = homalg_variable_441*homalg_variable_438;
matrix homalg_variable_439 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_442,homalg_variable_72);
IsZeroMatrix(homalg_variable_439);
ncols(homalg_variable_439);
matrix homalg_variable_443 = SyzygiesGeneratorsOfRows(homalg_variable_439);
IsZeroMatrix(homalg_variable_443);
ncols(homalg_variable_443);
GetColumnIndependentUnitPositions(homalg_variable_443, list ( 0 ));
matrix homalg_variable_445 = submat(homalg_variable_439,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 12 ));
matrix homalg_variable_444 = SyzygiesGeneratorsOfRows(homalg_variable_445);
IsZeroMatrix(homalg_variable_444);
ncols(homalg_variable_444);
GetColumnIndependentUnitPositions(homalg_variable_444, list ( 0 ));
matrix homalg_variable_447 = submat(homalg_variable_445,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_446 = SyzygiesGeneratorsOfRows(homalg_variable_447);
IsZeroMatrix(homalg_variable_446);
ZeroRows(homalg_variable_447);
IsIdentityMatrix(homalg_variable_447);
NonTrivialDegreePerRowWithColPosition( homalg_variable_447 );
print(transpose(homalg_variable_447));
tst_ignore(string(memory(0)));
matrix homalg_variable_449 = homalg_variable_440*homalg_variable_438;
matrix homalg_variable_450 = homalg_variable_73*homalg_variable_449;
matrix homalg_variable_451 = homalg_variable_450*homalg_variable_447;
matrix homalg_variable_448 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_451,homalg_variable_74);
IsZeroMatrix(homalg_variable_448);
ncols(homalg_variable_448);
matrix homalg_variable_452 = SyzygiesGeneratorsOfRows(homalg_variable_448);
IsZeroMatrix(homalg_variable_452);
ncols(homalg_variable_452);
GetColumnIndependentUnitPositions(homalg_variable_452, list ( 0 ));
matrix homalg_variable_454 = submat(homalg_variable_448,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 15, 16, 17 ));
matrix homalg_variable_453 = SyzygiesGeneratorsOfRows(homalg_variable_454);
IsZeroMatrix(homalg_variable_453);
ncols(homalg_variable_453);
GetColumnIndependentUnitPositions(homalg_variable_453, list ( 0 ));
matrix homalg_variable_456 = submat(homalg_variable_454,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 13 ));
matrix homalg_variable_455 = SyzygiesGeneratorsOfRows(homalg_variable_456);
IsZeroMatrix(homalg_variable_455);
ncols(homalg_variable_455);
GetColumnIndependentUnitPositions(homalg_variable_455, list ( 0 ));
matrix homalg_variable_458 = submat(homalg_variable_456,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 10 ));
matrix homalg_variable_457 = SyzygiesGeneratorsOfRows(homalg_variable_458);
IsZeroMatrix(homalg_variable_457);
ZeroRows(homalg_variable_458);
IsIdentityMatrix(homalg_variable_458);
NonTrivialDegreePerRowWithColPosition( homalg_variable_458 );
print(transpose(homalg_variable_458));
tst_ignore(string(memory(0)));
kill homalg_variable_424;kill homalg_variable_427;kill homalg_variable_428;kill homalg_variable_430;kill homalg_variable_434;kill homalg_variable_435;kill homalg_variable_437;kill homalg_variable_441;kill homalg_variable_442;kill homalg_variable_446;
matrix homalg_variable_460 = homalg_variable_449*homalg_variable_447;
matrix homalg_variable_461 = homalg_variable_75*homalg_variable_460;
matrix homalg_variable_462 = homalg_variable_461*homalg_variable_458;
matrix homalg_variable_459 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_462,homalg_variable_76);
IsZeroMatrix(homalg_variable_459);
ncols(homalg_variable_459);
matrix homalg_variable_463 = SyzygiesGeneratorsOfRows(homalg_variable_459);
IsZeroMatrix(homalg_variable_463);
ncols(homalg_variable_463);
GetColumnIndependentUnitPositions(homalg_variable_463, list ( 0 ));
matrix homalg_variable_465 = submat(homalg_variable_459,1..8,intvec( 1, 2, 3, 4, 6, 7, 9, 10, 11, 12, 15, 16, 17, 19, 20, 21, 22 ));
matrix homalg_variable_464 = SyzygiesGeneratorsOfRows(homalg_variable_465);
IsZeroMatrix(homalg_variable_464);
ncols(homalg_variable_464);
GetColumnIndependentUnitPositions(homalg_variable_464, list ( 0 ));
matrix homalg_variable_467 = submat(homalg_variable_465,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 9, 11, 12, 14, 15, 16 ));
matrix homalg_variable_466 = SyzygiesGeneratorsOfRows(homalg_variable_467);
IsZeroMatrix(homalg_variable_466);
ncols(homalg_variable_466);
GetColumnIndependentUnitPositions(homalg_variable_466, list ( 0 ));
matrix homalg_variable_469 = submat(homalg_variable_467,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 11, 12 ));
matrix homalg_variable_468 = SyzygiesGeneratorsOfRows(homalg_variable_469);
IsZeroMatrix(homalg_variable_468);
ncols(homalg_variable_468);
GetColumnIndependentUnitPositions(homalg_variable_468, list ( 0 ));
matrix homalg_variable_471 = submat(homalg_variable_469,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9 ));
matrix homalg_variable_470 = SyzygiesGeneratorsOfRows(homalg_variable_471);
IsZeroMatrix(homalg_variable_470);
ZeroRows(homalg_variable_471);
IsIdentityMatrix(homalg_variable_471);
NonTrivialDegreePerRowWithColPosition( homalg_variable_471 );
print(transpose(homalg_variable_471));
tst_ignore(string(memory(0)));
matrix homalg_variable_473 = homalg_variable_460*homalg_variable_458;
matrix homalg_variable_474 = homalg_variable_77*homalg_variable_473;
matrix homalg_variable_475 = homalg_variable_474*homalg_variable_471;
matrix homalg_variable_472 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_475,homalg_variable_78);
IsZeroMatrix(homalg_variable_472);
ncols(homalg_variable_472);
matrix homalg_variable_476 = SyzygiesGeneratorsOfRows(homalg_variable_472);
IsZeroMatrix(homalg_variable_476);
ncols(homalg_variable_476);
GetColumnIndependentUnitPositions(homalg_variable_476, list ( 0 ));
matrix homalg_variable_478 = submat(homalg_variable_472,1..8,intvec( 1, 2, 3, 4, 6, 7, 8, 9, 10, 12, 13, 15, 16, 17, 19, 20, 21, 22 ));
matrix homalg_variable_477 = SyzygiesGeneratorsOfRows(homalg_variable_478);
IsZeroMatrix(homalg_variable_477);
ncols(homalg_variable_477);
GetColumnIndependentUnitPositions(homalg_variable_477, list ( 0 ));
matrix homalg_variable_480 = submat(homalg_variable_478,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 13, 15, 16, 17 ));
matrix homalg_variable_479 = SyzygiesGeneratorsOfRows(homalg_variable_480);
IsZeroMatrix(homalg_variable_479);
ncols(homalg_variable_479);
GetColumnIndependentUnitPositions(homalg_variable_479, list ( 0 ));
matrix homalg_variable_482 = submat(homalg_variable_480,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 10, 12, 13 ));
matrix homalg_variable_481 = SyzygiesGeneratorsOfRows(homalg_variable_482);
IsZeroMatrix(homalg_variable_481);
ncols(homalg_variable_481);
GetColumnIndependentUnitPositions(homalg_variable_481, list ( 0 ));
matrix homalg_variable_484 = submat(homalg_variable_482,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 10 ));
matrix homalg_variable_483 = SyzygiesGeneratorsOfRows(homalg_variable_484);
IsZeroMatrix(homalg_variable_483);
ncols(homalg_variable_483);
GetColumnIndependentUnitPositions(homalg_variable_483, list ( 0 ));
matrix homalg_variable_486 = submat(homalg_variable_484,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 9 ));
matrix homalg_variable_485 = SyzygiesGeneratorsOfRows(homalg_variable_486);
IsZeroMatrix(homalg_variable_485);
ZeroRows(homalg_variable_486);
IsIdentityMatrix(homalg_variable_486);
NonTrivialDegreePerRowWithColPosition( homalg_variable_486 );
print(transpose(homalg_variable_486));
tst_ignore(string(memory(0)));
matrix homalg_variable_488 = homalg_variable_473*homalg_variable_471;
matrix homalg_variable_489 = homalg_variable_79*homalg_variable_488;
matrix homalg_variable_490 = homalg_variable_489*homalg_variable_486;
matrix homalg_variable_487 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_490,homalg_variable_80);
IsZeroMatrix(homalg_variable_487);
ncols(homalg_variable_487);
matrix homalg_variable_491 = SyzygiesGeneratorsOfRows(homalg_variable_487);
IsZeroMatrix(homalg_variable_491);
ZeroRows(homalg_variable_487);
IsIdentityMatrix(homalg_variable_487);
NonTrivialDegreePerRowWithColPosition( homalg_variable_487 );
print(transpose(homalg_variable_487));
tst_ignore(string(memory(0)));
kill homalg_variable_461;kill homalg_variable_462;kill homalg_variable_470;kill homalg_variable_475;kill homalg_variable_485;kill homalg_variable_491;
matrix homalg_variable_493 = homalg_variable_488*homalg_variable_486;
matrix homalg_variable_494 = homalg_variable_81*homalg_variable_493;
matrix homalg_variable_495 = homalg_variable_494*homalg_variable_487;
matrix homalg_variable_492 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_495,homalg_variable_82);
IsZeroMatrix(homalg_variable_492);
ncols(homalg_variable_492);
matrix homalg_variable_496 = SyzygiesGeneratorsOfRows(homalg_variable_492);
IsZeroMatrix(homalg_variable_496);
ncols(homalg_variable_496);
GetColumnIndependentUnitPositions(homalg_variable_496, list ( 0 ));
matrix homalg_variable_498 = submat(homalg_variable_492,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_497 = SyzygiesGeneratorsOfRows(homalg_variable_498);
IsZeroMatrix(homalg_variable_497);
ZeroRows(homalg_variable_498);
IsIdentityMatrix(homalg_variable_498);
NonTrivialDegreePerRowWithColPosition( homalg_variable_498 );
print(transpose(homalg_variable_498));
tst_ignore(string(memory(0)));
matrix homalg_variable_500 = homalg_variable_493*homalg_variable_487;
matrix homalg_variable_501 = homalg_variable_83*homalg_variable_500;
matrix homalg_variable_502 = homalg_variable_501*homalg_variable_498;
matrix homalg_variable_499 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_502,homalg_variable_84);
IsZeroMatrix(homalg_variable_499);
ncols(homalg_variable_499);
matrix homalg_variable_503 = SyzygiesGeneratorsOfRows(homalg_variable_499);
IsZeroMatrix(homalg_variable_503);
ncols(homalg_variable_503);
GetColumnIndependentUnitPositions(homalg_variable_503, list ( 0 ));
matrix homalg_variable_505 = submat(homalg_variable_499,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_504 = SyzygiesGeneratorsOfRows(homalg_variable_505);
IsZeroMatrix(homalg_variable_504);
ZeroRows(homalg_variable_505);
IsIdentityMatrix(homalg_variable_505);
NonTrivialDegreePerRowWithColPosition( homalg_variable_505 );
print(transpose(homalg_variable_505));
tst_ignore(string(memory(0)));
matrix homalg_variable_507 = homalg_variable_500*homalg_variable_498;
matrix homalg_variable_508 = homalg_variable_85*homalg_variable_507;
matrix homalg_variable_509 = homalg_variable_508*homalg_variable_505;
matrix homalg_variable_506 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_509,homalg_variable_86);
IsZeroMatrix(homalg_variable_506);
ncols(homalg_variable_506);
matrix homalg_variable_510 = SyzygiesGeneratorsOfRows(homalg_variable_506);
IsZeroMatrix(homalg_variable_510);
ncols(homalg_variable_510);
GetColumnIndependentUnitPositions(homalg_variable_510, list ( 0 ));
matrix homalg_variable_512 = submat(homalg_variable_506,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 10, 12 ));
matrix homalg_variable_511 = SyzygiesGeneratorsOfRows(homalg_variable_512);
IsZeroMatrix(homalg_variable_511);
ncols(homalg_variable_511);
GetColumnIndependentUnitPositions(homalg_variable_511, list ( 0 ));
matrix homalg_variable_514 = submat(homalg_variable_512,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_513 = SyzygiesGeneratorsOfRows(homalg_variable_514);
IsZeroMatrix(homalg_variable_513);
ZeroRows(homalg_variable_514);
IsIdentityMatrix(homalg_variable_514);
NonTrivialDegreePerRowWithColPosition( homalg_variable_514 );
print(transpose(homalg_variable_514));
tst_ignore(string(memory(0)));
matrix homalg_variable_516 = homalg_variable_507*homalg_variable_505;
matrix homalg_variable_517 = homalg_variable_87*homalg_variable_516;
matrix homalg_variable_518 = homalg_variable_517*homalg_variable_514;
matrix homalg_variable_515 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_518,homalg_variable_88);
IsZeroMatrix(homalg_variable_515);
ncols(homalg_variable_515);
matrix homalg_variable_519 = SyzygiesGeneratorsOfRows(homalg_variable_515);
IsZeroMatrix(homalg_variable_519);
ncols(homalg_variable_519);
GetColumnIndependentUnitPositions(homalg_variable_519, list ( 0 ));
matrix homalg_variable_521 = submat(homalg_variable_515,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 15, 16 ));
matrix homalg_variable_520 = SyzygiesGeneratorsOfRows(homalg_variable_521);
IsZeroMatrix(homalg_variable_520);
ncols(homalg_variable_520);
GetColumnIndependentUnitPositions(homalg_variable_520, list ( 0 ));
matrix homalg_variable_523 = submat(homalg_variable_521,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 11 ));
matrix homalg_variable_522 = SyzygiesGeneratorsOfRows(homalg_variable_523);
IsZeroMatrix(homalg_variable_522);
ZeroRows(homalg_variable_523);
IsIdentityMatrix(homalg_variable_523);
NonTrivialDegreePerRowWithColPosition( homalg_variable_523 );
print(transpose(homalg_variable_523));
tst_ignore(string(memory(0)));
kill homalg_variable_495;kill homalg_variable_497;kill homalg_variable_501;kill homalg_variable_502;kill homalg_variable_504;kill homalg_variable_508;kill homalg_variable_509;kill homalg_variable_513;kill homalg_variable_522;
matrix homalg_variable_525 = homalg_variable_516*homalg_variable_514;
matrix homalg_variable_526 = homalg_variable_89*homalg_variable_525;
matrix homalg_variable_527 = homalg_variable_526*homalg_variable_523;
matrix homalg_variable_524 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_527,homalg_variable_90);
IsZeroMatrix(homalg_variable_524);
ncols(homalg_variable_524);
matrix homalg_variable_528 = SyzygiesGeneratorsOfRows(homalg_variable_524);
IsZeroMatrix(homalg_variable_528);
ncols(homalg_variable_528);
GetColumnIndependentUnitPositions(homalg_variable_528, list ( 0 ));
matrix homalg_variable_530 = submat(homalg_variable_524,1..8,intvec( 1, 2, 3, 4, 6, 9, 10, 13, 14, 15, 16, 19, 20, 21 ));
matrix homalg_variable_529 = SyzygiesGeneratorsOfRows(homalg_variable_530);
IsZeroMatrix(homalg_variable_529);
ncols(homalg_variable_529);
GetColumnIndependentUnitPositions(homalg_variable_529, list ( 0 ));
matrix homalg_variable_532 = submat(homalg_variable_530,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 12, 13 ));
matrix homalg_variable_531 = SyzygiesGeneratorsOfRows(homalg_variable_532);
IsZeroMatrix(homalg_variable_531);
ncols(homalg_variable_531);
GetColumnIndependentUnitPositions(homalg_variable_531, list ( 0 ));
matrix homalg_variable_534 = submat(homalg_variable_532,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_533 = SyzygiesGeneratorsOfRows(homalg_variable_534);
IsZeroMatrix(homalg_variable_533);
ZeroRows(homalg_variable_534);
IsIdentityMatrix(homalg_variable_534);
NonTrivialDegreePerRowWithColPosition( homalg_variable_534 );
print(transpose(homalg_variable_534));
tst_ignore(string(memory(0)));
matrix homalg_variable_536 = homalg_variable_525*homalg_variable_523;
matrix homalg_variable_537 = homalg_variable_91*homalg_variable_536;
matrix homalg_variable_538 = homalg_variable_537*homalg_variable_534;
matrix homalg_variable_535 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_538,homalg_variable_92);
IsZeroMatrix(homalg_variable_535);
ncols(homalg_variable_535);
matrix homalg_variable_539 = SyzygiesGeneratorsOfRows(homalg_variable_535);
IsZeroMatrix(homalg_variable_539);
ncols(homalg_variable_539);
GetColumnIndependentUnitPositions(homalg_variable_539, list ( 0 ));
matrix homalg_variable_541 = submat(homalg_variable_535,1..8,intvec( 1, 2, 3, 5, 6, 8, 9, 10, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 24, 25, 26, 27, 28 ));
matrix homalg_variable_540 = SyzygiesGeneratorsOfRows(homalg_variable_541);
IsZeroMatrix(homalg_variable_540);
ncols(homalg_variable_540);
GetColumnIndependentUnitPositions(homalg_variable_540, list ( 0 ));
matrix homalg_variable_543 = submat(homalg_variable_541,1..8,intvec( 1, 2, 3, 4, 6, 7, 9, 10, 11, 12, 13, 15, 16, 17, 19, 20, 21, 22 ));
matrix homalg_variable_542 = SyzygiesGeneratorsOfRows(homalg_variable_543);
IsZeroMatrix(homalg_variable_542);
ncols(homalg_variable_542);
GetColumnIndependentUnitPositions(homalg_variable_542, list ( 0 ));
matrix homalg_variable_545 = submat(homalg_variable_543,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 10, 12, 13, 15, 16, 17 ));
matrix homalg_variable_544 = SyzygiesGeneratorsOfRows(homalg_variable_545);
IsZeroMatrix(homalg_variable_544);
ncols(homalg_variable_544);
GetColumnIndependentUnitPositions(homalg_variable_544, list ( 0 ));
matrix homalg_variable_547 = submat(homalg_variable_545,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 11, 12 ));
matrix homalg_variable_546 = SyzygiesGeneratorsOfRows(homalg_variable_547);
IsZeroMatrix(homalg_variable_546);
ncols(homalg_variable_546);
GetColumnIndependentUnitPositions(homalg_variable_546, list ( 0 ));
matrix homalg_variable_549 = submat(homalg_variable_547,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9 ));
matrix homalg_variable_548 = SyzygiesGeneratorsOfRows(homalg_variable_549);
IsZeroMatrix(homalg_variable_548);
ZeroRows(homalg_variable_549);
IsIdentityMatrix(homalg_variable_549);
NonTrivialDegreePerRowWithColPosition( homalg_variable_549 );
print(transpose(homalg_variable_549));
tst_ignore(string(memory(0)));
matrix homalg_variable_551 = homalg_variable_536*homalg_variable_534;
matrix homalg_variable_552 = homalg_variable_93*homalg_variable_551;
matrix homalg_variable_553 = homalg_variable_552*homalg_variable_549;
matrix homalg_variable_550 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_553,homalg_variable_94);
IsZeroMatrix(homalg_variable_550);
ncols(homalg_variable_550);
matrix homalg_variable_554 = SyzygiesGeneratorsOfRows(homalg_variable_550);
IsZeroMatrix(homalg_variable_554);
ZeroRows(homalg_variable_550);
IsIdentityMatrix(homalg_variable_550);
NonTrivialDegreePerRowWithColPosition( homalg_variable_550 );
print(transpose(homalg_variable_550));
tst_ignore(string(memory(0)));
kill homalg_variable_526;kill homalg_variable_527;kill homalg_variable_533;kill homalg_variable_537;kill homalg_variable_538;kill homalg_variable_548;kill homalg_variable_554;
matrix homalg_variable_556 = homalg_variable_551*homalg_variable_549;
matrix homalg_variable_557 = homalg_variable_95*homalg_variable_556;
matrix homalg_variable_558 = homalg_variable_557*homalg_variable_550;
matrix homalg_variable_555 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_558,homalg_variable_96);
IsZeroMatrix(homalg_variable_555);
ncols(homalg_variable_555);
matrix homalg_variable_559 = SyzygiesGeneratorsOfRows(homalg_variable_555);
IsZeroMatrix(homalg_variable_559);
ncols(homalg_variable_559);
GetColumnIndependentUnitPositions(homalg_variable_559, list ( 0 ));
matrix homalg_variable_561 = submat(homalg_variable_555,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_560 = SyzygiesGeneratorsOfRows(homalg_variable_561);
IsZeroMatrix(homalg_variable_560);
ZeroRows(homalg_variable_561);
IsIdentityMatrix(homalg_variable_561);
NonTrivialDegreePerRowWithColPosition( homalg_variable_561 );
print(transpose(homalg_variable_561));
tst_ignore(string(memory(0)));
matrix homalg_variable_563 = homalg_variable_556*homalg_variable_550;
matrix homalg_variable_564 = homalg_variable_97*homalg_variable_563;
matrix homalg_variable_565 = homalg_variable_564*homalg_variable_561;
matrix homalg_variable_562 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_565,homalg_variable_98);
IsZeroMatrix(homalg_variable_562);
ncols(homalg_variable_562);
matrix homalg_variable_566 = SyzygiesGeneratorsOfRows(homalg_variable_562);
IsZeroMatrix(homalg_variable_566);
ncols(homalg_variable_566);
GetColumnIndependentUnitPositions(homalg_variable_566, list ( 0 ));
matrix homalg_variable_568 = submat(homalg_variable_562,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 10 ));
matrix homalg_variable_567 = SyzygiesGeneratorsOfRows(homalg_variable_568);
IsZeroMatrix(homalg_variable_567);
ncols(homalg_variable_567);
GetColumnIndependentUnitPositions(homalg_variable_567, list ( 0 ));
matrix homalg_variable_570 = submat(homalg_variable_568,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_569 = SyzygiesGeneratorsOfRows(homalg_variable_570);
IsZeroMatrix(homalg_variable_569);
ZeroRows(homalg_variable_570);
IsIdentityMatrix(homalg_variable_570);
NonTrivialDegreePerRowWithColPosition( homalg_variable_570 );
print(transpose(homalg_variable_570));
tst_ignore(string(memory(0)));
matrix homalg_variable_572 = homalg_variable_563*homalg_variable_561;
matrix homalg_variable_573 = homalg_variable_99*homalg_variable_572;
matrix homalg_variable_574 = homalg_variable_573*homalg_variable_570;
matrix homalg_variable_571 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_574,homalg_variable_100);
IsZeroMatrix(homalg_variable_571);
ncols(homalg_variable_571);
matrix homalg_variable_575 = SyzygiesGeneratorsOfRows(homalg_variable_571);
IsZeroMatrix(homalg_variable_575);
ncols(homalg_variable_575);
GetColumnIndependentUnitPositions(homalg_variable_575, list ( 0 ));
matrix homalg_variable_577 = submat(homalg_variable_571,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 10 ));
matrix homalg_variable_576 = SyzygiesGeneratorsOfRows(homalg_variable_577);
IsZeroMatrix(homalg_variable_576);
ncols(homalg_variable_576);
GetColumnIndependentUnitPositions(homalg_variable_576, list ( 0 ));
matrix homalg_variable_579 = submat(homalg_variable_577,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_578 = SyzygiesGeneratorsOfRows(homalg_variable_579);
IsZeroMatrix(homalg_variable_578);
ZeroRows(homalg_variable_579);
IsIdentityMatrix(homalg_variable_579);
NonTrivialDegreePerRowWithColPosition( homalg_variable_579 );
print(transpose(homalg_variable_579));
tst_ignore(string(memory(0)));
kill homalg_variable_557;kill homalg_variable_558;kill homalg_variable_560;kill homalg_variable_564;kill homalg_variable_565;kill homalg_variable_569;kill homalg_variable_578;
matrix homalg_variable_581 = homalg_variable_572*homalg_variable_570;
matrix homalg_variable_582 = homalg_variable_101*homalg_variable_581;
matrix homalg_variable_583 = homalg_variable_582*homalg_variable_579;
matrix homalg_variable_580 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_583,homalg_variable_102);
IsZeroMatrix(homalg_variable_580);
ncols(homalg_variable_580);
matrix homalg_variable_584 = SyzygiesGeneratorsOfRows(homalg_variable_580);
IsZeroMatrix(homalg_variable_584);
ncols(homalg_variable_584);
GetColumnIndependentUnitPositions(homalg_variable_584, list ( 0 ));
matrix homalg_variable_586 = submat(homalg_variable_580,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 12 ));
matrix homalg_variable_585 = SyzygiesGeneratorsOfRows(homalg_variable_586);
IsZeroMatrix(homalg_variable_585);
ncols(homalg_variable_585);
GetColumnIndependentUnitPositions(homalg_variable_585, list ( 0 ));
matrix homalg_variable_588 = submat(homalg_variable_586,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_587 = SyzygiesGeneratorsOfRows(homalg_variable_588);
IsZeroMatrix(homalg_variable_587);
ZeroRows(homalg_variable_588);
IsIdentityMatrix(homalg_variable_588);
NonTrivialDegreePerRowWithColPosition( homalg_variable_588 );
print(transpose(homalg_variable_588));
tst_ignore(string(memory(0)));
matrix homalg_variable_590 = homalg_variable_581*homalg_variable_579;
matrix homalg_variable_591 = homalg_variable_103*homalg_variable_590;
matrix homalg_variable_592 = homalg_variable_591*homalg_variable_588;
matrix homalg_variable_589 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_592,homalg_variable_104);
IsZeroMatrix(homalg_variable_589);
ncols(homalg_variable_589);
matrix homalg_variable_593 = SyzygiesGeneratorsOfRows(homalg_variable_589);
IsZeroMatrix(homalg_variable_593);
ncols(homalg_variable_593);
GetColumnIndependentUnitPositions(homalg_variable_593, list ( 0 ));
matrix homalg_variable_595 = submat(homalg_variable_589,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 10, 12, 13, 15, 16, 17 ));
matrix homalg_variable_594 = SyzygiesGeneratorsOfRows(homalg_variable_595);
IsZeroMatrix(homalg_variable_594);
ncols(homalg_variable_594);
GetColumnIndependentUnitPositions(homalg_variable_594, list ( 0 ));
matrix homalg_variable_597 = submat(homalg_variable_595,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 11, 12 ));
matrix homalg_variable_596 = SyzygiesGeneratorsOfRows(homalg_variable_597);
IsZeroMatrix(homalg_variable_596);
ncols(homalg_variable_596);
GetColumnIndependentUnitPositions(homalg_variable_596, list ( 0 ));
matrix homalg_variable_599 = submat(homalg_variable_597,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_598 = SyzygiesGeneratorsOfRows(homalg_variable_599);
IsZeroMatrix(homalg_variable_598);
ZeroRows(homalg_variable_599);
IsIdentityMatrix(homalg_variable_599);
NonTrivialDegreePerRowWithColPosition( homalg_variable_599 );
print(transpose(homalg_variable_599));
tst_ignore(string(memory(0)));
matrix homalg_variable_601 = homalg_variable_590*homalg_variable_588;
matrix homalg_variable_602 = homalg_variable_105*homalg_variable_601;
matrix homalg_variable_603 = homalg_variable_602*homalg_variable_599;
matrix homalg_variable_600 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_603,homalg_variable_106);
IsZeroMatrix(homalg_variable_600);
ncols(homalg_variable_600);
matrix homalg_variable_604 = SyzygiesGeneratorsOfRows(homalg_variable_600);
IsZeroMatrix(homalg_variable_604);
ncols(homalg_variable_604);
GetColumnIndependentUnitPositions(homalg_variable_604, list ( 0 ));
matrix homalg_variable_606 = submat(homalg_variable_600,1..8,intvec( 1, 2, 3, 4, 6, 7, 9, 10, 11, 13, 14, 15, 16, 17, 19, 20, 21, 22 ));
matrix homalg_variable_605 = SyzygiesGeneratorsOfRows(homalg_variable_606);
IsZeroMatrix(homalg_variable_605);
ncols(homalg_variable_605);
GetColumnIndependentUnitPositions(homalg_variable_605, list ( 0 ));
matrix homalg_variable_608 = submat(homalg_variable_606,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 15, 16, 17 ));
matrix homalg_variable_607 = SyzygiesGeneratorsOfRows(homalg_variable_608);
IsZeroMatrix(homalg_variable_607);
ncols(homalg_variable_607);
GetColumnIndependentUnitPositions(homalg_variable_607, list ( 0 ));
matrix homalg_variable_610 = submat(homalg_variable_608,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 12 ));
matrix homalg_variable_609 = SyzygiesGeneratorsOfRows(homalg_variable_610);
IsZeroMatrix(homalg_variable_609);
ncols(homalg_variable_609);
GetColumnIndependentUnitPositions(homalg_variable_609, list ( 0 ));
matrix homalg_variable_612 = submat(homalg_variable_610,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_611 = SyzygiesGeneratorsOfRows(homalg_variable_612);
IsZeroMatrix(homalg_variable_611);
ZeroRows(homalg_variable_612);
IsIdentityMatrix(homalg_variable_612);
NonTrivialDegreePerRowWithColPosition( homalg_variable_612 );
print(transpose(homalg_variable_612));
tst_ignore(string(memory(0)));
kill homalg_variable_583;kill homalg_variable_587;kill homalg_variable_591;kill homalg_variable_592;kill homalg_variable_598;kill homalg_variable_611;
matrix homalg_variable_614 = homalg_variable_601*homalg_variable_599;
matrix homalg_variable_615 = homalg_variable_107*homalg_variable_614;
matrix homalg_variable_616 = homalg_variable_615*homalg_variable_612;
matrix homalg_variable_613 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_616,homalg_variable_108);
IsZeroMatrix(homalg_variable_613);
ncols(homalg_variable_613);
matrix homalg_variable_617 = SyzygiesGeneratorsOfRows(homalg_variable_613);
IsZeroMatrix(homalg_variable_617);
ZeroRows(homalg_variable_613);
IsIdentityMatrix(homalg_variable_613);
NonTrivialDegreePerRowWithColPosition( homalg_variable_613 );
print(transpose(homalg_variable_613));
tst_ignore(string(memory(0)));
matrix homalg_variable_619 = homalg_variable_614*homalg_variable_612;
matrix homalg_variable_620 = homalg_variable_109*homalg_variable_619;
matrix homalg_variable_621 = homalg_variable_620*homalg_variable_613;
matrix homalg_variable_618 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_621,homalg_variable_110);
IsZeroMatrix(homalg_variable_618);
ncols(homalg_variable_618);
matrix homalg_variable_622 = SyzygiesGeneratorsOfRows(homalg_variable_618);
IsZeroMatrix(homalg_variable_622);
ZeroRows(homalg_variable_618);
IsIdentityMatrix(homalg_variable_618);
NonTrivialDegreePerRowWithColPosition( homalg_variable_618 );
print(transpose(homalg_variable_618));
tst_ignore(string(memory(0)));
matrix homalg_variable_624 = homalg_variable_619*homalg_variable_613;
matrix homalg_variable_625 = homalg_variable_111*homalg_variable_624;
matrix homalg_variable_626 = homalg_variable_625*homalg_variable_618;
matrix homalg_variable_623 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_626,homalg_variable_112);
IsZeroMatrix(homalg_variable_623);
ncols(homalg_variable_623);
matrix homalg_variable_627 = SyzygiesGeneratorsOfRows(homalg_variable_623);
IsZeroMatrix(homalg_variable_627);
ncols(homalg_variable_627);
GetColumnIndependentUnitPositions(homalg_variable_627, list ( 0 ));
matrix homalg_variable_629 = submat(homalg_variable_623,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_628 = SyzygiesGeneratorsOfRows(homalg_variable_629);
IsZeroMatrix(homalg_variable_628);
ZeroRows(homalg_variable_629);
IsIdentityMatrix(homalg_variable_629);
NonTrivialDegreePerRowWithColPosition( homalg_variable_629 );
print(transpose(homalg_variable_629));
tst_ignore(string(memory(0)));
kill homalg_variable_615;kill homalg_variable_616;kill homalg_variable_617;kill homalg_variable_620;kill homalg_variable_621;kill homalg_variable_622;kill homalg_variable_628;
matrix homalg_variable_631 = homalg_variable_624*homalg_variable_618;
matrix homalg_variable_632 = homalg_variable_113*homalg_variable_631;
matrix homalg_variable_633 = homalg_variable_632*homalg_variable_629;
matrix homalg_variable_630 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_633,homalg_variable_114);
IsZeroMatrix(homalg_variable_630);
ncols(homalg_variable_630);
matrix homalg_variable_634 = SyzygiesGeneratorsOfRows(homalg_variable_630);
IsZeroMatrix(homalg_variable_634);
ncols(homalg_variable_634);
GetColumnIndependentUnitPositions(homalg_variable_634, list ( 0 ));
matrix homalg_variable_636 = submat(homalg_variable_630,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_635 = SyzygiesGeneratorsOfRows(homalg_variable_636);
IsZeroMatrix(homalg_variable_635);
ZeroRows(homalg_variable_636);
IsIdentityMatrix(homalg_variable_636);
NonTrivialDegreePerRowWithColPosition( homalg_variable_636 );
print(transpose(homalg_variable_636));
tst_ignore(string(memory(0)));
matrix homalg_variable_638 = homalg_variable_631*homalg_variable_629;
matrix homalg_variable_639 = homalg_variable_115*homalg_variable_638;
matrix homalg_variable_640 = homalg_variable_639*homalg_variable_636;
matrix homalg_variable_637 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_640,homalg_variable_116);
IsZeroMatrix(homalg_variable_637);
ncols(homalg_variable_637);
matrix homalg_variable_641 = SyzygiesGeneratorsOfRows(homalg_variable_637);
IsZeroMatrix(homalg_variable_641);
ncols(homalg_variable_641);
GetColumnIndependentUnitPositions(homalg_variable_641, list ( 0 ));
matrix homalg_variable_643 = submat(homalg_variable_637,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 13 ));
matrix homalg_variable_642 = SyzygiesGeneratorsOfRows(homalg_variable_643);
IsZeroMatrix(homalg_variable_642);
ncols(homalg_variable_642);
GetColumnIndependentUnitPositions(homalg_variable_642, list ( 0 ));
matrix homalg_variable_645 = submat(homalg_variable_643,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8, 10 ));
matrix homalg_variable_644 = SyzygiesGeneratorsOfRows(homalg_variable_645);
IsZeroMatrix(homalg_variable_644);
ncols(homalg_variable_644);
GetColumnIndependentUnitPositions(homalg_variable_644, list ( 0 ));
matrix homalg_variable_647 = submat(homalg_variable_645,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_646 = SyzygiesGeneratorsOfRows(homalg_variable_647);
IsZeroMatrix(homalg_variable_646);
ZeroRows(homalg_variable_647);
IsIdentityMatrix(homalg_variable_647);
NonTrivialDegreePerRowWithColPosition( homalg_variable_647 );
print(transpose(homalg_variable_647));
tst_ignore(string(memory(0)));
matrix homalg_variable_649 = homalg_variable_638*homalg_variable_636;
matrix homalg_variable_650 = homalg_variable_117*homalg_variable_649;
matrix homalg_variable_651 = homalg_variable_650*homalg_variable_647;
matrix homalg_variable_648 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_651,homalg_variable_118);
IsZeroMatrix(homalg_variable_648);
ncols(homalg_variable_648);
matrix homalg_variable_652 = SyzygiesGeneratorsOfRows(homalg_variable_648);
IsZeroMatrix(homalg_variable_652);
ncols(homalg_variable_652);
GetColumnIndependentUnitPositions(homalg_variable_652, list ( 0 ));
matrix homalg_variable_654 = submat(homalg_variable_648,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 15, 16, 17 ));
matrix homalg_variable_653 = SyzygiesGeneratorsOfRows(homalg_variable_654);
IsZeroMatrix(homalg_variable_653);
ncols(homalg_variable_653);
GetColumnIndependentUnitPositions(homalg_variable_653, list ( 0 ));
kill homalg_variable_632;kill homalg_variable_633;kill homalg_variable_635;kill homalg_variable_639;kill homalg_variable_640;kill homalg_variable_646;
matrix homalg_variable_656 = submat(homalg_variable_654,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 10, 12, 13 ));
matrix homalg_variable_655 = SyzygiesGeneratorsOfRows(homalg_variable_656);
IsZeroMatrix(homalg_variable_655);
ncols(homalg_variable_655);
GetColumnIndependentUnitPositions(homalg_variable_655, list ( 0 ));
matrix homalg_variable_658 = submat(homalg_variable_656,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8, 10 ));
matrix homalg_variable_657 = SyzygiesGeneratorsOfRows(homalg_variable_658);
IsZeroMatrix(homalg_variable_657);
ncols(homalg_variable_657);
GetColumnIndependentUnitPositions(homalg_variable_657, list ( 0 ));
matrix homalg_variable_660 = submat(homalg_variable_658,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_659 = SyzygiesGeneratorsOfRows(homalg_variable_660);
IsZeroMatrix(homalg_variable_659);
ZeroRows(homalg_variable_660);
IsIdentityMatrix(homalg_variable_660);
NonTrivialDegreePerRowWithColPosition( homalg_variable_660 );
print(transpose(homalg_variable_660));
tst_ignore(string(memory(0)));
matrix homalg_variable_662 = homalg_variable_649*homalg_variable_647;
matrix homalg_variable_663 = homalg_variable_119*homalg_variable_662;
matrix homalg_variable_664 = homalg_variable_663*homalg_variable_660;
matrix homalg_variable_661 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_664,homalg_variable_120);
IsZeroMatrix(homalg_variable_661);
ncols(homalg_variable_661);
matrix homalg_variable_665 = SyzygiesGeneratorsOfRows(homalg_variable_661);
IsZeroMatrix(homalg_variable_665);
ZeroRows(homalg_variable_661);
IsIdentityMatrix(homalg_variable_661);
NonTrivialDegreePerRowWithColPosition( homalg_variable_661 );
print(transpose(homalg_variable_661));
tst_ignore(string(memory(0)));
matrix homalg_variable_667 = homalg_variable_662*homalg_variable_660;
matrix homalg_variable_668 = homalg_variable_121*homalg_variable_667;
matrix homalg_variable_669 = homalg_variable_668*homalg_variable_661;
matrix homalg_variable_666 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_669,homalg_variable_122);
IsZeroMatrix(homalg_variable_666);
ncols(homalg_variable_666);
matrix homalg_variable_670 = SyzygiesGeneratorsOfRows(homalg_variable_666);
IsZeroMatrix(homalg_variable_670);
ncols(homalg_variable_670);
GetColumnIndependentUnitPositions(homalg_variable_670, list ( 0 ));
matrix homalg_variable_672 = submat(homalg_variable_666,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_671 = SyzygiesGeneratorsOfRows(homalg_variable_672);
IsZeroMatrix(homalg_variable_671);
ZeroRows(homalg_variable_672);
IsIdentityMatrix(homalg_variable_672);
NonTrivialDegreePerRowWithColPosition( homalg_variable_672 );
print(transpose(homalg_variable_672));
tst_ignore(string(memory(0)));
kill homalg_variable_659;kill homalg_variable_663;kill homalg_variable_664;kill homalg_variable_665;kill homalg_variable_671;
matrix homalg_variable_674 = homalg_variable_667*homalg_variable_661;
matrix homalg_variable_675 = homalg_variable_123*homalg_variable_674;
matrix homalg_variable_676 = homalg_variable_675*homalg_variable_672;
matrix homalg_variable_673 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_676,homalg_variable_124);
IsZeroMatrix(homalg_variable_673);
ncols(homalg_variable_673);
matrix homalg_variable_677 = SyzygiesGeneratorsOfRows(homalg_variable_673);
IsZeroMatrix(homalg_variable_677);
ncols(homalg_variable_677);
GetColumnIndependentUnitPositions(homalg_variable_677, list ( 0 ));
matrix homalg_variable_679 = submat(homalg_variable_673,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_678 = SyzygiesGeneratorsOfRows(homalg_variable_679);
IsZeroMatrix(homalg_variable_678);
ZeroRows(homalg_variable_679);
IsIdentityMatrix(homalg_variable_679);
NonTrivialDegreePerRowWithColPosition( homalg_variable_679 );
print(transpose(homalg_variable_679));
tst_ignore(string(memory(0)));
matrix homalg_variable_681 = homalg_variable_674*homalg_variable_672;
matrix homalg_variable_682 = homalg_variable_125*homalg_variable_681;
matrix homalg_variable_683 = homalg_variable_682*homalg_variable_679;
matrix homalg_variable_680 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_683,homalg_variable_126);
IsZeroMatrix(homalg_variable_680);
ncols(homalg_variable_680);
matrix homalg_variable_684 = SyzygiesGeneratorsOfRows(homalg_variable_680);
IsZeroMatrix(homalg_variable_684);
ncols(homalg_variable_684);
GetColumnIndependentUnitPositions(homalg_variable_684, list ( 0 ));
matrix homalg_variable_686 = submat(homalg_variable_680,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 11, 12, 13 ));
matrix homalg_variable_685 = SyzygiesGeneratorsOfRows(homalg_variable_686);
IsZeroMatrix(homalg_variable_685);
ncols(homalg_variable_685);
GetColumnIndependentUnitPositions(homalg_variable_685, list ( 0 ));
matrix homalg_variable_688 = submat(homalg_variable_686,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_687 = SyzygiesGeneratorsOfRows(homalg_variable_688);
IsZeroMatrix(homalg_variable_687);
ZeroRows(homalg_variable_688);
IsIdentityMatrix(homalg_variable_688);
NonTrivialDegreePerRowWithColPosition( homalg_variable_688 );
print(transpose(homalg_variable_688));
tst_ignore(string(memory(0)));
matrix homalg_variable_690 = homalg_variable_681*homalg_variable_679;
matrix homalg_variable_691 = homalg_variable_127*homalg_variable_690;
matrix homalg_variable_692 = homalg_variable_691*homalg_variable_688;
matrix homalg_variable_689 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_692,homalg_variable_128);
IsZeroMatrix(homalg_variable_689);
ncols(homalg_variable_689);
matrix homalg_variable_693 = SyzygiesGeneratorsOfRows(homalg_variable_689);
IsZeroMatrix(homalg_variable_693);
ncols(homalg_variable_693);
GetColumnIndependentUnitPositions(homalg_variable_693, list ( 0 ));
matrix homalg_variable_695 = submat(homalg_variable_689,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 13, 15, 16, 17 ));
matrix homalg_variable_694 = SyzygiesGeneratorsOfRows(homalg_variable_695);
IsZeroMatrix(homalg_variable_694);
ncols(homalg_variable_694);
GetColumnIndependentUnitPositions(homalg_variable_694, list ( 0 ));
matrix homalg_variable_697 = submat(homalg_variable_695,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 12 ));
matrix homalg_variable_696 = SyzygiesGeneratorsOfRows(homalg_variable_697);
IsZeroMatrix(homalg_variable_696);
ncols(homalg_variable_696);
GetColumnIndependentUnitPositions(homalg_variable_696, list ( 0 ));
matrix homalg_variable_699 = submat(homalg_variable_697,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_698 = SyzygiesGeneratorsOfRows(homalg_variable_699);
IsZeroMatrix(homalg_variable_698);
ZeroRows(homalg_variable_699);
IsIdentityMatrix(homalg_variable_699);
NonTrivialDegreePerRowWithColPosition( homalg_variable_699 );
print(transpose(homalg_variable_699));
tst_ignore(string(memory(0)));
kill homalg_variable_678;kill homalg_variable_682;kill homalg_variable_683;kill homalg_variable_687;kill homalg_variable_698;
matrix homalg_variable_701 = homalg_variable_690*homalg_variable_688;
matrix homalg_variable_702 = homalg_variable_129*homalg_variable_701;
matrix homalg_variable_703 = homalg_variable_702*homalg_variable_699;
matrix homalg_variable_700 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_703,homalg_variable_130);
IsZeroMatrix(homalg_variable_700);
ncols(homalg_variable_700);
matrix homalg_variable_704 = SyzygiesGeneratorsOfRows(homalg_variable_700);
IsZeroMatrix(homalg_variable_704);
ncols(homalg_variable_704);
GetColumnIndependentUnitPositions(homalg_variable_704, list ( 0 ));
matrix homalg_variable_706 = submat(homalg_variable_700,1..8,intvec( 1, 2, 3, 4, 6, 9, 11, 13, 15, 16, 19, 21, 22 ));
matrix homalg_variable_705 = SyzygiesGeneratorsOfRows(homalg_variable_706);
IsZeroMatrix(homalg_variable_705);
ncols(homalg_variable_705);
GetColumnIndependentUnitPositions(homalg_variable_705, list ( 0 ));
matrix homalg_variable_708 = submat(homalg_variable_706,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 11 ));
matrix homalg_variable_707 = SyzygiesGeneratorsOfRows(homalg_variable_708);
IsZeroMatrix(homalg_variable_707);
ZeroRows(homalg_variable_708);
IsIdentityMatrix(homalg_variable_708);
NonTrivialDegreePerRowWithColPosition( homalg_variable_708 );
print(transpose(homalg_variable_708));
tst_ignore(string(memory(0)));
matrix homalg_variable_710 = homalg_variable_701*homalg_variable_699;
matrix homalg_variable_711 = homalg_variable_131*homalg_variable_710;
matrix homalg_variable_712 = homalg_variable_711*homalg_variable_708;
matrix homalg_variable_709 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_712,homalg_variable_132);
IsZeroMatrix(homalg_variable_709);
ncols(homalg_variable_709);
matrix homalg_variable_713 = SyzygiesGeneratorsOfRows(homalg_variable_709);
IsZeroMatrix(homalg_variable_713);
ZeroRows(homalg_variable_709);
IsIdentityMatrix(homalg_variable_709);
NonTrivialDegreePerRowWithColPosition( homalg_variable_709 );
print(transpose(homalg_variable_709));
tst_ignore(string(memory(0)));
kill homalg_variable_702;kill homalg_variable_703;kill homalg_variable_707;kill homalg_variable_713;
matrix homalg_variable_715 = homalg_variable_710*homalg_variable_708;
matrix homalg_variable_716 = homalg_variable_133*homalg_variable_715;
matrix homalg_variable_717 = homalg_variable_716*homalg_variable_709;
matrix homalg_variable_714 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_717,homalg_variable_134);
IsZeroMatrix(homalg_variable_714);
ncols(homalg_variable_714);
matrix homalg_variable_718 = SyzygiesGeneratorsOfRows(homalg_variable_714);
IsZeroMatrix(homalg_variable_718);
ncols(homalg_variable_718);
GetColumnIndependentUnitPositions(homalg_variable_718, list ( 0 ));
matrix homalg_variable_720 = submat(homalg_variable_714,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_719 = SyzygiesGeneratorsOfRows(homalg_variable_720);
IsZeroMatrix(homalg_variable_719);
ZeroRows(homalg_variable_720);
IsIdentityMatrix(homalg_variable_720);
NonTrivialDegreePerRowWithColPosition( homalg_variable_720 );
print(transpose(homalg_variable_720));
tst_ignore(string(memory(0)));
matrix homalg_variable_722 = homalg_variable_715*homalg_variable_709;
matrix homalg_variable_723 = homalg_variable_135*homalg_variable_722;
matrix homalg_variable_724 = homalg_variable_723*homalg_variable_720;
matrix homalg_variable_721 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_724,homalg_variable_136);
IsZeroMatrix(homalg_variable_721);
ncols(homalg_variable_721);
matrix homalg_variable_725 = SyzygiesGeneratorsOfRows(homalg_variable_721);
IsZeroMatrix(homalg_variable_725);
ncols(homalg_variable_725);
GetColumnIndependentUnitPositions(homalg_variable_725, list ( 0 ));
matrix homalg_variable_727 = submat(homalg_variable_721,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 10 ));
matrix homalg_variable_726 = SyzygiesGeneratorsOfRows(homalg_variable_727);
IsZeroMatrix(homalg_variable_726);
ncols(homalg_variable_726);
GetColumnIndependentUnitPositions(homalg_variable_726, list ( 0 ));
matrix homalg_variable_729 = submat(homalg_variable_727,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_728 = SyzygiesGeneratorsOfRows(homalg_variable_729);
IsZeroMatrix(homalg_variable_728);
ZeroRows(homalg_variable_729);
IsIdentityMatrix(homalg_variable_729);
NonTrivialDegreePerRowWithColPosition( homalg_variable_729 );
print(transpose(homalg_variable_729));
tst_ignore(string(memory(0)));
matrix homalg_variable_731 = homalg_variable_722*homalg_variable_720;
matrix homalg_variable_732 = homalg_variable_137*homalg_variable_731;
matrix homalg_variable_733 = homalg_variable_732*homalg_variable_729;
matrix homalg_variable_730 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_733,homalg_variable_138);
IsZeroMatrix(homalg_variable_730);
ncols(homalg_variable_730);
matrix homalg_variable_734 = SyzygiesGeneratorsOfRows(homalg_variable_730);
IsZeroMatrix(homalg_variable_734);
ncols(homalg_variable_734);
GetColumnIndependentUnitPositions(homalg_variable_734, list ( 0 ));
matrix homalg_variable_736 = submat(homalg_variable_730,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 11, 12, 13 ));
matrix homalg_variable_735 = SyzygiesGeneratorsOfRows(homalg_variable_736);
IsZeroMatrix(homalg_variable_735);
ncols(homalg_variable_735);
GetColumnIndependentUnitPositions(homalg_variable_735, list ( 0 ));
matrix homalg_variable_738 = submat(homalg_variable_736,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 10 ));
matrix homalg_variable_737 = SyzygiesGeneratorsOfRows(homalg_variable_738);
IsZeroMatrix(homalg_variable_737);
ncols(homalg_variable_737);
GetColumnIndependentUnitPositions(homalg_variable_737, list ( 0 ));
matrix homalg_variable_740 = submat(homalg_variable_738,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_739 = SyzygiesGeneratorsOfRows(homalg_variable_740);
IsZeroMatrix(homalg_variable_739);
ZeroRows(homalg_variable_740);
IsIdentityMatrix(homalg_variable_740);
NonTrivialDegreePerRowWithColPosition( homalg_variable_740 );
print(transpose(homalg_variable_740));
tst_ignore(string(memory(0)));
kill homalg_variable_719;kill homalg_variable_723;kill homalg_variable_724;kill homalg_variable_728;kill homalg_variable_739;
matrix homalg_variable_742 = homalg_variable_731*homalg_variable_729;
matrix homalg_variable_743 = homalg_variable_139*homalg_variable_742;
matrix homalg_variable_744 = homalg_variable_743*homalg_variable_740;
matrix homalg_variable_741 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_744,homalg_variable_140);
IsZeroMatrix(homalg_variable_741);
ncols(homalg_variable_741);
matrix homalg_variable_745 = SyzygiesGeneratorsOfRows(homalg_variable_741);
IsZeroMatrix(homalg_variable_745);
ncols(homalg_variable_745);
GetColumnIndependentUnitPositions(homalg_variable_745, list ( 0 ));
matrix homalg_variable_747 = submat(homalg_variable_741,1..8,intvec( 1, 2, 3, 4, 5, 7, 8, 10, 11, 12, 14, 15, 16, 17 ));
matrix homalg_variable_746 = SyzygiesGeneratorsOfRows(homalg_variable_747);
IsZeroMatrix(homalg_variable_746);
ncols(homalg_variable_746);
GetColumnIndependentUnitPositions(homalg_variable_746, list ( 0 ));
matrix homalg_variable_749 = submat(homalg_variable_747,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 11, 12 ));
matrix homalg_variable_748 = SyzygiesGeneratorsOfRows(homalg_variable_749);
IsZeroMatrix(homalg_variable_748);
ncols(homalg_variable_748);
GetColumnIndependentUnitPositions(homalg_variable_748, list ( 0 ));
matrix homalg_variable_751 = submat(homalg_variable_749,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_750 = SyzygiesGeneratorsOfRows(homalg_variable_751);
IsZeroMatrix(homalg_variable_750);
ZeroRows(homalg_variable_751);
IsIdentityMatrix(homalg_variable_751);
NonTrivialDegreePerRowWithColPosition( homalg_variable_751 );
print(transpose(homalg_variable_751));
tst_ignore(string(memory(0)));
matrix homalg_variable_753 = homalg_variable_742*homalg_variable_740;
matrix homalg_variable_754 = homalg_variable_141*homalg_variable_753;
matrix homalg_variable_755 = homalg_variable_754*homalg_variable_751;
matrix homalg_variable_752 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_755,homalg_variable_142);
IsZeroMatrix(homalg_variable_752);
ncols(homalg_variable_752);
matrix homalg_variable_756 = SyzygiesGeneratorsOfRows(homalg_variable_752);
IsZeroMatrix(homalg_variable_756);
ncols(homalg_variable_756);
GetColumnIndependentUnitPositions(homalg_variable_756, list ( 0 ));
matrix homalg_variable_758 = submat(homalg_variable_752,1..8,intvec( 1, 2, 3, 4, 6, 7, 9, 10, 11, 13, 14, 15, 16, 17, 19, 20, 21, 22 ));
matrix homalg_variable_757 = SyzygiesGeneratorsOfRows(homalg_variable_758);
IsZeroMatrix(homalg_variable_757);
ncols(homalg_variable_757);
GetColumnIndependentUnitPositions(homalg_variable_757, list ( 0 ));
matrix homalg_variable_760 = submat(homalg_variable_758,1..8,intvec( 1, 2, 3, 4, 5, 7, 10, 11, 12, 15, 16 ));
matrix homalg_variable_759 = SyzygiesGeneratorsOfRows(homalg_variable_760);
IsZeroMatrix(homalg_variable_759);
ncols(homalg_variable_759);
GetColumnIndependentUnitPositions(homalg_variable_759, list ( 0 ));
matrix homalg_variable_762 = submat(homalg_variable_760,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8, 10 ));
matrix homalg_variable_761 = SyzygiesGeneratorsOfRows(homalg_variable_762);
IsZeroMatrix(homalg_variable_761);
ncols(homalg_variable_761);
GetColumnIndependentUnitPositions(homalg_variable_761, list ( 0 ));
matrix homalg_variable_764 = submat(homalg_variable_762,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_763 = SyzygiesGeneratorsOfRows(homalg_variable_764);
IsZeroMatrix(homalg_variable_763);
ZeroRows(homalg_variable_764);
IsIdentityMatrix(homalg_variable_764);
NonTrivialDegreePerRowWithColPosition( homalg_variable_764 );
print(transpose(homalg_variable_764));
tst_ignore(string(memory(0)));
kill homalg_variable_743;kill homalg_variable_744;kill homalg_variable_750;kill homalg_variable_763;
matrix homalg_variable_766 = homalg_variable_753*homalg_variable_751;
matrix homalg_variable_767 = homalg_variable_143*homalg_variable_766;
matrix homalg_variable_768 = homalg_variable_767*homalg_variable_764;
matrix homalg_variable_765 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_768,homalg_variable_144);
IsZeroMatrix(homalg_variable_765);
ncols(homalg_variable_765);
matrix homalg_variable_769 = SyzygiesGeneratorsOfRows(homalg_variable_765);
IsZeroMatrix(homalg_variable_769);
ZeroRows(homalg_variable_765);
IsIdentityMatrix(homalg_variable_765);
NonTrivialDegreePerRowWithColPosition( homalg_variable_765 );
print(transpose(homalg_variable_765));
tst_ignore(string(memory(0)));
matrix homalg_variable_771 = homalg_variable_766*homalg_variable_764;
matrix homalg_variable_772 = homalg_variable_145*homalg_variable_771;
matrix homalg_variable_773 = homalg_variable_772*homalg_variable_765;
matrix homalg_variable_770 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_773,homalg_variable_146);
IsZeroMatrix(homalg_variable_770);
ncols(homalg_variable_770);
matrix homalg_variable_774 = SyzygiesGeneratorsOfRows(homalg_variable_770);
IsZeroMatrix(homalg_variable_774);
ncols(homalg_variable_774);
GetColumnIndependentUnitPositions(homalg_variable_774, list ( 0 ));
matrix homalg_variable_776 = submat(homalg_variable_770,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_775 = SyzygiesGeneratorsOfRows(homalg_variable_776);
IsZeroMatrix(homalg_variable_775);
ZeroRows(homalg_variable_776);
IsIdentityMatrix(homalg_variable_776);
NonTrivialDegreePerRowWithColPosition( homalg_variable_776 );
print(transpose(homalg_variable_776));
tst_ignore(string(memory(0)));
kill homalg_variable_767;kill homalg_variable_768;kill homalg_variable_769;kill homalg_variable_775;
matrix homalg_variable_778 = homalg_variable_771*homalg_variable_765;
matrix homalg_variable_779 = homalg_variable_147*homalg_variable_778;
matrix homalg_variable_780 = homalg_variable_779*homalg_variable_776;
matrix homalg_variable_777 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_780,homalg_variable_148);
IsZeroMatrix(homalg_variable_777);
ncols(homalg_variable_777);
matrix homalg_variable_781 = SyzygiesGeneratorsOfRows(homalg_variable_777);
IsZeroMatrix(homalg_variable_781);
ncols(homalg_variable_781);
GetColumnIndependentUnitPositions(homalg_variable_781, list ( 0 ));
matrix homalg_variable_783 = submat(homalg_variable_777,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 10 ));
matrix homalg_variable_782 = SyzygiesGeneratorsOfRows(homalg_variable_783);
IsZeroMatrix(homalg_variable_782);
ncols(homalg_variable_782);
GetColumnIndependentUnitPositions(homalg_variable_782, list ( 0 ));
matrix homalg_variable_785 = submat(homalg_variable_783,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_784 = SyzygiesGeneratorsOfRows(homalg_variable_785);
IsZeroMatrix(homalg_variable_784);
ZeroRows(homalg_variable_785);
IsIdentityMatrix(homalg_variable_785);
NonTrivialDegreePerRowWithColPosition( homalg_variable_785 );
print(transpose(homalg_variable_785));
tst_ignore(string(memory(0)));
matrix homalg_variable_787 = homalg_variable_778*homalg_variable_776;
matrix homalg_variable_788 = homalg_variable_149*homalg_variable_787;
matrix homalg_variable_789 = homalg_variable_788*homalg_variable_785;
matrix homalg_variable_786 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_789,homalg_variable_150);
IsZeroMatrix(homalg_variable_786);
ncols(homalg_variable_786);
matrix homalg_variable_790 = SyzygiesGeneratorsOfRows(homalg_variable_786);
IsZeroMatrix(homalg_variable_790);
ncols(homalg_variable_790);
GetColumnIndependentUnitPositions(homalg_variable_790, list ( 0 ));
matrix homalg_variable_792 = submat(homalg_variable_786,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 9, 11, 12, 13 ));
matrix homalg_variable_791 = SyzygiesGeneratorsOfRows(homalg_variable_792);
IsZeroMatrix(homalg_variable_791);
ncols(homalg_variable_791);
GetColumnIndependentUnitPositions(homalg_variable_791, list ( 0 ));
matrix homalg_variable_794 = submat(homalg_variable_792,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9 ));
matrix homalg_variable_793 = SyzygiesGeneratorsOfRows(homalg_variable_794);
IsZeroMatrix(homalg_variable_793);
ZeroRows(homalg_variable_794);
IsIdentityMatrix(homalg_variable_794);
NonTrivialDegreePerRowWithColPosition( homalg_variable_794 );
print(transpose(homalg_variable_794));
tst_ignore(string(memory(0)));
kill homalg_variable_780;kill homalg_variable_784;kill homalg_variable_793;
matrix homalg_variable_796 = homalg_variable_787*homalg_variable_785;
matrix homalg_variable_797 = homalg_variable_151*homalg_variable_796;
matrix homalg_variable_798 = homalg_variable_797*homalg_variable_794;
matrix homalg_variable_795 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_798,homalg_variable_152);
IsZeroMatrix(homalg_variable_795);
ncols(homalg_variable_795);
matrix homalg_variable_799 = SyzygiesGeneratorsOfRows(homalg_variable_795);
IsZeroMatrix(homalg_variable_799);
ncols(homalg_variable_799);
GetColumnIndependentUnitPositions(homalg_variable_799, list ( 0 ));
matrix homalg_variable_801 = submat(homalg_variable_795,1..8,intvec( 1, 2, 3, 4, 5, 7, 10, 14, 17 ));
matrix homalg_variable_800 = SyzygiesGeneratorsOfRows(homalg_variable_801);
IsZeroMatrix(homalg_variable_800);
ncols(homalg_variable_800);
GetColumnIndependentUnitPositions(homalg_variable_800, list ( 0 ));
matrix homalg_variable_803 = submat(homalg_variable_801,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_802 = SyzygiesGeneratorsOfRows(homalg_variable_803);
IsZeroMatrix(homalg_variable_802);
ZeroRows(homalg_variable_803);
IsIdentityMatrix(homalg_variable_803);
NonTrivialDegreePerRowWithColPosition( homalg_variable_803 );
print(transpose(homalg_variable_803));
tst_ignore(string(memory(0)));
matrix homalg_variable_805 = homalg_variable_796*homalg_variable_794;
matrix homalg_variable_806 = homalg_variable_153*homalg_variable_805;
matrix homalg_variable_807 = homalg_variable_806*homalg_variable_803;
matrix homalg_variable_804 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_807,homalg_variable_154);
IsZeroMatrix(homalg_variable_804);
ncols(homalg_variable_804);
matrix homalg_variable_808 = SyzygiesGeneratorsOfRows(homalg_variable_804);
IsZeroMatrix(homalg_variable_808);
ncols(homalg_variable_808);
GetColumnIndependentUnitPositions(homalg_variable_808, list ( 0 ));
matrix homalg_variable_810 = submat(homalg_variable_804,1..8,intvec( 1, 2, 3, 4, 6, 9, 11, 13, 15, 16, 19, 21, 22 ));
matrix homalg_variable_809 = SyzygiesGeneratorsOfRows(homalg_variable_810);
IsZeroMatrix(homalg_variable_809);
ncols(homalg_variable_809);
GetColumnIndependentUnitPositions(homalg_variable_809, list ( 0 ));
matrix homalg_variable_812 = submat(homalg_variable_810,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 11 ));
matrix homalg_variable_811 = SyzygiesGeneratorsOfRows(homalg_variable_812);
IsZeroMatrix(homalg_variable_811);
ZeroRows(homalg_variable_812);
IsIdentityMatrix(homalg_variable_812);
NonTrivialDegreePerRowWithColPosition( homalg_variable_812 );
print(transpose(homalg_variable_812));
tst_ignore(string(memory(0)));
kill homalg_variable_802;kill homalg_variable_811;
matrix homalg_variable_814 = homalg_variable_805*homalg_variable_803;
matrix homalg_variable_815 = homalg_variable_155*homalg_variable_814;
matrix homalg_variable_816 = homalg_variable_815*homalg_variable_812;
matrix homalg_variable_813 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_816,homalg_variable_156);
IsZeroMatrix(homalg_variable_813);
ncols(homalg_variable_813);
matrix homalg_variable_817 = SyzygiesGeneratorsOfRows(homalg_variable_813);
IsZeroMatrix(homalg_variable_817);
ZeroRows(homalg_variable_813);
IsIdentityMatrix(homalg_variable_813);
NonTrivialDegreePerRowWithColPosition( homalg_variable_813 );
print(transpose(homalg_variable_813));
tst_ignore(string(memory(0)));
matrix homalg_variable_819 = homalg_variable_814*homalg_variable_812;
matrix homalg_variable_820 = homalg_variable_157*homalg_variable_819;
matrix homalg_variable_821 = homalg_variable_820*homalg_variable_813;
matrix homalg_variable_818 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_821,homalg_variable_158);
IsZeroMatrix(homalg_variable_818);
ncols(homalg_variable_818);
matrix homalg_variable_822 = SyzygiesGeneratorsOfRows(homalg_variable_818);
IsZeroMatrix(homalg_variable_822);
ncols(homalg_variable_822);
GetColumnIndependentUnitPositions(homalg_variable_822, list ( 0 ));
matrix homalg_variable_824 = submat(homalg_variable_818,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_823 = SyzygiesGeneratorsOfRows(homalg_variable_824);
IsZeroMatrix(homalg_variable_823);
ZeroRows(homalg_variable_824);
IsIdentityMatrix(homalg_variable_824);
NonTrivialDegreePerRowWithColPosition( homalg_variable_824 );
print(transpose(homalg_variable_824));
tst_ignore(string(memory(0)));
matrix homalg_variable_826 = homalg_variable_819*homalg_variable_813;
matrix homalg_variable_827 = homalg_variable_159*homalg_variable_826;
matrix homalg_variable_828 = homalg_variable_827*homalg_variable_824;
matrix homalg_variable_825 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_828,homalg_variable_160);
IsZeroMatrix(homalg_variable_825);
ncols(homalg_variable_825);
matrix homalg_variable_829 = SyzygiesGeneratorsOfRows(homalg_variable_825);
IsZeroMatrix(homalg_variable_829);
ncols(homalg_variable_829);
GetColumnIndependentUnitPositions(homalg_variable_829, list ( 0 ));
kill homalg_variable_817;kill homalg_variable_820;kill homalg_variable_821;kill homalg_variable_823;
matrix homalg_variable_831 = submat(homalg_variable_825,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 9, 10 ));
matrix homalg_variable_830 = SyzygiesGeneratorsOfRows(homalg_variable_831);
IsZeroMatrix(homalg_variable_830);
ncols(homalg_variable_830);
GetColumnIndependentUnitPositions(homalg_variable_830, list ( 0 ));
matrix homalg_variable_833 = submat(homalg_variable_831,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_832 = SyzygiesGeneratorsOfRows(homalg_variable_833);
IsZeroMatrix(homalg_variable_832);
ZeroRows(homalg_variable_833);
IsIdentityMatrix(homalg_variable_833);
NonTrivialDegreePerRowWithColPosition( homalg_variable_833 );
print(transpose(homalg_variable_833));
tst_ignore(string(memory(0)));
matrix homalg_variable_835 = homalg_variable_826*homalg_variable_824;
matrix homalg_variable_836 = homalg_variable_161*homalg_variable_835;
matrix homalg_variable_837 = homalg_variable_836*homalg_variable_833;
matrix homalg_variable_834 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_837,homalg_variable_162);
IsZeroMatrix(homalg_variable_834);
ncols(homalg_variable_834);
matrix homalg_variable_838 = SyzygiesGeneratorsOfRows(homalg_variable_834);
IsZeroMatrix(homalg_variable_838);
ncols(homalg_variable_838);
GetColumnIndependentUnitPositions(homalg_variable_838, list ( 0 ));
matrix homalg_variable_840 = submat(homalg_variable_834,1..8,intvec( 1, 2, 3, 4, 5, 6, 8, 11, 13 ));
matrix homalg_variable_839 = SyzygiesGeneratorsOfRows(homalg_variable_840);
IsZeroMatrix(homalg_variable_839);
ncols(homalg_variable_839);
GetColumnIndependentUnitPositions(homalg_variable_839, list ( 0 ));
matrix homalg_variable_842 = submat(homalg_variable_840,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_841 = SyzygiesGeneratorsOfRows(homalg_variable_842);
IsZeroMatrix(homalg_variable_841);
ZeroRows(homalg_variable_842);
IsIdentityMatrix(homalg_variable_842);
NonTrivialDegreePerRowWithColPosition( homalg_variable_842 );
print(transpose(homalg_variable_842));
tst_ignore(string(memory(0)));
matrix homalg_variable_844 = homalg_variable_835*homalg_variable_833;
matrix homalg_variable_845 = homalg_variable_163*homalg_variable_844;
matrix homalg_variable_846 = homalg_variable_845*homalg_variable_842;
matrix homalg_variable_843 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_846,homalg_variable_164);
IsZeroMatrix(homalg_variable_843);
ncols(homalg_variable_843);
matrix homalg_variable_847 = SyzygiesGeneratorsOfRows(homalg_variable_843);
IsZeroMatrix(homalg_variable_847);
ncols(homalg_variable_847);
GetColumnIndependentUnitPositions(homalg_variable_847, list ( 0 ));
kill homalg_variable_832;kill homalg_variable_836;kill homalg_variable_837;kill homalg_variable_841;
matrix homalg_variable_849 = submat(homalg_variable_843,1..8,intvec( 1, 2, 3, 4, 5, 7, 10, 14, 17 ));
matrix homalg_variable_848 = SyzygiesGeneratorsOfRows(homalg_variable_849);
IsZeroMatrix(homalg_variable_848);
ncols(homalg_variable_848);
GetColumnIndependentUnitPositions(homalg_variable_848, list ( 0 ));
matrix homalg_variable_851 = submat(homalg_variable_849,1..8,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_850 = SyzygiesGeneratorsOfRows(homalg_variable_851);
IsZeroMatrix(homalg_variable_850);
ZeroRows(homalg_variable_851);
IsIdentityMatrix(homalg_variable_851);
NonTrivialDegreePerRowWithColPosition( homalg_variable_851 );
print(transpose(homalg_variable_851));
tst_ignore(string(memory(0)));

tst_status(1);$

