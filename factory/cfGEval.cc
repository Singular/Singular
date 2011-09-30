#include "config.h"

#include "cfGEval.h"
#include "cf_generator.h"
#include "cf_assert.h"

GEvaluation::GEvaluation( const GEvaluation & e ):Evaluation()
{
    if ( e.gen == 0 )
      gen = 0;
    else
    {
      for (int i= 0; i < values.max() - values.min() + 1; i++)
        delete gen[i];
      delete [] gen;

      gen= new CFGenerator* [e.values.max() - e.values.min() + 1];
      for (int i= 0; i < e.values.max() - e.values.min() + 1; i++)
        gen[i] = e.gen[i]->clone();
    }
    values = e.values;
}

GEvaluation::GEvaluation (int min0, int max0, const CFGenerator & sample): Evaluation (min0, max0)
{
  gen= new CFGenerator* [max0 - min0 + 1];
  for (int i= 0; i < max0 - min0 + 1; i++)
    gen[i]= sample.clone();
}

GEvaluation::~GEvaluation()
{
    if ( gen != 0 )
    {
      for (int i= 0; i < values.max() - values.max() + 1; i++)
        delete gen[i];
      delete [] gen;
    }
}

GEvaluation&
GEvaluation::operator= ( const GEvaluation & e )
{
  if ( this != &e )
  {
    if (gen!=0)
    {
      for (int i= 0; i < values.max() - values.min() + 1; i++)
        delete gen[i];
      delete [] gen;
    }
    if ( e.gen == 0 )
      gen = 0;
    else
    {
      gen= new CFGenerator* [e.values.max() - e.values.min() + 1];
      for (int i= 0; i < e.values.max() - e.values.min() + 1; i++)
        gen[i] = e.gen[i]->clone();
    }
    values = e.values;
  }
  return *this;
}

void
GEvaluation::reset()
{
  int t= values.max();
  int u= values.min();
  for (int i= u; i <= t; i++)
  {
    values[i]= 0;
    gen[i - u]->reset();
  }
}

void
GEvaluation::init (int n)
{
  #ifndef NOASSERT
  int t= values.max();
  #endif
  int u= values.min();
  ASSERT (n <= t - u + 1, "wrong number of evaluation points");
  for (int i= u; i <= u + n - 1; i++)
  {
    gen[i - u]->next();
    values[i]= gen[i - u]->item();
  }
}

bool
GEvaluation::nextpoint (int n) // n= number of non zero components
{
  int t= values.max();
  int u= values.min();
  ASSERT (n <= t - u + 1, "wrong number of evaluation points");
  if (t - u + 1 < n) return false;
  for (int i= u; i <= t; i++)
  {
    if (!values[i].isZero())
    {
      if (n == 1)
      {
        if (gen[i - u]->hasItems())
        {
          gen[i - u]->next();
          values[i]= gen[i - u]->item();
          return true;
        }
        else if (!gen [i - u]->hasItems() && i + 1 <= t)
        {
          gen[i - u]->reset();
          gen[i + 1 - u]->next();
          values[i + 1]= gen [i + 1 - u]->item();
          values[i]= 0;
          return true;
        }
        else return false;
      }
      int count= 1;
      int j= i + 1;
      while (j <= t && count < n)
      {
        if (!values [j].isZero())
          count++;
        j++;
      }
      if (j > t)
      {
        if (gen[j - 1 - u]->hasItems())
        {
          gen[j - 1 - u]->next();
          values[j - 1]= gen[j - 1 - u]->item();
          return true;
        }
        int count2= 0;
        for (int k= j - 2; k >= i; k--)
        {
          if (!values[k].isZero() && !values[k + 1].isZero())
            count2++;
          if (!values[k].isZero() && !gen[k - u]->hasItems() &&
              values[k + 1].isZero())
          {
            values[k]= 0;
            gen[k - u]->reset();
            gen[k - u + 1]->next();
            values[k + 1]= gen[k - u + 1]->item();
            if (count2 == 0)
            {
              gen[k - u + 2]->reset();
              gen[k - u + 2]->next();
              values[k + 2]= gen[k - u + 2]->item();
            }
            else
            {
              for (int index= 2; index <= count2 + 2 && k + index <= t; index++)
              {
                gen[k + index - u]-> reset();
                gen[k + index - u]->next();
                values[k + index]= gen[k + index - u]->item();
              }
            }
            for (int index= k + count2 + 3; index <= t; index++)
            {
              gen[index - u]->reset();
              values[index]= 0;
            }
            return true;
          }
          if (!values[k].isZero() && gen[k - u]->hasItems() &&
              values[k + 1].isZero())
          {
            gen[k - u]->next();
            values[k]= gen[k - u]->item();
            gen[k - u + 1]->next();
            values[k + 1]= gen[k - u + 1]->item();
            if (count2 > 0)
            {
              for (int index= 2; index <= count2 + 1 && k + index <= t; index++)
              {
                gen[k + index - u]->reset();
                gen[k + index - u]->next();
                values[k + index]= gen[k + index - u]->item();
              }
            }
            for (int index= k + count2 + 2; index <= t; index++)
            {
              gen[index - u]->reset();
              values[index]= 0;
            }
            return true;
          }
          if (!values[k].isZero() && gen [k - u]->hasItems())
          {
            gen[k - u]->next();
            values[k]= gen[k - u]->item();
            gen[k - u + 1]->reset();
            gen[k - u + 1]->next();
            values[k + 1]= gen[k - u + 1]->item();
            if (count2 > 0)
            {
              for (int index= 2; index <= count2 + 1 && k + index <= t; index++)
              {
                gen[k + index - u]->reset();
                gen[k + index - u]->next();
                values[k + index]= gen[k + index - u]->item();
              }
            }
            for (int index= k + count2 + 2; index <= t; index++)
            {
              gen[index - u]->reset();
              values[index]= 0;
            }
            return true;
          }
        }
      }
      else
      {
        if (gen[j - u - 1]->hasItems())
        {
          gen[j - 1 - u]->next();
          values[j - 1]= gen[j - u - 1]->item();
        }
        else
        {
          gen[j - u - 1]->reset();
          gen[j - u]->next();
          values[j]= gen[j - u]->item();
          values[j - 1]= 0;
        }
        return true;
      }
    }
  }
  // should never reach this
  return false;
}

