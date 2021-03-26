LIB "tst.lib"; tst_init();


option(noredefine);option(redSB);LIB "matrix.lib";LIB "involut.lib";LIB "nctools.lib";LIB "polylib.lib";LIB "finvar.lib";
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
      if (deg(M[k,j]) == 0) //IsUnit
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


proc GetColumnIndependentUnitPositions_Z (matrix M, list pos_list)
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
      if (M[k,j] == 1 || M[k,j] == -1) //IsUnit
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
      if (deg(M[i,k]) == 0) //IsUnit
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


proc GetRowIndependentUnitPositions_Z (matrix M, list pos_list)
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
      if (M[i,k] == 1 || M[i,k] == -1) //IsUnit
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
      if (deg(M[rest[i],j]) == 0) //IsUnit
      {
        return(string(j,",",rest[i])); // this is not a mistake
      }
    }
  }
  return("fail");
}


proc GetUnitPosition_Z (matrix M, list pos_list)
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
      if (M[rest[i],j] == 1 || M[rest[i],j] == -1) //IsUnit
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


proc PositionOfFirstNonZeroEntryPerRow (matrix M)
{
  int b = 1;
  intmat m[1][ncols(M)];
  for (int i=1; i<=ncols(M); i++)
  {
    for (int j=1; j<=nrows(M); j++)
    {
      if ( M[j,i] <> 0 ) { m[1,i] = j; break; }
    }
    if ( b && i > 1 ) { if ( m[1,i] <> m[1,i-1] ) { b = 0; } } // Singular is strange
  }
  if ( b ) { return(m[1,1]); } else { return(m); }
}


proc PositionOfFirstNonZeroEntryPerColumn (matrix M)
{
  int b = 1;
  intmat m[1][nrows(M)];
  for (int j=1; j<=nrows(M); j++)
  {
    for (int i=1; i<=ncols(M); i++)
    {
      if ( M[j,i] <> 0 ) { m[1,j] = i; break; }
    }
    if ( b && j > 1 ) { if ( m[1,j] <> m[1,j-1] ) { b = 0; } } // Singular is strange
  }
  if ( b ) { return(m[1,1]); } else { return(m); }
}


