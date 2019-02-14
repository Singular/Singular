#include "kernel/mod2.h"
#include "Singular/blackbox.h"
#include "Singular/dyn_modules/interval/interval.h"
#include "Singular/ipshell.h" // for iiCheckTypes
#include "Singular/links/ssiLink.h"
#include "Singular/mod_lib.h"

/*
 * CONSTRUCTORS & DESTRUCTORS
 */

/* interval */

interval::interval(const ring r)
{
  lower = n_Init(0, r->cf);
  upper = n_Init(0, r->cf);
  R = r;
  R->ref++;
}

interval::interval(number a, const ring r)
{
  // dangerous: check if a is in coefs r->cf
  lower = a;
  upper = n_Copy(a, r->cf);
  R = r;
  R->ref++;
}

interval::interval(number a, number b, const ring r)
{
  lower = a;
  upper = b;
  R = r;
  R->ref++;
}

interval::interval(interval *I)
{
  lower = n_Copy(I->lower, I->R->cf);
  upper = n_Copy(I->upper, I->R->cf);
  R = I->R;
  R->ref++;
}

interval::~interval()
{
  n_Delete(&lower, R->cf);
  n_Delete(&upper, R->cf);
  R->ref--;
}

interval& interval::setRing(ring r)
{
  if (R != r)
  {
    // check if coefficient fields are equal
    // if not pass numbers to new cf-field
    if (R->cf != r->cf)
    {
      nMapFunc fun = n_SetMap(R->cf, r->cf);
      number lo = fun(lower, R->cf, r->cf),
             up = fun(upper, R->cf, r->cf);
      n_Delete(&lower, R->cf);
      n_Delete(&upper, R->cf);
      lower = lo;
      upper = up;
    }
    R->ref--;
    r->ref++;
    R = r;
  }

  return *this;
}

/* box */

box::box()
{
  R = currRing;
  int i, n = R->N;
  intervals = (interval**) omAlloc0(n * sizeof(interval*));
  if (intervals != NULL)
  {
    for (i = 0; i < n; i++)
    {
      intervals[i] = new interval();
    }
  }
  R->ref++;
}

box::box(box* B)
{
  R = B->R;
  int i, n = R->N;
  intervals = (interval**) omAlloc0(n * sizeof(interval*));
  if (intervals != NULL)
  {
    for (i = 0; i < n; i++)
    {
      intervals[i] = new interval(B->intervals[i]);
    }
  }
  R->ref++;
}

box::~box()
{
  int i, n = R->N;
  for (i = 0; i < n; i++)
  {
    delete intervals[i];
  }
  omFree((void**) intervals);
  R->ref--;
}

// note: does not copy input
box& box::setInterval(int i, interval *I)
{
  if (0 <= i && i < R->N)
  {
    delete intervals[i];
    intervals[i] = I;
  }
  return *this;
}

/*
 * TYPE IDs
 */

STATIC_VAR int intervalID;
STATIC_VAR int boxID;

/*
 * INTERVAL FUNCTIONS
 */

static void* interval_Init(blackbox*)
{
  return (void*) new interval();
}

// convert interval to string
static char* interval_String(blackbox*, void *d)
{
  if (d == NULL)
  {
    // invalid object
    return omStrDup("[?]");
  }
  else
  {
    interval* i = (interval*) d;

    // use n_Write since nothing better (?) exists
    StringSetS("[");
    n_Write(i->lower, i->R->cf);
    StringAppendS(", ");
    n_Write(i->upper, i->R->cf);
    StringAppendS("]");

    return StringEndS();
  }
}

static void* interval_Copy(blackbox*, void *d)
{
  return (void*) new interval((interval*) d);
}

// destroy interval
static void interval_Destroy(blackbox*, void *d)
{
  if (d != NULL)
    delete (interval*) d;
}

