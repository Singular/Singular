#include <gfanlib/gfanlib.h>

#include <libpolys/coeffs/bigintmat.h>
#include <libpolys/coeffs/longrat.h>
#include <libpolys/coeffs/numbers.h>

#include <Singular/ipid.h> // for coeffs_BIGINT

number integerToNumber(const gfan::Integer &I)
{
  mpz_t i;
  mpz_init(i);
  I.setGmp(i);
  long m = 268435456;
  if(mpz_cmp_si(i,m))
  {
    int temp = (int) mpz_get_si(i);
    return n_Init(temp,coeffs_BIGINT);
  }
  else
    return n_InitMPZ(i,coeffs_BIGINT);
}

bigintmat* zVectorToBigintmat(const gfan::ZVector &zv)
{
  int d=zv.size();
  bigintmat* bim = new bigintmat(1,d,coeffs_BIGINT);
  for(int i=1;i<=d;i++)
  {
    number temp = integerToNumber(zv[i-1]);
    bim->set(1,i,temp);
    n_Delete(&temp,coeffs_BIGINT);
  }
  return bim;
}

bigintmat* zMatrixToBigintmat(const gfan::ZMatrix &zm)
{
  int d=zm.getHeight();
  int n=zm.getWidth();
  bigintmat* bim = new bigintmat(d,n,coeffs_BIGINT);
  for(int i=1;i<=d;i++)
    for(int j=1; j<=n; j++)
    {
      number temp = integerToNumber(zm[i-1][j-1]);
      bim->set(i,j,temp);
      n_Delete(&temp,coeffs_BIGINT);
    }
  return bim;
}

gfan::Integer* numberToInteger(const number &n)
{
  if (SR_HDL(n) & SR_INT)
    return new gfan::Integer(SR_TO_INT(n));
  else
    return new gfan::Integer(n->z);
}

gfan::ZMatrix* bigintmatToZMatrix(const bigintmat &bim)
{
  int d=bim.rows();
  int n=bim.cols();
  gfan::ZMatrix* zm = new gfan::ZMatrix(d,n);
  for(int i=0;i<d;i++)
    for(int j=0;j<n;j++)
    {
      number temp = BIMATELEM(bim, i+1, j+1);
      gfan::Integer* gi = numberToInteger(temp);
      (*zm)[i][j] = *gi;
      delete gi;
    }
  return zm;
}

gfan::ZVector* bigintmatToZVector(const bigintmat &bim)
{
  gfan::ZVector* zv=new gfan::ZVector(bim.cols());
  for(int j=0; j<bim.cols(); j++)
  {
    number temp = BIMATELEM(bim, 1, j+1);
    gfan::Integer* gi = numberToInteger(temp);
    (*zv)[j] = *gi;
    n_Delete(&temp,coeffs_BIGINT);
    delete gi;
  }
  return zv;
}

gfan::ZVector intStar2ZVector(const int d, const int* i)
{
  gfan::ZVector zv(d);
  for(int j=0; j<d; j++)
    zv[j]=i[j+1];
  return zv;
}

gfan::ZVector wvhdlEntryToZVector(const int n, const int* wvhdl0)
{
  gfan::ZVector zv(n);
  for(int j=0; j<n; j++)
    zv[j]=wvhdl0[j];
  return zv;
}

int* ZVectorToIntStar(const gfan::ZVector &v, bool &overflow)
{
  int* w = (int*) omAlloc(v.size()*sizeof(int));
  for (unsigned i=0; i<v.size(); i++)
  {
    if (!v[i].fitsInInt())
    {
      omFree(w);
      WerrorS("intoverflow converting gfan:ZVector to int*");
      overflow = true;
      return NULL;
    }
    w[i]=v[i].toInt();
  }
  return w;
}

char* toString(gfan::ZMatrix const &zm)
{
  bigintmat* bim = zMatrixToBigintmat(zm);
  char* s = bim->StringAsPrinted();
  if (s==NULL)
    s = (char*) omAlloc0(sizeof(char));
  delete bim;
  return s;
}

gfan::ZFan* toFanStar(std::set<gfan::ZCone> setOfCones)
{
  if (setOfCones.size() > 0)
  {
    std::set<gfan::ZCone>::iterator cone = setOfCones.begin();
    gfan::ZFan* zf = new gfan::ZFan(cone->ambientDimension());
    for (std::set<gfan::ZCone>::iterator cone = setOfCones.begin(); cone!=setOfCones.end(); cone++)
      zf->insert(*cone);
    return zf;
  }
  else
    return new gfan::ZFan(gfan::ZFan::fullFan(currRing->N));
}

std::set<gfan::ZVector> rays(std::set<gfan::ZCone> setOfCones)
{
  std::set<gfan::ZVector> setOfRays;
  for (std::set<gfan::ZCone>::iterator cone = setOfCones.begin(); cone!=setOfCones.end(); cone++)
  {
    gfan::ZMatrix raysOfCone = cone->extremeRays();
    for (int i=0; i<raysOfCone.getHeight(); i++)
      setOfRays.insert(raysOfCone[i]);
  }
  return setOfRays;
}