proc IndicatorMatrixOfNonZeroEntries(matrix M)
{
  intmat m[ncols(M)][nrows(M)];
  for (int i=1; i<=ncols(M); i++)
  {
    for (int j=1; j<=nrows(M); j++)
    {
      m[i,j] = ( M[j,i] <> 0 );
    }
  }
  return(m);
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
  matrix B = BasisOfRowModule(M);
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
  return(BasisOfRowModule(modulo(M1, M2)));
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


if ( defined(superCommutative) == 1 ) // the new name of the SCA constructor
{ proc superCommutative_ForHomalg = superCommutative; }
else
{
  if ( defined(SuperCommutative) == 1 ) // the old name of the SCA constructor
  { proc superCommutative_ForHomalg = SuperCommutative; }
}



proc CoefficientsOfUnreducedNumeratorOfWeightedHilbertPoincareSeries (module m,weights,degrees)
{
  module M = std(m);
  attrib(M,"isHomog",degrees);
  return(hilb(M,1,weights));
}


proc PrimaryDecomposition (matrix m)
{
  return(primdecSY(m))
}

ring homalg_variable_1 = 0,dummy_variable,dp;
setring homalg_variable_1;
short=0;option(redTail);

proc Involution (matrix m)
{
  return(transpose(m));
}

ring homalg_variable_2 = (0),(x,y,z),dp;
setring homalg_variable_2;
short=0;option(redTail);

proc Involution (matrix m)
{
  return(transpose(m));
}

matrix homalg_variable_3[6][5] =  x*y,  y*z,    z,        0,         0,    x^3*z,x^2*z^2,0,        x*z^2,     -z^2, x^4,  x^3*z,  0,        x^2*z,     -x*z, 0,    0,      x*y,      -y^2,      x^2-1,0,    0,      x^2*z,    -x*y*z,    y*z,  0,    0,      x^2*y-x^2,-x*y^2+x*y,y^2-y ;
homalg_variable_3 = transpose(homalg_variable_3);
matrix homalg_variable_4 = BasisOfRowModule(homalg_variable_3);
ncols(homalg_variable_4);
homalg_variable_4==homalg_variable_3;
matrix homalg_variable_5 = SyzygiesGeneratorsOfRows(homalg_variable_4);
IsZeroMatrix(homalg_variable_5);
ncols(homalg_variable_5);
GetColumnIndependentUnitPositions(homalg_variable_5, list ( 0 ));
matrix homalg_variable_7 = Involution(homalg_variable_4);
matrix homalg_variable_6 = BasisOfColumnModule(homalg_variable_7);
nrows(homalg_variable_6);
homalg_variable_6==homalg_variable_7;
matrix homalg_variable_9[5][5] = unitmat(5);
matrix homalg_variable_8 = DecideZeroColumns(homalg_variable_9,homalg_variable_6);
IsZeroMatrix(homalg_variable_8);
ZeroColumns(homalg_variable_8);
IsIdentityMatrix(homalg_variable_8);
matrix homalg_variable_10 = SyzygiesGeneratorsOfColumns(homalg_variable_4);
IsZeroMatrix(homalg_variable_10);
nrows(homalg_variable_10);
matrix homalg_variable_11 = ReducedSyzygiesGeneratorsOfRows(homalg_variable_10);
IsZeroMatrix(homalg_variable_11);
ncols(homalg_variable_11);
matrix homalg_variable_12 = DecideZeroRows(homalg_variable_11,homalg_variable_4);
IsZeroMatrix(homalg_variable_12);
ZeroRows(homalg_variable_12);
matrix homalg_variable_13[5][5] =  x^2+y-z,x*z-z,  0,        z,         -z,   x-1,    x+y-1,  -y,       -1,        0,    x^3+y,  x^2*z+y,x^2+y^2+y,-x*y+x*z+y,x*y-z,x,      x,      x,        y^2+x,     1,    0,      0,      -x*y,     y^2,       1     ;
homalg_variable_13 = transpose(homalg_variable_13);
matrix homalg_variable_14 = ReducedSyzygiesGeneratorsOfRows(homalg_variable_4);
IsZeroMatrix(homalg_variable_14);
ncols(homalg_variable_14);
matrix homalg_variable_15 = ReducedSyzygiesGeneratorsOfRows(homalg_variable_14);
IsZeroMatrix(homalg_variable_15);
ncols(homalg_variable_15);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_15); matrix homalg_variable_16 = homalg_variable_l[1]; matrix homalg_variable_17 = homalg_variable_l[2];
nrows(homalg_variable_16);
matrix homalg_variable_18[1][1] = unitmat(1);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_18,homalg_variable_16); matrix homalg_variable_19 = homalg_variable_l[1]; matrix homalg_variable_20 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_19);
matrix homalg_variable_21 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_4);
IsZeroMatrix(homalg_variable_21);
nrows(homalg_variable_21);
ZeroColumns(homalg_variable_21);
matrix homalg_variable_22 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_14);
IsZeroMatrix(homalg_variable_22);
nrows(homalg_variable_22);
matrix homalg_variable_23 = BasisOfColumnModule(homalg_variable_4);
nrows(homalg_variable_23);
homalg_variable_23==homalg_variable_4;
matrix homalg_variable_24 = DecideZeroColumns(homalg_variable_22,homalg_variable_23);
IsZeroMatrix(homalg_variable_24);
ZeroColumns(homalg_variable_24);
matrix homalg_variable_25 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_15);
IsZeroMatrix(homalg_variable_25);
nrows(homalg_variable_25);
matrix homalg_variable_26 = BasisOfColumnModule(homalg_variable_14);
nrows(homalg_variable_26);
homalg_variable_26==homalg_variable_14;
matrix homalg_variable_27 = DecideZeroColumns(homalg_variable_25,homalg_variable_26);
IsZeroMatrix(homalg_variable_27);
ZeroColumns(homalg_variable_27);
matrix homalg_variable_28 = SyzygiesGeneratorsOfColumns(homalg_variable_21);
IsZeroMatrix(homalg_variable_28);
matrix homalg_variable_29 = RelativeSyzygiesGeneratorsOfColumns(homalg_variable_24,homalg_variable_23);
IsZeroMatrix(homalg_variable_29);
nrows(homalg_variable_29);
matrix homalg_variable_30 = BasisOfColumnModule(homalg_variable_29);
nrows(homalg_variable_30);
homalg_variable_30==homalg_variable_29;
matrix homalg_variable_31 = SyzygiesGeneratorsOfColumns(homalg_variable_30);
IsZeroMatrix(homalg_variable_31);
nrows(homalg_variable_31);
GetRowIndependentUnitPositions(homalg_variable_31, list ( 0 ));
matrix homalg_variable_33 = submat(homalg_variable_30,intvec( 1, 3, 4, 5, 6 ),1..4);
matrix homalg_variable_32 = SyzygiesGeneratorsOfColumns(homalg_variable_33);
IsZeroMatrix(homalg_variable_32);
nrows(homalg_variable_32);
GetRowIndependentUnitPositions(homalg_variable_32, list ( 0 ));
matrix homalg_variable_34 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_33);
IsZeroMatrix(homalg_variable_34);
nrows(homalg_variable_34);
matrix homalg_variable_36 = submat(homalg_variable_27,intvec( 1, 4 ),1..4);
matrix homalg_variable_35 = RelativeSyzygiesGeneratorsOfColumns(homalg_variable_36,homalg_variable_26);
IsZeroMatrix(homalg_variable_35);
nrows(homalg_variable_35);
matrix homalg_variable_37 = BasisOfColumnModule(homalg_variable_35);
nrows(homalg_variable_37);
homalg_variable_37==homalg_variable_35;
matrix homalg_variable_38 = SyzygiesGeneratorsOfColumns(homalg_variable_37);
IsZeroMatrix(homalg_variable_38);
nrows(homalg_variable_38);
GetRowIndependentUnitPositions(homalg_variable_38, list ( 0 ));
matrix homalg_variable_40 = submat(homalg_variable_37,intvec( 1, 3, 4, 5 ),1..2);
matrix homalg_variable_39 = SyzygiesGeneratorsOfColumns(homalg_variable_40);
IsZeroMatrix(homalg_variable_39);
nrows(homalg_variable_39);
GetRowIndependentUnitPositions(homalg_variable_39, list ( 0 ));
matrix homalg_variable_41 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_40);
IsZeroMatrix(homalg_variable_41);
nrows(homalg_variable_41);
matrix homalg_variable_42 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_41);
IsZeroMatrix(homalg_variable_42);
nrows(homalg_variable_42);
matrix homalg_variable_43 = SyzygiesGeneratorsOfColumns(homalg_variable_16);
IsZeroMatrix(homalg_variable_43);
nrows(homalg_variable_43);
GetRowIndependentUnitPositions(homalg_variable_43, list ( 0 ));
matrix homalg_variable_44 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_16);
IsZeroMatrix(homalg_variable_44);
nrows(homalg_variable_44);
matrix homalg_variable_45 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_44);
IsZeroMatrix(homalg_variable_45);
nrows(homalg_variable_45);
ZeroColumns(homalg_variable_4);
matrix homalg_variable_46 = BasisOfColumnModule(homalg_variable_10);
nrows(homalg_variable_46);
homalg_variable_46==homalg_variable_10;
matrix homalg_variable_47 = SyzygiesGeneratorsOfColumns(homalg_variable_46);
IsZeroMatrix(homalg_variable_47);
matrix homalg_variable_49[7][5] = homalg_variable_21,homalg_variable_9;
matrix homalg_variable_48 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_49);
IsZeroMatrix(homalg_variable_48);
nrows(homalg_variable_48);
ZeroColumns(homalg_variable_48);
ZeroColumns(homalg_variable_46);
matrix homalg_variable_50 = submat(homalg_variable_48,1..2,intvec( 3, 4, 5, 6, 7 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_50,homalg_variable_46); matrix homalg_variable_51 = homalg_variable_l[1]; matrix homalg_variable_52 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_51);
matrix homalg_variable_53 = homalg_variable_52*(-1);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_53); matrix homalg_variable_54 = homalg_variable_l[1]; matrix homalg_variable_55 = homalg_variable_l[2];
nrows(homalg_variable_54);
IsIdentityMatrix(homalg_variable_54);
ncols(homalg_variable_55);
matrix homalg_variable_57 = homalg_variable_55*homalg_variable_48;
matrix homalg_variable_56 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_57);
IsZeroMatrix(homalg_variable_56);
ZeroColumns(homalg_variable_22);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_22); matrix homalg_variable_58 = homalg_variable_l[1]; matrix homalg_variable_59 = homalg_variable_l[2];
nrows(homalg_variable_58);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_4,homalg_variable_58); matrix homalg_variable_60 = homalg_variable_l[1]; matrix homalg_variable_61 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_60);
ncols(homalg_variable_59);
matrix homalg_variable_62[9][6] = homalg_variable_24,homalg_variable_23;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_62); matrix homalg_variable_63 = homalg_variable_l[1]; matrix homalg_variable_64 = homalg_variable_l[2];
nrows(homalg_variable_63);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_24,homalg_variable_63); matrix homalg_variable_65 = homalg_variable_l[1]; matrix homalg_variable_66 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_65);
ncols(homalg_variable_64);
matrix homalg_variable_68 = homalg_variable_66*(-1);
matrix homalg_variable_69 = submat(homalg_variable_64,1..4,intvec( 1, 2, 3, 4 ));
matrix homalg_variable_70 = homalg_variable_68*homalg_variable_69;
matrix homalg_variable_67 = DecideZeroColumns(homalg_variable_70,homalg_variable_30);
IsZeroMatrix(homalg_variable_67);
matrix homalg_variable_71[11][4] = homalg_variable_67,homalg_variable_30;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_71); matrix homalg_variable_72 = homalg_variable_l[1]; matrix homalg_variable_73 = homalg_variable_l[2];
nrows(homalg_variable_72);
matrix homalg_variable_75[4][4] = unitmat(4);
matrix homalg_variable_74 = DecideZeroColumns(homalg_variable_75,homalg_variable_30);
IsZeroMatrix(homalg_variable_74);
IsIdentityMatrix(homalg_variable_72);
ncols(homalg_variable_73);
matrix homalg_variable_76 = SyzygiesGeneratorsOfColumns(homalg_variable_22);
IsZeroMatrix(homalg_variable_76);
nrows(homalg_variable_76);
matrix homalg_variable_78 = homalg_variable_61*(-1);
matrix homalg_variable_79 = homalg_variable_78*homalg_variable_59;
matrix homalg_variable_80 = submat(homalg_variable_73,1..4,intvec( 1, 2, 3, 4 ));
matrix homalg_variable_81 = homalg_variable_74*homalg_variable_80;
matrix homalg_variable_82[9][4] = homalg_variable_79,homalg_variable_81;
matrix homalg_variable_77 = RelativeSyzygiesGeneratorsOfColumns(homalg_variable_82,homalg_variable_76);
IsZeroMatrix(homalg_variable_77);
nrows(homalg_variable_77);
matrix homalg_variable_83 = SyzygiesGeneratorsOfColumns(homalg_variable_77);
IsZeroMatrix(homalg_variable_83);
nrows(homalg_variable_83);
GetRowIndependentUnitPositions(homalg_variable_83, list ( 0 ));
matrix homalg_variable_85 = submat(homalg_variable_77,intvec( 1, 3, 5, 6, 7, 8 ),1..9);
matrix homalg_variable_84 = SyzygiesGeneratorsOfColumns(homalg_variable_85);
IsZeroMatrix(homalg_variable_84);
ZeroColumns(homalg_variable_85);
ZeroColumns(homalg_variable_33);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_33); matrix homalg_variable_86 = homalg_variable_l[1]; matrix homalg_variable_87 = homalg_variable_l[2];
nrows(homalg_variable_86);
matrix homalg_variable_88 = submat(homalg_variable_85,1..6,intvec( 6, 7, 8, 9 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_88,homalg_variable_86); matrix homalg_variable_89 = homalg_variable_l[1]; matrix homalg_variable_90 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_89);
ncols(homalg_variable_87);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_85); matrix homalg_variable_91 = homalg_variable_l[1]; matrix homalg_variable_92 = homalg_variable_l[2];
nrows(homalg_variable_91);
IsIdentityMatrix(homalg_variable_91);
matrix homalg_variable_93[2][4] = 0;
matrix homalg_variable_94[2][9] = concat(homalg_variable_46,homalg_variable_93);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_94,homalg_variable_91); matrix homalg_variable_95 = homalg_variable_l[1]; matrix homalg_variable_96 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_95);
ncols(homalg_variable_92);
matrix homalg_variable_97 = BasisOfColumnModule(homalg_variable_32);
nrows(homalg_variable_97);
homalg_variable_97==homalg_variable_32;
matrix homalg_variable_99 = homalg_variable_90*(-1);
matrix homalg_variable_100 = homalg_variable_99*homalg_variable_87;
matrix homalg_variable_98 = DecideZeroColumns(homalg_variable_100,homalg_variable_97);
IsZeroMatrix(homalg_variable_98);
matrix homalg_variable_101[7][5] = homalg_variable_98,homalg_variable_97;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_101); matrix homalg_variable_102 = homalg_variable_l[1]; matrix homalg_variable_103 = homalg_variable_l[2];
nrows(homalg_variable_102);
matrix homalg_variable_104 = DecideZeroColumns(homalg_variable_9,homalg_variable_97);
IsZeroMatrix(homalg_variable_104);
IsIdentityMatrix(homalg_variable_102);
ncols(homalg_variable_103);
matrix homalg_variable_106 = homalg_variable_96*(-1);
matrix homalg_variable_107 = homalg_variable_106*homalg_variable_92;
matrix homalg_variable_108 = homalg_variable_107*homalg_variable_85;
matrix homalg_variable_109 = submat(homalg_variable_103,1..5,intvec( 1, 2, 3, 4, 5, 6 ));
matrix homalg_variable_110 = homalg_variable_104*homalg_variable_109;
matrix homalg_variable_111 = homalg_variable_110*homalg_variable_85;
matrix homalg_variable_112[7][9] = homalg_variable_108,homalg_variable_111;
matrix homalg_variable_105 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_112);
IsZeroMatrix(homalg_variable_105);
nrows(homalg_variable_105);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_34); matrix homalg_variable_113 = homalg_variable_l[1]; matrix homalg_variable_114 = homalg_variable_l[2];
nrows(homalg_variable_113);
matrix homalg_variable_115 = submat(homalg_variable_105,1..1,intvec( 3, 4, 5, 6, 7 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_115,homalg_variable_113); matrix homalg_variable_116 = homalg_variable_l[1]; matrix homalg_variable_117 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_116);
ncols(homalg_variable_114);
matrix homalg_variable_118 = homalg_variable_117*(-1);
matrix homalg_variable_119 = homalg_variable_118*homalg_variable_114;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_119); matrix homalg_variable_120 = homalg_variable_l[1]; matrix homalg_variable_121 = homalg_variable_l[2];
nrows(homalg_variable_120);
IsIdentityMatrix(homalg_variable_120);
ncols(homalg_variable_121);
matrix homalg_variable_123 = homalg_variable_121*homalg_variable_105;
matrix homalg_variable_122 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_123);
IsZeroMatrix(homalg_variable_122);
ZeroColumns(homalg_variable_14);
matrix homalg_variable_124 = submat(homalg_variable_14,intvec( 1, 3, 4, 5, 6 ),1..4);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_124); matrix homalg_variable_125 = homalg_variable_l[1]; matrix homalg_variable_126 = homalg_variable_l[2];
nrows(homalg_variable_125);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_14,homalg_variable_125); matrix homalg_variable_127 = homalg_variable_l[1]; matrix homalg_variable_128 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_127);
ncols(homalg_variable_126);
matrix homalg_variable_129 = SyzygiesGeneratorsOfColumns(homalg_variable_124);
IsZeroMatrix(homalg_variable_129);
nrows(homalg_variable_129);
matrix homalg_variable_130 = BasisOfColumnModule(homalg_variable_129);
nrows(homalg_variable_130);
homalg_variable_130==homalg_variable_129;
matrix homalg_variable_131 = SyzygiesGeneratorsOfColumns(homalg_variable_130);
IsZeroMatrix(homalg_variable_131);
nrows(homalg_variable_131);
GetRowIndependentUnitPositions(homalg_variable_131, list ( 0 ));
matrix homalg_variable_132 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_130);
IsZeroMatrix(homalg_variable_132);
nrows(homalg_variable_132);
matrix homalg_variable_134 = homalg_variable_128*(-1);
matrix homalg_variable_135 = homalg_variable_134*homalg_variable_126;
matrix homalg_variable_133 = DecideZeroColumns(homalg_variable_135,homalg_variable_130);
IsZeroMatrix(homalg_variable_133);
matrix homalg_variable_136[9][5] = homalg_variable_133,homalg_variable_130;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_136); matrix homalg_variable_137 = homalg_variable_l[1]; matrix homalg_variable_138 = homalg_variable_l[2];
nrows(homalg_variable_137);
matrix homalg_variable_139 = DecideZeroColumns(homalg_variable_9,homalg_variable_130);
IsZeroMatrix(homalg_variable_139);
IsIdentityMatrix(homalg_variable_137);
ncols(homalg_variable_138);
matrix homalg_variable_141 = homalg_variable_79*homalg_variable_22;
matrix homalg_variable_142 = homalg_variable_81*homalg_variable_22;
matrix homalg_variable_143[9][6] = homalg_variable_141,homalg_variable_142;
matrix homalg_variable_144 = submat(homalg_variable_138,1..5,intvec( 1, 2, 3, 4, 5, 6 ));
matrix homalg_variable_145 = homalg_variable_139*homalg_variable_144;
matrix homalg_variable_146[14][6] = homalg_variable_143,homalg_variable_145;
matrix homalg_variable_140 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_146);
IsZeroMatrix(homalg_variable_140);
nrows(homalg_variable_140);
ZeroColumns(homalg_variable_140);
ZeroColumns(homalg_variable_130);
IsZeroMatrix(homalg_variable_108);
ZeroColumns(homalg_variable_112);
matrix homalg_variable_147 = submat(homalg_variable_140,1..8,intvec( 10, 11, 12, 13, 14 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_147,homalg_variable_130); matrix homalg_variable_148 = homalg_variable_l[1]; matrix homalg_variable_149 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_148);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_140); matrix homalg_variable_150 = homalg_variable_l[1]; matrix homalg_variable_151 = homalg_variable_l[2];
nrows(homalg_variable_150);
matrix homalg_variable_152[2][5] = 0;
matrix homalg_variable_153[2][14] = concat(homalg_variable_108,homalg_variable_152);
matrix homalg_variable_154[5][5] = 0;
matrix homalg_variable_155[5][14] = concat(homalg_variable_111,homalg_variable_154);
matrix homalg_variable_156[7][14] = homalg_variable_153,homalg_variable_155;
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_156,homalg_variable_150); matrix homalg_variable_157 = homalg_variable_l[1]; matrix homalg_variable_158 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_157);
ncols(homalg_variable_151);
matrix homalg_variable_159 = BasisOfColumnModule(homalg_variable_131);
nrows(homalg_variable_159);
homalg_variable_159==homalg_variable_131;
matrix homalg_variable_161 = homalg_variable_149*(-1);
matrix homalg_variable_160 = DecideZeroColumns(homalg_variable_161,homalg_variable_159);
IsZeroMatrix(homalg_variable_160);
matrix homalg_variable_162[9][3] = homalg_variable_160,homalg_variable_159;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_162); matrix homalg_variable_163 = homalg_variable_l[1]; matrix homalg_variable_164 = homalg_variable_l[2];
nrows(homalg_variable_163);
matrix homalg_variable_166[3][3] = unitmat(3);
matrix homalg_variable_165 = DecideZeroColumns(homalg_variable_166,homalg_variable_159);
IsZeroMatrix(homalg_variable_165);
IsIdentityMatrix(homalg_variable_163);
ncols(homalg_variable_164);
matrix homalg_variable_168 = homalg_variable_158*(-1);
matrix homalg_variable_169 = homalg_variable_168*homalg_variable_151;
matrix homalg_variable_170 = homalg_variable_169*homalg_variable_140;
matrix homalg_variable_171 = submat(homalg_variable_164,1..3,intvec( 1, 2, 3, 4, 5, 6, 7, 8 ));
matrix homalg_variable_172 = homalg_variable_165*homalg_variable_171;
matrix homalg_variable_173 = homalg_variable_172*homalg_variable_140;
matrix homalg_variable_174[10][14] = homalg_variable_170,homalg_variable_173;
matrix homalg_variable_167 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_174);
IsZeroMatrix(homalg_variable_167);
nrows(homalg_variable_167);
ZeroColumns(homalg_variable_167);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_132); matrix homalg_variable_175 = homalg_variable_l[1]; matrix homalg_variable_176 = homalg_variable_l[2];
nrows(homalg_variable_175);
matrix homalg_variable_177 = submat(homalg_variable_167,1..2,intvec( 8, 9, 10 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_177,homalg_variable_175); matrix homalg_variable_178 = homalg_variable_l[1]; matrix homalg_variable_179 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_178);
ncols(homalg_variable_176);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_167); matrix homalg_variable_180 = homalg_variable_l[1]; matrix homalg_variable_181 = homalg_variable_l[2];
nrows(homalg_variable_180);
matrix homalg_variable_182[1][3] = 0;
matrix homalg_variable_183[1][10] = concat(homalg_variable_123,homalg_variable_182);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_183,homalg_variable_180); matrix homalg_variable_184 = homalg_variable_l[1]; matrix homalg_variable_185 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_184);
ncols(homalg_variable_181);
matrix homalg_variable_186 = homalg_variable_179*(-1);
matrix homalg_variable_187 = homalg_variable_186*homalg_variable_176;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_187); matrix homalg_variable_188 = homalg_variable_l[1]; matrix homalg_variable_189 = homalg_variable_l[2];
nrows(homalg_variable_188);
IsIdentityMatrix(homalg_variable_188);
ncols(homalg_variable_189);
matrix homalg_variable_191 = homalg_variable_185*(-1);
matrix homalg_variable_192 = homalg_variable_191*homalg_variable_181;
matrix homalg_variable_193 = homalg_variable_192*homalg_variable_167;
matrix homalg_variable_194 = homalg_variable_189*homalg_variable_167;
matrix homalg_variable_195[2][10] = homalg_variable_193,homalg_variable_194;
matrix homalg_variable_190 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_195);
IsZeroMatrix(homalg_variable_190);
ZeroColumns(homalg_variable_25);
IsIdentityMatrix(homalg_variable_25);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_25); matrix homalg_variable_196 = homalg_variable_l[1]; matrix homalg_variable_197 = homalg_variable_l[2];
nrows(homalg_variable_196);
IsIdentityMatrix(homalg_variable_196);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_124,homalg_variable_196); matrix homalg_variable_198 = homalg_variable_l[1]; matrix homalg_variable_199 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_198);
ncols(homalg_variable_197);
matrix homalg_variable_200 = DecideZeroColumns(homalg_variable_36,homalg_variable_26);
IsZeroMatrix(homalg_variable_200);
matrix homalg_variable_201[8][4] = homalg_variable_200,homalg_variable_26;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_201); matrix homalg_variable_202 = homalg_variable_l[1]; matrix homalg_variable_203 = homalg_variable_l[2];
nrows(homalg_variable_202);
IsIdentityMatrix(homalg_variable_202);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_27,homalg_variable_202); matrix homalg_variable_204 = homalg_variable_l[1]; matrix homalg_variable_205 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_204);
ncols(homalg_variable_203);
matrix homalg_variable_207 = homalg_variable_205*(-1);
matrix homalg_variable_208 = submat(homalg_variable_203,1..4,intvec( 1, 2 ));
matrix homalg_variable_209 = homalg_variable_207*homalg_variable_208;
matrix homalg_variable_206 = DecideZeroColumns(homalg_variable_209,homalg_variable_37);
IsZeroMatrix(homalg_variable_206);
matrix homalg_variable_210[9][2] = homalg_variable_206,homalg_variable_37;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_210); matrix homalg_variable_211 = homalg_variable_l[1]; matrix homalg_variable_212 = homalg_variable_l[2];
nrows(homalg_variable_211);
matrix homalg_variable_214[2][2] = unitmat(2);
matrix homalg_variable_213 = DecideZeroColumns(homalg_variable_214,homalg_variable_37);
IsZeroMatrix(homalg_variable_213);
IsIdentityMatrix(homalg_variable_211);
ncols(homalg_variable_212);
matrix homalg_variable_215 = SyzygiesGeneratorsOfColumns(homalg_variable_25);
IsZeroMatrix(homalg_variable_215);
nrows(homalg_variable_215);
matrix homalg_variable_217 = homalg_variable_199*(-1);
matrix homalg_variable_218 = homalg_variable_217*homalg_variable_197;
matrix homalg_variable_219 = submat(homalg_variable_212,1..2,intvec( 1, 2, 3, 4 ));
matrix homalg_variable_220 = homalg_variable_213*homalg_variable_219;
matrix homalg_variable_221[7][4] = homalg_variable_218,homalg_variable_220;
matrix homalg_variable_216 = RelativeSyzygiesGeneratorsOfColumns(homalg_variable_221,homalg_variable_215);
IsZeroMatrix(homalg_variable_216);
nrows(homalg_variable_216);
matrix homalg_variable_222 = SyzygiesGeneratorsOfColumns(homalg_variable_216);
IsZeroMatrix(homalg_variable_222);
nrows(homalg_variable_222);
GetRowIndependentUnitPositions(homalg_variable_222, list ( 0 ));
matrix homalg_variable_224 = submat(homalg_variable_216,intvec( 1, 3, 4, 5 ),1..7);
matrix homalg_variable_223 = SyzygiesGeneratorsOfColumns(homalg_variable_224);
IsZeroMatrix(homalg_variable_223);
ZeroColumns(homalg_variable_224);
ZeroColumns(homalg_variable_40);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_40); matrix homalg_variable_225 = homalg_variable_l[1]; matrix homalg_variable_226 = homalg_variable_l[2];
nrows(homalg_variable_225);
matrix homalg_variable_227 = submat(homalg_variable_224,1..4,intvec( 6, 7 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_227,homalg_variable_225); matrix homalg_variable_228 = homalg_variable_l[1]; matrix homalg_variable_229 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_228);
ncols(homalg_variable_226);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_224); matrix homalg_variable_230 = homalg_variable_l[1]; matrix homalg_variable_231 = homalg_variable_l[2];
nrows(homalg_variable_230);
matrix homalg_variable_232[3][2] = 0;
matrix homalg_variable_233[3][7] = concat(homalg_variable_130,homalg_variable_232);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_233,homalg_variable_230); matrix homalg_variable_234 = homalg_variable_l[1]; matrix homalg_variable_235 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_234);
ncols(homalg_variable_231);
matrix homalg_variable_236 = BasisOfColumnModule(homalg_variable_39);
nrows(homalg_variable_236);
homalg_variable_236==homalg_variable_39;
matrix homalg_variable_238 = homalg_variable_229*(-1);
matrix homalg_variable_239 = homalg_variable_238*homalg_variable_226;
matrix homalg_variable_237 = DecideZeroColumns(homalg_variable_239,homalg_variable_236);
IsZeroMatrix(homalg_variable_237);
matrix homalg_variable_240[7][4] = homalg_variable_237,homalg_variable_236;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_240); matrix homalg_variable_241 = homalg_variable_l[1]; matrix homalg_variable_242 = homalg_variable_l[2];
nrows(homalg_variable_241);
matrix homalg_variable_243 = DecideZeroColumns(homalg_variable_75,homalg_variable_236);
IsZeroMatrix(homalg_variable_243);
IsIdentityMatrix(homalg_variable_241);
ncols(homalg_variable_242);
matrix homalg_variable_245 = homalg_variable_235*(-1);
matrix homalg_variable_246 = homalg_variable_245*homalg_variable_231;
matrix homalg_variable_247 = homalg_variable_246*homalg_variable_224;
matrix homalg_variable_248 = submat(homalg_variable_242,1..4,intvec( 1, 2, 3, 4 ));
matrix homalg_variable_249 = homalg_variable_243*homalg_variable_248;
matrix homalg_variable_250 = homalg_variable_249*homalg_variable_224;
matrix homalg_variable_251[7][7] = homalg_variable_247,homalg_variable_250;
matrix homalg_variable_244 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_251);
IsZeroMatrix(homalg_variable_244);
nrows(homalg_variable_244);
ZeroColumns(homalg_variable_244);
ZeroColumns(homalg_variable_41);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_41); matrix homalg_variable_252 = homalg_variable_l[1]; matrix homalg_variable_253 = homalg_variable_l[2];
nrows(homalg_variable_252);
matrix homalg_variable_254 = submat(homalg_variable_244,1..3,intvec( 4, 5, 6, 7 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_254,homalg_variable_252); matrix homalg_variable_255 = homalg_variable_l[1]; matrix homalg_variable_256 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_255);
ncols(homalg_variable_253);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_244); matrix homalg_variable_257 = homalg_variable_l[1]; matrix homalg_variable_258 = homalg_variable_l[2];
nrows(homalg_variable_257);
matrix homalg_variable_259[1][4] = 0;
matrix homalg_variable_260[1][7] = concat(homalg_variable_132,homalg_variable_259);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_260,homalg_variable_257); matrix homalg_variable_261 = homalg_variable_l[1]; matrix homalg_variable_262 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_261);
ncols(homalg_variable_258);
matrix homalg_variable_263 = SyzygiesGeneratorsOfColumns(homalg_variable_41);
IsZeroMatrix(homalg_variable_263);
nrows(homalg_variable_263);
matrix homalg_variable_264 = BasisOfColumnModule(homalg_variable_263);
nrows(homalg_variable_264);
homalg_variable_264==homalg_variable_263;
matrix homalg_variable_266 = homalg_variable_256*(-1);
matrix homalg_variable_267 = homalg_variable_266*homalg_variable_253;
matrix homalg_variable_265 = DecideZeroColumns(homalg_variable_267,homalg_variable_264);
IsZeroMatrix(homalg_variable_265);
matrix homalg_variable_268[4][3] = homalg_variable_265,homalg_variable_264;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_268); matrix homalg_variable_269 = homalg_variable_l[1]; matrix homalg_variable_270 = homalg_variable_l[2];
nrows(homalg_variable_269);
matrix homalg_variable_271 = DecideZeroColumns(homalg_variable_166,homalg_variable_264);
IsZeroMatrix(homalg_variable_271);
IsIdentityMatrix(homalg_variable_269);
ncols(homalg_variable_270);
matrix homalg_variable_273 = homalg_variable_262*(-1);
matrix homalg_variable_274 = homalg_variable_273*homalg_variable_258;
matrix homalg_variable_275 = homalg_variable_274*homalg_variable_244;
matrix homalg_variable_276 = submat(homalg_variable_270,1..3,intvec( 1, 2, 3 ));
matrix homalg_variable_277 = homalg_variable_271*homalg_variable_276;
matrix homalg_variable_278 = homalg_variable_277*homalg_variable_244;
matrix homalg_variable_279[4][7] = homalg_variable_275,homalg_variable_278;
matrix homalg_variable_272 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_279);
IsZeroMatrix(homalg_variable_272);
nrows(homalg_variable_272);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_42); matrix homalg_variable_280 = homalg_variable_l[1]; matrix homalg_variable_281 = homalg_variable_l[2];
nrows(homalg_variable_280);
matrix homalg_variable_282 = submat(homalg_variable_272,1..1,intvec( 2, 3, 4 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_282,homalg_variable_280); matrix homalg_variable_283 = homalg_variable_l[1]; matrix homalg_variable_284 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_283);
ncols(homalg_variable_281);
matrix homalg_variable_285 = homalg_variable_284*(-1);
matrix homalg_variable_286 = homalg_variable_285*homalg_variable_281;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_286); matrix homalg_variable_287 = homalg_variable_l[1]; matrix homalg_variable_288 = homalg_variable_l[2];
nrows(homalg_variable_287);
IsIdentityMatrix(homalg_variable_287);
ncols(homalg_variable_288);
ZeroColumns(homalg_variable_15);
matrix homalg_variable_289 = submat(homalg_variable_15,intvec( 2, 3, 4 ),1..1);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_289); matrix homalg_variable_290 = homalg_variable_l[1]; matrix homalg_variable_291 = homalg_variable_l[2];
nrows(homalg_variable_290);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_15,homalg_variable_290); matrix homalg_variable_292 = homalg_variable_l[1]; matrix homalg_variable_293 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_292);
ncols(homalg_variable_291);
matrix homalg_variable_294 = SyzygiesGeneratorsOfColumns(homalg_variable_289);
IsZeroMatrix(homalg_variable_294);
nrows(homalg_variable_294);
matrix homalg_variable_295 = BasisOfColumnModule(homalg_variable_294);
nrows(homalg_variable_295);
homalg_variable_295==homalg_variable_294;
matrix homalg_variable_296 = SyzygiesGeneratorsOfColumns(homalg_variable_295);
IsZeroMatrix(homalg_variable_296);
nrows(homalg_variable_296);
GetRowIndependentUnitPositions(homalg_variable_296, list ( 0 ));
matrix homalg_variable_297 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_295);
IsZeroMatrix(homalg_variable_297);
nrows(homalg_variable_297);
matrix homalg_variable_299 = homalg_variable_288*homalg_variable_272;
matrix homalg_variable_298 = SyzygiesGeneratorsOfColumns(homalg_variable_299);
IsZeroMatrix(homalg_variable_298);
matrix homalg_variable_301 = homalg_variable_293*(-1);
matrix homalg_variable_302 = homalg_variable_301*homalg_variable_291;
matrix homalg_variable_300 = DecideZeroColumns(homalg_variable_302,homalg_variable_295);
IsZeroMatrix(homalg_variable_300);
matrix homalg_variable_303[7][3] = homalg_variable_300,homalg_variable_295;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_303); matrix homalg_variable_304 = homalg_variable_l[1]; matrix homalg_variable_305 = homalg_variable_l[2];
nrows(homalg_variable_304);
matrix homalg_variable_306 = DecideZeroColumns(homalg_variable_166,homalg_variable_295);
IsZeroMatrix(homalg_variable_306);
IsIdentityMatrix(homalg_variable_304);
ncols(homalg_variable_305);
matrix homalg_variable_308 = homalg_variable_218*homalg_variable_25;
matrix homalg_variable_309 = homalg_variable_220*homalg_variable_25;
matrix homalg_variable_310[7][4] = homalg_variable_308,homalg_variable_309;
matrix homalg_variable_311 = submat(homalg_variable_305,1..3,intvec( 1, 2, 3, 4 ));
matrix homalg_variable_312 = homalg_variable_306*homalg_variable_311;
matrix homalg_variable_313[10][4] = homalg_variable_310,homalg_variable_312;
matrix homalg_variable_307 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_313);
IsZeroMatrix(homalg_variable_307);
nrows(homalg_variable_307);
ZeroColumns(homalg_variable_307);
ZeroColumns(homalg_variable_295);
IsIdentityMatrix(homalg_variable_295);
IsZeroMatrix(homalg_variable_247);
ZeroColumns(homalg_variable_251);
IsIdentityMatrix(homalg_variable_251);
matrix homalg_variable_314 = submat(homalg_variable_307,1..6,intvec( 8, 9, 10 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_314,homalg_variable_295); matrix homalg_variable_315 = homalg_variable_l[1]; matrix homalg_variable_316 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_315);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_307); matrix homalg_variable_317 = homalg_variable_l[1]; matrix homalg_variable_318 = homalg_variable_l[2];
nrows(homalg_variable_317);
matrix homalg_variable_319[3][3] = 0;
matrix homalg_variable_320[3][10] = concat(homalg_variable_247,homalg_variable_319);
matrix homalg_variable_321[4][3] = 0;
matrix homalg_variable_322[4][10] = concat(homalg_variable_250,homalg_variable_321);
matrix homalg_variable_323[7][10] = homalg_variable_320,homalg_variable_322;
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_323,homalg_variable_317); matrix homalg_variable_324 = homalg_variable_l[1]; matrix homalg_variable_325 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_324);
ncols(homalg_variable_318);
matrix homalg_variable_326 = BasisOfColumnModule(homalg_variable_296);
nrows(homalg_variable_326);
homalg_variable_326==homalg_variable_296;
matrix homalg_variable_328 = homalg_variable_316*(-1);
matrix homalg_variable_327 = DecideZeroColumns(homalg_variable_328,homalg_variable_326);
IsZeroMatrix(homalg_variable_327);
matrix homalg_variable_329[7][3] = homalg_variable_327,homalg_variable_326;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_329); matrix homalg_variable_330 = homalg_variable_l[1]; matrix homalg_variable_331 = homalg_variable_l[2];
nrows(homalg_variable_330);
matrix homalg_variable_332 = DecideZeroColumns(homalg_variable_166,homalg_variable_326);
IsZeroMatrix(homalg_variable_332);
IsIdentityMatrix(homalg_variable_330);
ncols(homalg_variable_331);
matrix homalg_variable_334 = homalg_variable_325*(-1);
matrix homalg_variable_335 = homalg_variable_334*homalg_variable_318;
matrix homalg_variable_336 = homalg_variable_335*homalg_variable_307;
matrix homalg_variable_337 = submat(homalg_variable_331,1..3,intvec( 1, 2, 3, 4, 5, 6 ));
matrix homalg_variable_338 = homalg_variable_332*homalg_variable_337;
matrix homalg_variable_339 = homalg_variable_338*homalg_variable_307;
matrix homalg_variable_340[10][10] = homalg_variable_336,homalg_variable_339;
matrix homalg_variable_333 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_340);
IsZeroMatrix(homalg_variable_333);
nrows(homalg_variable_333);
ZeroColumns(homalg_variable_333);
IsZeroMatrix(homalg_variable_275);
ZeroColumns(homalg_variable_279);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_297); matrix homalg_variable_341 = homalg_variable_l[1]; matrix homalg_variable_342 = homalg_variable_l[2];
nrows(homalg_variable_341);
matrix homalg_variable_343 = submat(homalg_variable_333,1..4,intvec( 8, 9, 10 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_343,homalg_variable_341); matrix homalg_variable_344 = homalg_variable_l[1]; matrix homalg_variable_345 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_344);
ncols(homalg_variable_342);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_333); matrix homalg_variable_346 = homalg_variable_l[1]; matrix homalg_variable_347 = homalg_variable_l[2];
nrows(homalg_variable_346);
matrix homalg_variable_348[1][3] = 0;
matrix homalg_variable_349[1][10] = concat(homalg_variable_275,homalg_variable_348);
matrix homalg_variable_350[3][3] = 0;
matrix homalg_variable_351[3][10] = concat(homalg_variable_278,homalg_variable_350);
matrix homalg_variable_352[4][10] = homalg_variable_349,homalg_variable_351;
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_352,homalg_variable_346); matrix homalg_variable_353 = homalg_variable_l[1]; matrix homalg_variable_354 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_353);
ncols(homalg_variable_347);
matrix homalg_variable_355 = homalg_variable_345*(-1);
matrix homalg_variable_356 = homalg_variable_355*homalg_variable_342;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_356); matrix homalg_variable_357 = homalg_variable_l[1]; matrix homalg_variable_358 = homalg_variable_l[2];
nrows(homalg_variable_357);
IsIdentityMatrix(homalg_variable_357);
ncols(homalg_variable_358);
matrix homalg_variable_360 = homalg_variable_354*(-1);
matrix homalg_variable_361 = homalg_variable_360*homalg_variable_347;
matrix homalg_variable_362 = homalg_variable_361*homalg_variable_333;
matrix homalg_variable_363 = homalg_variable_358*homalg_variable_333;
matrix homalg_variable_364[5][10] = homalg_variable_362,homalg_variable_363;
matrix homalg_variable_359 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_364);
IsZeroMatrix(homalg_variable_359);
nrows(homalg_variable_359);
matrix homalg_variable_365 = submat(homalg_variable_359,1..1,intvec( 5 ));
IsZeroMatrix(homalg_variable_365);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_359); matrix homalg_variable_366 = homalg_variable_l[1]; matrix homalg_variable_367 = homalg_variable_l[2];
nrows(homalg_variable_366);
matrix homalg_variable_368[1][1] = 0;
matrix homalg_variable_369[1][5] = concat(homalg_variable_299,homalg_variable_368);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_369,homalg_variable_366); matrix homalg_variable_370 = homalg_variable_l[1]; matrix homalg_variable_371 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_370);
ncols(homalg_variable_367);
matrix homalg_variable_373[1][1] = unitmat(1);
matrix homalg_variable_374[4][1] = homalg_variable_289,homalg_variable_373;
matrix homalg_variable_372 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_374);
IsZeroMatrix(homalg_variable_372);
nrows(homalg_variable_372);
ZeroColumns(homalg_variable_372);
ZeroColumns(homalg_variable_16);
matrix homalg_variable_375 = submat(homalg_variable_372,1..3,intvec( 4 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_375,homalg_variable_16); matrix homalg_variable_376 = homalg_variable_l[1]; matrix homalg_variable_377 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_376);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_372); matrix homalg_variable_378 = homalg_variable_l[1]; matrix homalg_variable_379 = homalg_variable_l[2];
nrows(homalg_variable_378);
matrix homalg_variable_380[3][1] = 0;
matrix homalg_variable_381[3][4] = concat(homalg_variable_295,homalg_variable_380);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_381,homalg_variable_378); matrix homalg_variable_382 = homalg_variable_l[1]; matrix homalg_variable_383 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_382);
ncols(homalg_variable_379);
matrix homalg_variable_384 = BasisOfColumnModule(homalg_variable_43);
nrows(homalg_variable_384);
homalg_variable_384==homalg_variable_43;
matrix homalg_variable_386 = homalg_variable_377*(-1);
matrix homalg_variable_385 = DecideZeroColumns(homalg_variable_386,homalg_variable_384);
IsZeroMatrix(homalg_variable_385);
matrix homalg_variable_387[6][3] = homalg_variable_385,homalg_variable_384;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_387); matrix homalg_variable_388 = homalg_variable_l[1]; matrix homalg_variable_389 = homalg_variable_l[2];
nrows(homalg_variable_388);
matrix homalg_variable_390 = DecideZeroColumns(homalg_variable_166,homalg_variable_384);
IsZeroMatrix(homalg_variable_390);
IsIdentityMatrix(homalg_variable_388);
ncols(homalg_variable_389);
matrix homalg_variable_392 = homalg_variable_383*(-1);
matrix homalg_variable_393 = homalg_variable_392*homalg_variable_379;
matrix homalg_variable_394 = homalg_variable_393*homalg_variable_372;
matrix homalg_variable_395 = submat(homalg_variable_389,1..3,intvec( 1, 2, 3 ));
matrix homalg_variable_396 = homalg_variable_390*homalg_variable_395;
matrix homalg_variable_397 = homalg_variable_396*homalg_variable_372;
matrix homalg_variable_398[6][4] = homalg_variable_394,homalg_variable_397;
matrix homalg_variable_391 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_398);
IsZeroMatrix(homalg_variable_391);
nrows(homalg_variable_391);
ZeroColumns(homalg_variable_391);
ZeroColumns(homalg_variable_44);
IsIdentityMatrix(homalg_variable_44);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_44); matrix homalg_variable_399 = homalg_variable_l[1]; matrix homalg_variable_400 = homalg_variable_l[2];
nrows(homalg_variable_399);
IsIdentityMatrix(homalg_variable_399);
matrix homalg_variable_401 = submat(homalg_variable_391,1..3,intvec( 4, 5, 6 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_401,homalg_variable_399); matrix homalg_variable_402 = homalg_variable_l[1]; matrix homalg_variable_403 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_402);
ncols(homalg_variable_400);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_391); matrix homalg_variable_404 = homalg_variable_l[1]; matrix homalg_variable_405 = homalg_variable_l[2];
nrows(homalg_variable_404);
matrix homalg_variable_406[1][3] = 0;
matrix homalg_variable_407[1][6] = concat(homalg_variable_297,homalg_variable_406);
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_407,homalg_variable_404); matrix homalg_variable_408 = homalg_variable_l[1]; matrix homalg_variable_409 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_408);
ncols(homalg_variable_405);
matrix homalg_variable_410 = SyzygiesGeneratorsOfColumns(homalg_variable_44);
IsZeroMatrix(homalg_variable_410);
nrows(homalg_variable_410);
matrix homalg_variable_411 = BasisOfColumnModule(homalg_variable_410);
nrows(homalg_variable_411);
homalg_variable_411==homalg_variable_410;
matrix homalg_variable_413 = homalg_variable_403*(-1);
matrix homalg_variable_414 = homalg_variable_413*homalg_variable_400;
matrix homalg_variable_412 = DecideZeroColumns(homalg_variable_414,homalg_variable_411);
IsZeroMatrix(homalg_variable_412);
matrix homalg_variable_415[4][3] = homalg_variable_412,homalg_variable_411;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_415); matrix homalg_variable_416 = homalg_variable_l[1]; matrix homalg_variable_417 = homalg_variable_l[2];
nrows(homalg_variable_416);
matrix homalg_variable_418 = DecideZeroColumns(homalg_variable_166,homalg_variable_411);
IsZeroMatrix(homalg_variable_418);
IsIdentityMatrix(homalg_variable_416);
ncols(homalg_variable_417);
matrix homalg_variable_420 = homalg_variable_409*(-1);
matrix homalg_variable_421 = homalg_variable_420*homalg_variable_405;
matrix homalg_variable_422 = homalg_variable_421*homalg_variable_391;
matrix homalg_variable_423 = submat(homalg_variable_417,1..3,intvec( 1, 2, 3 ));
matrix homalg_variable_424 = homalg_variable_418*homalg_variable_423;
matrix homalg_variable_425 = homalg_variable_424*homalg_variable_391;
matrix homalg_variable_426[4][6] = homalg_variable_422,homalg_variable_425;
matrix homalg_variable_419 = ReducedSyzygiesGeneratorsOfColumns(homalg_variable_426);
IsZeroMatrix(homalg_variable_419);
nrows(homalg_variable_419);
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_45); matrix homalg_variable_427 = homalg_variable_l[1]; matrix homalg_variable_428 = homalg_variable_l[2];
nrows(homalg_variable_427);
matrix homalg_variable_429 = submat(homalg_variable_419,1..1,intvec( 2, 3, 4 ));
list homalg_variable_l=DecideZeroColumnsEffectively(homalg_variable_429,homalg_variable_427); matrix homalg_variable_430 = homalg_variable_l[1]; matrix homalg_variable_431 = homalg_variable_l[2];
IsZeroMatrix(homalg_variable_430);
ncols(homalg_variable_428);
matrix homalg_variable_432 = homalg_variable_431*(-1);
matrix homalg_variable_433 = homalg_variable_432*homalg_variable_428;
list homalg_variable_l=BasisOfColumnsCoeff(homalg_variable_433); matrix homalg_variable_434 = homalg_variable_l[1]; matrix homalg_variable_435 = homalg_variable_l[2];
nrows(homalg_variable_434);
IsIdentityMatrix(homalg_variable_434);
ncols(homalg_variable_435);
matrix homalg_variable_436 = SyzygiesGeneratorsOfColumns(homalg_variable_6);
IsZeroMatrix(homalg_variable_436);
nrows(homalg_variable_436);
GetRowIndependentUnitPositions(homalg_variable_436, list ( 0 ));
matrix homalg_variable_438 = Involution(homalg_variable_6);
matrix homalg_variable_437 = BasisOfRowModule(homalg_variable_438);
ncols(homalg_variable_437);
matrix homalg_variable_439[70][84] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_440[70][84] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_439==homalg_variable_440;
matrix homalg_variable_442[70][70] = unitmat(70);
matrix homalg_variable_441 = DecideZeroRows(homalg_variable_442,homalg_variable_439);
IsZeroMatrix(homalg_variable_441);
ZeroRows(homalg_variable_441);
IsIdentityMatrix(homalg_variable_441);
matrix homalg_variable_443[50][60] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_444[50][60] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_443==homalg_variable_444;
matrix homalg_variable_446[50][50] = unitmat(50);
matrix homalg_variable_445 = DecideZeroRows(homalg_variable_446,homalg_variable_443);
IsZeroMatrix(homalg_variable_445);
ZeroRows(homalg_variable_445);
IsIdentityMatrix(homalg_variable_445);
matrix homalg_variable_447[10][12] = dsum(homalg_variable_437,homalg_variable_437);
matrix homalg_variable_448[10][12] = dsum(homalg_variable_438,homalg_variable_438);
homalg_variable_447==homalg_variable_448;
matrix homalg_variable_450[10][10] = unitmat(10);
matrix homalg_variable_449 = DecideZeroRows(homalg_variable_450,homalg_variable_447);
IsZeroMatrix(homalg_variable_449);
ZeroRows(homalg_variable_449);
IsIdentityMatrix(homalg_variable_449);
matrix homalg_variable_451[35][42] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_452[35][42] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_451==homalg_variable_452;
matrix homalg_variable_454[35][35] = unitmat(35);
matrix homalg_variable_453 = DecideZeroRows(homalg_variable_454,homalg_variable_451);
IsZeroMatrix(homalg_variable_453);
ZeroRows(homalg_variable_453);
IsIdentityMatrix(homalg_variable_453);
matrix homalg_variable_455[10][12] = dsum(homalg_variable_437,homalg_variable_437);
matrix homalg_variable_456[10][12] = dsum(homalg_variable_438,homalg_variable_438);
homalg_variable_455==homalg_variable_456;
matrix homalg_variable_457 = DecideZeroRows(homalg_variable_450,homalg_variable_455);
IsZeroMatrix(homalg_variable_457);
ZeroRows(homalg_variable_457);
IsIdentityMatrix(homalg_variable_457);
matrix homalg_variable_458[50][60] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_459[50][60] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_458==homalg_variable_459;
matrix homalg_variable_460 = DecideZeroRows(homalg_variable_446,homalg_variable_458);
IsZeroMatrix(homalg_variable_460);
ZeroRows(homalg_variable_460);
IsIdentityMatrix(homalg_variable_460);
matrix homalg_variable_461[50][60] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_462[50][60] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_461==homalg_variable_462;
matrix homalg_variable_463 = DecideZeroRows(homalg_variable_446,homalg_variable_461);
IsZeroMatrix(homalg_variable_463);
ZeroRows(homalg_variable_463);
IsIdentityMatrix(homalg_variable_463);
matrix homalg_variable_464[25][30] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_465[25][30] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_464==homalg_variable_465;
matrix homalg_variable_467[25][25] = unitmat(25);
matrix homalg_variable_466 = DecideZeroRows(homalg_variable_467,homalg_variable_464);
IsZeroMatrix(homalg_variable_466);
ZeroRows(homalg_variable_466);
IsIdentityMatrix(homalg_variable_466);
homalg_variable_437==homalg_variable_438;
matrix homalg_variable_468 = DecideZeroRows(homalg_variable_9,homalg_variable_437);
IsZeroMatrix(homalg_variable_468);
ZeroRows(homalg_variable_468);
IsIdentityMatrix(homalg_variable_468);
matrix homalg_variable_469[20][24] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_470[20][24] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_469==homalg_variable_470;
matrix homalg_variable_472[20][20] = unitmat(20);
matrix homalg_variable_471 = DecideZeroRows(homalg_variable_472,homalg_variable_469);
IsZeroMatrix(homalg_variable_471);
ZeroRows(homalg_variable_471);
IsIdentityMatrix(homalg_variable_471);
matrix homalg_variable_473[30][36] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_474[30][36] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_473==homalg_variable_474;
matrix homalg_variable_476[30][30] = unitmat(30);
matrix homalg_variable_475 = DecideZeroRows(homalg_variable_476,homalg_variable_473);
IsZeroMatrix(homalg_variable_475);
ZeroRows(homalg_variable_475);
IsIdentityMatrix(homalg_variable_475);
matrix homalg_variable_477[20][24] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_478[20][24] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_477==homalg_variable_478;
matrix homalg_variable_479 = DecideZeroRows(homalg_variable_472,homalg_variable_477);
IsZeroMatrix(homalg_variable_479);
ZeroRows(homalg_variable_479);
IsIdentityMatrix(homalg_variable_479);
matrix homalg_variable_480 = DecideZeroRows(homalg_variable_9,homalg_variable_437);
IsZeroMatrix(homalg_variable_480);
ZeroRows(homalg_variable_480);
IsIdentityMatrix(homalg_variable_480);
matrix homalg_variable_481[30][36] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_482[30][36] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_481==homalg_variable_482;
matrix homalg_variable_483 = DecideZeroRows(homalg_variable_476,homalg_variable_481);
IsZeroMatrix(homalg_variable_483);
ZeroRows(homalg_variable_483);
IsIdentityMatrix(homalg_variable_483);
matrix homalg_variable_484[25][30] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_485[25][30] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_484==homalg_variable_485;
matrix homalg_variable_486 = DecideZeroRows(homalg_variable_467,homalg_variable_484);
IsZeroMatrix(homalg_variable_486);
ZeroRows(homalg_variable_486);
IsIdentityMatrix(homalg_variable_486);
matrix homalg_variable_487[20][24] = dsum(homalg_variable_437,homalg_variable_437,homalg_variable_437,homalg_variable_437);
matrix homalg_variable_488[20][24] = dsum(homalg_variable_438,homalg_variable_438,homalg_variable_438,homalg_variable_438);
homalg_variable_487==homalg_variable_488;
matrix homalg_variable_489 = DecideZeroRows(homalg_variable_472,homalg_variable_487);
IsZeroMatrix(homalg_variable_489);
ZeroRows(homalg_variable_489);
IsIdentityMatrix(homalg_variable_489);
matrix homalg_variable_490 = DecideZeroRows(homalg_variable_9,homalg_variable_437);
IsZeroMatrix(homalg_variable_490);
ZeroRows(homalg_variable_490);
IsIdentityMatrix(homalg_variable_490);
matrix homalg_variable_492[25][30] = tensor(homalg_variable_4,homalg_variable_9);
matrix homalg_variable_491 = RelativeSyzygiesGeneratorsOfRows(homalg_variable_492,homalg_variable_484);

homalg_variable_491;

tst_status(1);$