// assigning values to intervals
static BOOLEAN interval_Assign(leftv result, leftv args)
{
  interval *RES;

  /*
   * Allow assignments of the form
   *  I = a,
   *  I = a, b,
   *  I = J
   * where a, b numbers or ints and J interval
   */

  if (args->Typ() == intervalID)
  {
    RES = new interval((interval*) args->CopyD());
  }
  else
  {
    number n1, n2;

    if (args->Typ() == INT_CMD)
    {
      n1 = nInit((int)(long) args->Data());
    }
    else if (args->Typ() == NUMBER_CMD)
    {
      n1 = (number) args->CopyD();
    }
    else
    {
      WerrorS("Input not supported: first argument not int or number");
      return TRUE;
    }

    // check if second argument exists
    if (args->next == NULL)
    {
      RES = new interval(n1);
    }
    else
    {
      if (args->next->Typ() == INT_CMD)
      {
        n2 = nInit((int)(long) args->next->Data());
      }
      else if (args->next->Typ() == NUMBER_CMD)
      {
        n2 = (number) args->next->CopyD();
      }
      else
      {
        WerrorS("Input not supported: second argument not int or number");
        return TRUE;
      }

      RES = new interval(n1, n2);
    }
  }

  // destroy data of result if it exists
  if (result->Data() != NULL)
  {
    delete (interval*) result->Data();
  }

  if (result->rtyp == IDHDL)
  {
    IDDATA((idhdl)result->data) = (char*) RES;
  }
  else
  {
    result->rtyp = intervalID;
    result->data = (void*) RES;
  }

  args->CleanUp();
  return FALSE;
}

static BOOLEAN length(leftv result, leftv arg)
{
  if (arg != NULL && arg->Typ() == intervalID)
  {
    interval *I = (interval*) arg->Data();
    result->rtyp = NUMBER_CMD;
    result->data = (void*) n_Sub(I->upper, I->lower, I->R->cf);
    arg->CleanUp();
    return FALSE;
  }

  WerrorS("syntax: length(<interval>)");
  return TRUE;
}

// interval -> interval procedures

static interval* intervalScalarMultiply(number a, interval *I)
{
  // must assume a is in ring I->R
  number lo, up;
  if (nGreaterZero(a))
  {
    lo = n_Mult(a, I->lower, I->R->cf);
    up = n_Mult(a, I->upper, I->R->cf);
  }
  else
  {
    lo = n_Mult(a, I->upper, I->R->cf);
    up = n_Mult(a, I->lower, I->R->cf);
  }

  n_Normalize(lo, I->R->cf);
  n_Normalize(up, I->R->cf);

  return new interval(lo, up, I->R);
}

static interval* intervalMultiply(interval *I, interval *J)
{
  number lo, up;
  number nums[4];
  nums[0] = n_Mult(I->lower, J->lower, I->R->cf);
  nums[1] = n_Mult(I->lower, J->upper, I->R->cf);
  nums[2] = n_Mult(I->upper, J->lower, I->R->cf);
  nums[3] = n_Mult(I->upper, J->upper, I->R->cf);

  int i, imax = 0, imin = 0;
  for (i = 1; i < 4; i++)
  {
    if (n_Greater(nums[i], nums[imax], I->R->cf))
    {
      imax = i;
    }
    if (n_Greater(nums[imin], nums[i], I->R->cf))
    {
      imin = i;
    }
  }

  lo = n_Copy(nums[imin], I->R->cf);
  up = n_Copy(nums[imax], I->R->cf);

  // delete products
  for (i = 0; i < 4; i++)
  {
    n_Delete(&nums[i], I->R->cf);
  }

  n_Normalize(lo, I->R->cf);
  n_Normalize(up, I->R->cf);

  return new interval(lo, up, I->R);
}

static interval* intervalAdd(interval *I, interval *J)
{
  number lo = n_Add(I->lower, J->lower, I->R->cf),
         up = n_Add(I->upper, J->upper, I->R->cf);

  n_Normalize(lo, I->R->cf);
  n_Normalize(up, I->R->cf);

  return new interval(lo, up);
}

static interval* intervalSubtract(interval *I, interval *J)
{
  number lo = n_Sub(I->lower, J->upper, I->R->cf),
         up = n_Sub(I->upper, J->lower, I->R->cf);

  n_Normalize(lo, I->R->cf);
  n_Normalize(up, I->R->cf);

  return new interval(lo, up, I->R);
}

static bool intervalEqual(interval *I, interval *J)
{
  assume(I->R == J->R);
  return n_Equal(I->lower, J->lower, I->R->cf)
      && n_Equal(I->upper, J->upper, I->R->cf);
}

// ckeck if zero is contained in an interval
static bool intervalContainsZero(interval *I)
{
  number n = n_Mult(I->lower, I->upper, I->R->cf);
  bool result = !n_GreaterZero(n, I->R->cf);
  // delete helper number
  n_Delete(&n, I->R->cf);

  return result;
}

static interval* intervalPower(interval *I, int p)
{
  if (p == 0)
  {
    return new interval(n_Init(1,I->R->cf), I->R);
  }

  // no initialisation required (?)
  number lo, up;

  n_Power(I->lower, p, &lo, I->R->cf);
  n_Power(I->upper, p, &up, I->R->cf);

  // should work now
  if (p % 2 == 1)
  {
    return new interval(lo, up, I->R);
  }
  else
  {
    // perform pointer swap if necessary
    number tmp;
    if (n_Greater(lo, up, I->R->cf))
    {
      tmp = up;
      up = lo;
      lo = tmp;
    }

    if (intervalContainsZero(I))
    {
      n_Delete(&lo, I->R->cf);
      lo = n_Init(0, I->R->cf);
    }
    return new interval(lo, up, I->R);
  }
}

/*
 * BINARY OPERATIONS:
 * Cases handled:
 *   I + J,
 *
 *   I - J,
 *
 *   a * J,
 *   I * a,
 *   I * J,
 *
 *   a / J,
 *   I / a,
 *   I / J
 *
 *   I ^ n,
 *
 *   I == J
 *
 *  where I, J, interval, a, b int or number, n int
 */

static BOOLEAN interval_Op2(int op, leftv result, leftv i1, leftv i2)
{
  interval *RES;

  switch(op)
  {
    case '+':
    {
      if (i1->Typ() != intervalID || i2->Typ() != intervalID)
      {
        WerrorS("syntax: <interval> + <interval>");
        return TRUE;
      }
      interval *I1, *I2;
      I1 = (interval*) i1->Data();
      I2 = (interval*) i2->Data();
      if (I1->R != I2->R)
      {
        WerrorS("adding intervals defined in different rings not supported");
        return TRUE;
      }

      RES = intervalAdd(I1, I2);
      break;
    }
    case '-':
    {
      if (i1->Typ() != intervalID || i2->Typ() != intervalID)
      {
        WerrorS("syntax: <interval> - <interval>");
        return TRUE;
      }
      interval *I1, *I2;
      I1 = (interval*) i1->Data();
      I2 = (interval*) i2->Data();
      if (I1->R != I2->R)
      {
        WerrorS("subtracting intervals defined in different rings not supported");
        return TRUE;
      }

      RES = intervalSubtract(I1, I2);
      break;
    }
    case '*':
    {
      if (i1->Typ() == i2->Typ())
      {
        // both must be intervals
        interval *I1, *I2;
        I1 = (interval*) i1->Data();
        I2 = (interval*) i2->Data();
        if (I1->R != I2->R)
        {
          WerrorS("multiplying intervals defined in different rings not supported");
          return TRUE;
        }

        RES = intervalMultiply(I1, I2);
      }
      else
      {
        // one arg is scalar, one is interval
        // give new names to reduce to one case
        leftv iscalar, iinterv;
        if (i1->Typ() != intervalID)
        {
          // i1 is scalar
          iscalar = i1;
          iinterv = i2;
        }
        else
        {
          // i2 is scalar
          iscalar = i2;
          iinterv = i1;
        }
        number n;

        switch (iscalar->Typ())
        {
          case INT_CMD:
            { n = nInit((int)(long) iscalar->Data()); break; }
          case NUMBER_CMD:
            { n = (number) iscalar->CopyD(); break; }
          default:
            { WerrorS("first argument not int/number/interval"); return TRUE; }
        }

        interval *I = (interval*) iinterv->Data();

        RES = intervalScalarMultiply(n, I);
        // n no longer needed, delete it
        nDelete(&n);
      }

      break;
    }
    case '/':
    {
      if(i2->Typ() == intervalID)
      {
        interval *I2;
        I2 = (interval*) i2->Data();

        // make sure I2 is invertible
        if (intervalContainsZero(I2))
        {
          WerrorS("second interval contains zero");
          return TRUE;
        }

        number invlo, invup;
        invlo = n_Invers(I2->lower, I2->R->cf);
        invup = n_Invers(I2->upper, I2->R->cf);

        // inverse interval
        interval *I2inv = new interval(invup, invlo, I2->R);

        if (i1->Typ() == intervalID)
        {
          interval *I1 = (interval*) i1->Data();
          if (I1->R != I2->R)
          {
            WerrorS("dividing intervals from different rings not supported");
            delete I2inv;
            return TRUE;
          }
          RES = intervalMultiply(I1, I2inv);
        }
        else
        {
          // i1 is not an interval
          number n;
          switch (i1->Typ())
          {
            case INT_CMD:
              {
                n = nInit((int)(long) i1->Data());
                break;
              }
            case NUMBER_CMD:
              {
                n = nCopy((number) i1->Data());
                break;
              }
            default:
              {
                WerrorS("first argument not int/number/interval");
                delete I2inv;
                return TRUE;
              }
          }
          RES = intervalScalarMultiply(n, I2inv);
          nDelete(&n);
        }

        delete I2inv;
        break;
      }
      else
      {
        // i2 is not an interval
        interval *I1 = (interval*) i1->Data();
        number n;
        switch(i2->Typ())
        {
          case INT_CMD:
            { n = nInit((int)(long) i2->Data()); break; }
          case NUMBER_CMD:
            { n = nCopy((number) i2->Data()); break; }
          default:
            {
              WerrorS("second argument not int/number/interval");
              return TRUE;
            }
        }
        // handle zero to prevent memory leak (?)
        if (nIsZero(n))
        {
          WerrorS("<interval>/0 not supported");
          return TRUE;
        }

        number nInv = nInvers(n);
        nDelete(&n);
        RES = intervalScalarMultiply(nInv, I1);
        nDelete(&nInv);

        break;
      }

      break;
    }
    case '^':
    {
      if (i1->Typ() != intervalID || i2->Typ() != INT_CMD)
      {
        WerrorS("syntax: <interval> ^ <int>");
        return TRUE;
      }
      int p = (int)(long) i2->Data();
      if (p < 0)
      {
        WerrorS("<interval> ^ n not implemented for n < 0");
        return TRUE;
      }
      interval *I = (interval*) i1->Data();

      RES = intervalPower(I, p);
      break;
    }
    case EQUAL_EQUAL:
    {
      if (i1->Typ() != intervalID || i2->Typ() != intervalID)
      {
        WerrorS("syntax: <interval> == <interval>");
        return TRUE;
      }
      interval *I1, *I2;
      I1 = (interval*) i1->Data();
      I2 = (interval*) i2->Data();

      result->rtyp = INT_CMD;
      result->data = (void*) intervalEqual(I1, I2);
      i1->CleanUp();
      i2->CleanUp();
      return FALSE;
    }
    case '[':
    {
      if (i1->Typ() != intervalID || i2->Typ() != INT_CMD)
      {
        WerrorS("syntax: <interval>[<int>]");
        return TRUE;
      }

      interval *I = (interval*) i1->Data();
      int n = (int)(long) i2->Data();

      number out;
      if (n == 1)
      {
        out = nCopy(I->lower);
      }
      else if (n == 2)
      {
        out = nCopy(I->upper);
      }
      else
      {
        WerrorS("Allowed indices are 1 and 2");
        return TRUE;
      }

      // delete number in result
      if (result != NULL && result->Data() != NULL)
      {
        number r = (number) result->Data();
        nDelete(&r);
      }

      result->rtyp = NUMBER_CMD;
      result->data = (void*) out;
      i1->CleanUp();
      i2->CleanUp();
      return FALSE;
    }
    default:
    {
      // use default error
      return blackboxDefaultOp2(op, result, i1, i2);
    }
  }

  // destroy data of result if it exists
  if (result->Data() != NULL)
  {
    delete (interval*) result->Data();
  }

  result->rtyp = intervalID;
  result->data = (void*) RES;
  i1->CleanUp();
  i2->CleanUp();
  return FALSE;
}

static BOOLEAN interval_serialize(blackbox*, void *d, si_link f)
{
  /*
    * Format: "interval" setring number number
    */
  interval *I = (interval*) d;

  sleftv l, lo, up;
  memset(&l, 0, sizeof(l));
  memset(&lo, 0, sizeof(lo));
  memset(&up, 0, sizeof(up));

  l.rtyp = STRING_CMD;
  l.data = (void*) "interval";

  lo.rtyp = NUMBER_CMD;
  lo.data = (void*) I->lower;

  up.rtyp = NUMBER_CMD;
  up.data = (void*) I->upper;

  f->m->Write(f, &l);

  ring R = I->R;

  f->m->SetRing(f, R, TRUE);
  f->m->Write(f, &lo);
  f->m->Write(f, &up);

  // no idea
  if (R != currRing)
      f->m->SetRing(f, currRing, FALSE);

  return FALSE;
}

static BOOLEAN interval_deserialize(blackbox**, void **d, si_link f)
{
  leftv l;

  l = f->m->Read(f);
  l->next = f->m->Read(f);

  number lo = (number) l->CopyD(),
         up = (number) l->next->CopyD();

  l->CleanUp();

  *d = (void*) new interval(lo, up);
  return FALSE;
}

/*
 * BOX FUNCTIONS
 */

static void* box_Init(blackbox*)
{
  return (void*) new box();
}

static void* box_Copy(blackbox*, void *d)
{
  return (void*) new box((box*) d);
}

static void box_Destroy(blackbox*, void *d)
{
  if (d != NULL)
    delete (box*) d;
}

static char* box_String(blackbox*, void *d)
{
  blackbox *b_i = getBlackboxStuff(intervalID);
  box *B = (box*) d;
  int i, n = B->R->N;

  if (B == NULL || B->intervals == NULL)
  {
    return omStrDup("ooo");
  }

  StringSetS(interval_String(b_i, (void*) B->intervals[0]));

  for (i = 1; i < n; i++)
  {
    // interpret box as Cartesian product, hence use " x "
    StringAppendS(" x ");
    StringAppendS(interval_String(b_i, (void*) B->intervals[i]));
  }
  return StringEndS();
}

// assigning values to intervals
static BOOLEAN box_Assign(leftv result, leftv args)
{
  box *RES;

  /*
   * Allow assignments of the form
   *
   *      B = C,
   *      B = l,
   *
   * where B, C boxes, l list of intervals
   */

  if (args->Typ() == boxID)
  {
    box *B = (box*) args->Data();
    RES = new box(B);
  }
  else if (args->Typ() == LIST_CMD)
  {
    RES = new box();
    lists l = (lists) args->Data();

    int i, m = lSize(l), n = currRing->N;
    // minimum
    int M = m > (n-1) ? (n-1) : m;

    for (i = 0; i <= M; i++)
    {
      if (l->m[i].Typ() != intervalID)
      {
        WerrorS("list contains non-intervals");
        delete RES;
        args->CleanUp();
        return TRUE;
      }
      RES->setInterval(i, (interval*) l->m[i].CopyD());

      // make sure rings of boxes and their intervals are consistent
      // this is important for serialization
      RES->intervals[i]->setRing(RES->R);
    }
  }
  else
  {
    WerrorS("Input not supported: first argument not box, list, or interval");
    return TRUE;
  }

  // destroy data of result if it exists
  if (result != NULL && result->Data() != NULL)
  {
    delete (box*) result->Data();
  }

  if (result->rtyp == IDHDL)
  {
    IDDATA((idhdl)result->data) = (char*) RES;
  }
  else
  {
    result->rtyp = boxID;
    result->data = (void*) RES;
  }
  args->CleanUp();

  return FALSE;
}

static BOOLEAN box_Op2(int op, leftv result, leftv b1, leftv b2)
{
  if (b1 == NULL || b1->Typ() != boxID)
  {
    Werror("first argument is not box but type(%d), second is type(%d)",
        b1->Typ(), b2->Typ());
    return TRUE;
  }

  box *B1 = (box*) b1->Data();
  int n = B1->R->N;

  box *RES;
  switch(op)
  {
    case '[':
    {
      if (b2 == NULL || b2->Typ() != INT_CMD)
      {
        WerrorS("second argument not int");
        return TRUE;
      }
      if (result->Data() != NULL)
      {
        delete (interval*) result->Data();
      }

      int i = (int)(long) b2->Data();

      if (i < 1 || i > n)
      {
        WerrorS("index out of bounds");
        return TRUE;
      }

      // delete data of result
      if (result->Data() != NULL)
      {
        delete (interval*) result->Data();
      }

      result->rtyp = intervalID;
      result->data = (void*) new interval(B1->intervals[i-1]);
      b1->CleanUp();
      b2->CleanUp();
      return FALSE;
    }
    case '-':
    {
      if (b2 == NULL || b2->Typ() != boxID)
      {
        WerrorS("second argument not box");
        return TRUE;
      }

      box *B2 = (box*) b2->Data();
      // maybe try to skip this initialisation
      // copying def of box() results in segfault?
      if (B1->R != B2->R)
      {
        WerrorS("subtracting boxes from different rings not supported");
        return TRUE;
      }
      RES = new box();
      int i;
      for (i = 0; i < n; i++)
      {
        RES->setInterval(i, intervalSubtract(B1->intervals[i], B2->intervals[i]));
      }

      if (result->Data() != NULL)
      {
        delete (box*) result->Data();
      }

      result->rtyp = boxID;
      result->data = (void*) RES;
      b1->CleanUp();
      b2->CleanUp();
      return FALSE;
    }
    case EQUAL_EQUAL:
    {
      if (b2 == NULL || b2->Typ() != boxID)
      {
        WerrorS("second argument not box");
      }
      box *B2 = (box*) b2->Data();
      int i;
      bool res = true;
      for (i = 0; i < n; i++)
      {
        if (!intervalEqual(B1->intervals[i], B2->intervals[i]))
        {
          res = false;
          break;
        }
      }

      result->rtyp = INT_CMD;
      result->data = (void*) res;
      b1->CleanUp();
      b2->CleanUp();
      return FALSE;
    }
    default:
      return blackboxDefaultOp2(op, result, b1, b2);
  }
}

static BOOLEAN box_OpM(int op, leftv result, leftv args)
{
  leftv a = args;
  switch(op)
  {
    case INTERSECT_CMD:
    {
      if (args->Typ() != boxID)
      {
        WerrorS("can only intersect boxes");
        return TRUE;
      }
      box *B = (box*) args->Data();
      int i, n = B->R->N;
      number lowerb[n], upperb[n];

      // do not copy, use same pointers, copy at the end
      for (i = 0; i < n; i++)
      {
        lowerb[i] = B->intervals[i]->lower;
        upperb[i] = B->intervals[i]->upper;
      }

      args = args->next;
      while(args != NULL)
      {
        if (args->Typ() != boxID)
        {
          WerrorS("can only intersect boxes");
          return TRUE;
        }

        B = (box*) args->Data();
        for (i = 0; i < n; i++)
        {
          if (nGreater(B->intervals[i]->lower, lowerb[i]))
          {
            lowerb[i] = B->intervals[i]->lower;
          }
          if (nGreater(upperb[i], B->intervals[i]->upper))
          {
            upperb[i] = B->intervals[i]->upper;
          }

          if (nGreater(lowerb[i], upperb[i]))
          {
            result->rtyp = INT_CMD;
            result->data = (void*) (-1);
            a->CleanUp();
            return FALSE;
          }
        }
        args = args->next;
      }

      // now copy the numbers
      box *RES = new box();
      for (i = 0; i < n; i++)
      {
        RES->setInterval(i, new interval(nCopy(lowerb[i]), nCopy(upperb[i])));
      }

      result->rtyp = boxID;
      result->data = (void*) RES;
      a->CleanUp();
      return FALSE;
    }
    default:
      return blackboxDefaultOpM(op, result, args);
  }
}

static BOOLEAN box_serialize(blackbox*, void *d, si_link f)
{
  /*
   * Format: "box" setring intervals[1] .. intervals[N]
   */
  box *B = (box*) d;
  int N = B->R->N, i;
  sleftv l, iv;
  memset(&l, 0, sizeof(l));
  memset(&iv, 0, sizeof(iv));

  l.rtyp = STRING_CMD;
  l.data = (void*) "box";

  f->m->Write(f, &l);
  f->m->SetRing(f, B->R, TRUE);

  iv.rtyp = intervalID;
  for (i = 0; i < N; i++)
  {
    iv.data = (void*) B->intervals[i];
    f->m->Write(f, &iv);
  }

  if (currRing != B->R)
    f->m->SetRing(f, currRing, FALSE);

  return FALSE;
}

static BOOLEAN box_deserialize(blackbox**, void **d, si_link f)
{
  leftv l;

  // read once to set ring
  l = f->m->Read(f);

  ring R = currRing;
  int i, N = R->N;
  box *B = new box();

  B->setInterval(0, (interval*) l->CopyD());
  l->CleanUp();

  for (i = 1; i < N; i++)
  {
    l = f->m->Read(f);
    B->setInterval(i, (interval*) l->CopyD());
    l->CleanUp();
  }

  *d = (void*) B;
  return FALSE;
}

static BOOLEAN boxSet(leftv result, leftv args)
{
  // check for proper types
  const short t[] = {3, (short) boxID, INT_CMD, (short) intervalID};
  if (!iiCheckTypes(args, t, 1))
  {
    return TRUE;
  }
  box *B = (box*) args->Data();
  int n = B->R->N,
      i = (int)(long) args->next->Data();
  interval *I = (interval*) args->next->next->Data();

  if (i < 1 || i > n)
  {
    WerrorS("boxSet: index out of range");
    return TRUE;
  }

  box *RES = new box(B);

  RES->setInterval(i-1, new interval(I));
  // ensure consistency
  RES->intervals[i-1]->setRing(RES->R);

  result->rtyp = boxID;
  result->data = (void*) RES;
  args->CleanUp();
  return FALSE;
}

/*
 * POLY FUNCTIONS
 */

static BOOLEAN evalPolyAtBox(leftv result, leftv args)
{
  const short t[] = {2, POLY_CMD, (short) boxID};
  if (!iiCheckTypes(args, t, 1))
  {
    return TRUE;
  }

  poly p = (poly) args->Data();
  box *B = (box*) args->next->Data();
  int i, pot, n = B->R->N;

  interval *tmp, *tmpPot, *tmpMonom, *RES = new interval();

  while(p != NULL)
  {
    tmpMonom = new interval(nInit(1));

    for (i = 1; i <= n; i++)
    {
      pot = pGetExp(p, i);

      tmpPot = intervalPower(B->intervals[i-1], pot);
      tmp = intervalMultiply(tmpMonom, tmpPot);

      delete tmpMonom;
      delete tmpPot;

      tmpMonom = tmp;
    }

    tmp = intervalScalarMultiply(p->coef, tmpMonom);
    delete tmpMonom;
    tmpMonom = tmp;

    tmp = intervalAdd(RES, tmpMonom);
    delete RES;
    delete tmpMonom;

    RES = tmp;

    p = p->next;
  }

  if (result->Data() != NULL)
  {
    delete (box*) result->Data();
  }

  result->rtyp = intervalID;
  result->data = (void*) RES;
  args->CleanUp();
  return FALSE;
}

/*
 * INIT MODULE
 */

extern "C" int SI_MOD_INIT(interval)(SModulFunctions* psModulFunctions)
{
  blackbox *b_iv = (blackbox*) omAlloc0(sizeof(blackbox)),
           *b_bx = (blackbox*) omAlloc0(sizeof(blackbox));

  b_iv->blackbox_Init        = interval_Init;
  b_iv->blackbox_Copy        = interval_Copy;
  b_iv->blackbox_destroy     = interval_Destroy;
  b_iv->blackbox_String      = interval_String;
  b_iv->blackbox_Assign      = interval_Assign;
  b_iv->blackbox_Op2         = interval_Op2;
  b_iv->blackbox_serialize   = interval_serialize;
  b_iv->blackbox_deserialize = interval_deserialize;

  intervalID = setBlackboxStuff(b_iv, "interval");

  b_bx->blackbox_Init        = box_Init;
  b_bx->blackbox_Copy        = box_Copy;
  b_bx->blackbox_destroy     = box_Destroy;
  b_bx->blackbox_String      = box_String;
  b_bx->blackbox_Assign      = box_Assign;
  b_bx->blackbox_Op2         = box_Op2;
  b_bx->blackbox_OpM         = box_OpM;
  b_bx->blackbox_serialize   = box_serialize;
  b_bx->blackbox_deserialize = box_deserialize;

  boxID = setBlackboxStuff(b_bx, "box");

  // add additional functions
  psModulFunctions->iiAddCproc("rootisolation.lib", "length", FALSE, length);
  psModulFunctions->iiAddCproc("rootisolation.lib", "boxSet", FALSE, boxSet);
  psModulFunctions->iiAddCproc("rootisolation.lib", "evalPolyAtBox", FALSE,
    evalPolyAtBox);

  return MAX_TOK;
}
// vim: spell spelllang=en
