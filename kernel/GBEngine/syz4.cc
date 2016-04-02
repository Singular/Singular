/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: resolutions
*/

#include <kernel/GBEngine/syz.h>
#include <omalloc/omalloc.h>
#include <coeffs/numbers.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>

#include <vector>
#include <map>

static poly TraverseTail_test(poly multiplier, const int tail);
static poly ComputeImage_test(poly multiplier, const int tail);
static poly TraverseTail_test(poly multiplier, poly tail);
static poly ReduceTerm_test(poly multiplier, poly term4reduction, poly syztermCheck);
static poly leadmonom_test(const poly p, const ring r, const bool bSetZeroComp = true);

static ideal m_idLeads_test;
static ideal m_idTails_test;
static ideal m_syzLeads_test;

class CLCM_test: public std::vector<bool>
{
  public:
    CLCM_test(const ideal& L);
    void redefine(const ideal L);
    bool Check(const poly m) const;

  private:
    bool m_compute;
    unsigned int m_N;   // number of ring variables
};

CLCM_test::CLCM_test(const ideal& L):
    std::vector<bool>(),
    m_compute(false), m_N(0)   // m_N(rVar(currRing))
{
  const ring R = currRing;
  if( L != NULL )
  {
    const int l = IDELEMS(L);
    resize(l, false);
    for( int k = l - 1; k >= 0; k-- )
    {
      const poly a = L->m[k];
      for (unsigned int j = m_N; j > 0; j--)
        if ( !(*this)[j] )
          (*this)[j] = (p_GetExp(a, j, R) > 0);
    }
    m_compute = true;
  }
}

void CLCM_test::redefine(const ideal L)
{
  resize(0); // std::vector<bool>()
  m_compute = false;
  m_N = rVar(currRing);
  const ring R = currRing;
  if( L != NULL )
  {
    const int l = IDELEMS(L);
    resize(l, false);
    for( int k = l - 1; k >= 0; k-- )
    {
      const poly a = L->m[k];
      for (unsigned int j = m_N; j > 0; j--)
        if ( !(*this)[j] )
          (*this)[j] = (p_GetExp(a, j, R) > 0);
    }
    m_compute = true;
  }
}

bool CLCM_test::Check(const poly m) const
{
  if( m_compute && (m != NULL))
  {
    const ring R = currRing;
    for (unsigned int j = m_N; j > 0; j--)
      if ( (*this)[j] )
        if(p_GetExp(m, j, R) > 0)
          return true;
    return false;
  } else return true;
}

static CLCM_test m_lcm(NULL);

static poly TraverseNF_test(const poly a, const poly a2)
{
  const ideal& L = m_idLeads_test;
  const ring R = currRing;
  const int r = p_GetComp(a, R) - 1;
  poly aa = leadmonom_test(a, R);
  poly t = TraverseTail_test(aa, r);
  if( a2 != NULL )
  {
    const int r2 = p_GetComp(a2, R) - 1;
    poly aa2 = leadmonom_test(a2, R);
    poly s =  TraverseTail_test(aa2, r2);
    p_Delete(&aa2, R);
    t = p_Add_q(a2, p_Add_q(t, s, R), R);
  } else
    t = p_Add_q(t, ReduceTerm_test(aa, L->m[r], a), R);
  p_Delete(&aa, R);
  return t;
}

#define CACHE 1

#if CACHE
typedef poly TCacheKey_test;
typedef poly TCacheValue_test;

bool my_p_LmCmp_test (poly a, poly b, const ring r)
{
  return p_LmCmp(a, b, r) == -1;
}

struct CCacheCompare_test
{
  const ring& m_ring_test;
  CCacheCompare_test(): m_ring_test(currRing) {}
  CCacheCompare_test(const ring& r): m_ring_test(r) {}
  CCacheCompare_test(const CCacheCompare_test& lhs):
    m_ring_test(lhs.m_ring_test) {}
  CCacheCompare_test& operator=(const CCacheCompare_test& lhs)
  {
    return (const_cast<CCacheCompare_test&>(lhs));
  }
  inline bool operator() (const TCacheKey_test& l, const TCacheKey_test& r)
    const
  {
    return my_p_LmCmp_test(l, r, m_ring_test);
  }
};

typedef std::map<TCacheKey_test, TCacheValue_test, CCacheCompare_test>
  TP2PCache_test;
typedef std::map<int, TP2PCache_test> TCache_test;

static TCache_test m_cache_test;

static FORCE_INLINE poly myp_Head_test(const poly p, const bool bIgnoreCoeff,
  const ring r)
{
  p_LmCheckPolyRing1(p, r);
  poly np;
  omTypeAllocBin(poly, np, r->PolyBin);
  p_SetRingOfLm(np, r);
  memcpy(np->exp, p->exp, r->ExpL_Size*sizeof(long));
  pNext(np) = NULL;
  pSetCoeff0(np, (bIgnoreCoeff)? NULL : n_Copy(pGetCoeff(p), r->cf));
  p_LmCheckPolyRing1(np, r);
  return np;
}
#endif   // CACHE

static poly TraverseTail_test(poly multiplier, const int tail)
{
  const ring& r = currRing;
#if CACHE
  TCache_test::iterator top_itr = m_cache_test.find(tail);
  if ( top_itr != m_cache_test.end() )
  {
     TP2PCache_test& T = top_itr->second;
     TP2PCache_test::iterator itr = T.find(multiplier);
     if( itr != T.end() )
     {
       if( itr->second == NULL )
         return (NULL);
       poly p = p_Copy(itr->second, r);
       if( !n_Equal( pGetCoeff(multiplier), pGetCoeff(itr->first), r) )
       {
         number n = n_Div( pGetCoeff(multiplier), pGetCoeff(itr->first), r);
         p = p_Mult_nn(p, n, r);
         n_Delete(&n, r);
       }
       return p;
     }
     const poly p = ComputeImage_test(multiplier, tail);
     T.insert(TP2PCache_test::value_type(myp_Head_test(multiplier, (p==NULL),
       r), p) );
     return p_Copy(p, r);
  }
  CCacheCompare_test o(r);
  TP2PCache_test T(o);
#endif   // CACHE
  const poly p = ComputeImage_test(multiplier, tail);
#if CACHE
  T.insert(TP2PCache_test::value_type(myp_Head_test(multiplier, (p==NULL), r),
    p));
  m_cache_test.insert( TCache_test::value_type(tail, T) );
  return p_Copy(p, r);
#else
  return p;
#endif   // CACHE
}

static poly ComputeImage_test(poly multiplier, const int tail)
{
  const poly t = m_idTails_test->m[tail];
  if(t != NULL)
  {
    const poly p = TraverseTail_test(multiplier, t);
    return p;
  }
  return NULL;
}

#define BUCKETS 1
/* 0: use original code (not implemented)
 * 1: use Singular's SBuckets
 * 2: use Singular's polynomial arithmetic
 */

static poly TraverseTail_test(poly multiplier, poly tail)
{
  const ring r = currRing;
  if( !m_lcm.Check(multiplier) )
  {
    return NULL;
  }
#if BUCKETS == 0
  SBucketWrapper sum(r, m_sum_bucket_factory);
#elif BUCKETS == 1
  sBucket_pt sum = sBucketCreate(currRing);
#else   // BUCKETS == 2
  poly s = NULL;
#endif   // BUCKETS
  for(poly p = tail; p != NULL; p = pNext(p))   // iterate over the tail
  {
    const poly rt = ReduceTerm_test(multiplier, p, NULL);
#if BUCKETS == 0
    sum.Add(rt);
#elif BUCKETS == 1
    sBucket_Add_p(sum, rt, pLength(rt));
#else   // BUCKETS == 2
    s = p_Add_q(s, rt, r);
#endif   // BUCKETS
  }
#if BUCKETS == 0
  const poly s = sum.ClearAdd();
#elif BUCKETS == 1
  poly s;
  int l;
  sBucketClearAdd(sum, &s, &l);
  sBucketDestroy(&sum);
#else   // BUCKETS == 2
  // nothing to do
#endif   // BUCKETS
  return s;
}

static poly leadmonom_test(const poly p, const ring r, const bool bSetZeroComp)
{
  if( p == NULL )
    return NULL;
  poly m = p_LmInit(p, r);
  p_SetCoeff0(m, n_Copy(p_GetCoeff(p, r), r), r);
  if( bSetZeroComp )
    p_SetComp(m, 0, r);
  p_Setm(m, r);
  return m;
}

// _p_LmDivisibleByNoComp for a | b*c
static inline BOOLEAN _p_LmDivisibleByNoComp(const poly a, const poly b, const poly c, const ring r)
{
  int i=r->VarL_Size - 1;
  unsigned long divmask = r->divmask;
  unsigned long la, lb;

  if (r->VarL_LowIndex >= 0)
  {
    i += r->VarL_LowIndex;
    do
    {
      la = a->exp[i];
      lb = b->exp[i] + c->exp[i];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        return FALSE;
      }
      i--;
    }
    while (i>=r->VarL_LowIndex);
  }
  else
  {
    do
    {
      la = a->exp[r->VarL_Offset[i]];
      lb = b->exp[r->VarL_Offset[i]] + c->exp[r->VarL_Offset[i]];
      if ((la > lb) ||
          (((la & divmask) ^ (lb & divmask)) != ((lb - la) & divmask)))
      {
        return FALSE;
      }
      i--;
    }
    while (i>=0);
  }
  return TRUE;
}

class CLeadingTerm_test
{
  public:
    CLeadingTerm_test(unsigned int label,  const poly lt, const ring);

    bool DivisibilityCheck(const poly multiplier, const poly t, const unsigned long not_sev, const ring r) const;
    bool DivisibilityCheck(const poly product, const unsigned long not_sev, const ring r) const;

    bool CheckLT( const ideal & L ) const;

    inline poly lt() const { return m_lt; };
    inline unsigned long sev() const { return m_sev; };
    inline unsigned int label() const { return m_label; };

  private:
    const unsigned long m_sev; ///< not short exp. vector

    // NOTE/TODO: either of the following should be enough:
    const unsigned int  m_label; ///< index in the main L[] + 1

    const poly          m_lt; ///< the leading term itself L[label-1]

    // disable the following:
    CLeadingTerm_test();
    CLeadingTerm_test(const CLeadingTerm_test&);
    void operator=(const CLeadingTerm_test&);
};

CLeadingTerm_test::CLeadingTerm_test(unsigned int _label,  const poly _lt, const ring R):
    m_sev( p_GetShortExpVector(_lt, R) ),  m_label( _label ),  m_lt( _lt )
{
}

bool CLeadingTerm_test::DivisibilityCheck(const poly product, const unsigned long not_sev, const ring r) const
{
  return p_LmShortDivisibleByNoComp(lt(), sev(), product, not_sev, r);
}

bool CLeadingTerm_test::DivisibilityCheck(const poly m, const poly t, const unsigned long not_sev, const ring r) const
{
  if (sev() & not_sev) {
    return false;
  }
  return _p_LmDivisibleByNoComp(lt(), m, t, r);
}

class CReducerFinder_test
{
  friend class CDivisorEnumerator_test;
  friend class CDivisorEnumerator2_test;

  public:
    typedef long TComponentKey;
    typedef std::vector<const CLeadingTerm_test*> TReducers;

  private:
    typedef std::map< TComponentKey, TReducers> CReducersHash;

  public:
    /// goes over all leading terms
    CReducerFinder_test(const ideal L);

    void redefine(const ideal L);

    void Initialize(const ideal L);

    ~CReducerFinder_test();

    static poly FindReducer(const poly multiplier, const poly monom, const poly syzterm, const CReducerFinder_test& checker);

    bool IsDivisible(const poly q) const;

    inline bool IsNonempty() const { return !m_hash.empty(); }

    int PreProcessTerm(const poly t, CReducerFinder_test& syzChecker) const;

  private:
    ideal m_L; ///< only for debug

    CReducersHash m_hash; // can also be replaced with a vector indexed by components

  private:
    CReducerFinder_test(const CReducerFinder_test&);
    void operator=(const CReducerFinder_test&);
};

CReducerFinder_test::CReducerFinder_test(const ideal L):
    m_L(const_cast<ideal>(L)), // for debug anyway
    m_hash()
{
  if( L != NULL )
    Initialize(L);
}

CReducerFinder_test::~CReducerFinder_test()
{
  for( CReducersHash::iterator it = m_hash.begin(); it != m_hash.end(); it++ )
  {
    TReducers& v = it->second;
    for(TReducers::const_iterator vit = v.begin(); vit != v.end(); vit++ )
      delete const_cast<CLeadingTerm_test*>(*vit);
    v.erase(v.begin(), v.end());
  }
  m_hash.erase(m_hash.begin(), m_hash.end());
}

void CReducerFinder_test::redefine(const ideal L)
{
  m_L = const_cast<ideal>(L); // for debug anyway
  for( CReducersHash::iterator it = m_hash.begin(); it != m_hash.end(); it++ )
  {
    TReducers& v = it->second;
    for(TReducers::const_iterator vit = v.begin(); vit != v.end(); vit++ )
      delete const_cast<CLeadingTerm_test*>(*vit);
    v.erase(v.begin(), v.end());
  }
  m_hash.erase(m_hash.begin(), m_hash.end());
  m_hash.clear();
  if( L != NULL )
    Initialize(L);
}

static CReducerFinder_test m_checker(NULL);
static CReducerFinder_test m_div(NULL);

class CDivisorEnumerator_test
{
  private:
    const CReducerFinder_test& m_reds;
    const poly m_product;
    const unsigned long m_not_sev;
    const long m_comp;

    CReducerFinder_test::CReducersHash::const_iterator m_itr;
    CReducerFinder_test::TReducers::const_iterator m_current, m_finish;

    bool m_active;

  public:
    CDivisorEnumerator_test(const CReducerFinder_test& self, const poly product):
        m_reds(self),
        m_product(product),
        m_not_sev(~p_GetShortExpVector(product, currRing)),
        m_comp(p_GetComp(product, currRing)),
        m_itr(), m_current(), m_finish(),
        m_active(false)
    {
    }

    inline bool Reset()
    {
      m_active = false;
      m_itr = m_reds.m_hash.find(m_comp);
      if( m_itr == m_reds.m_hash.end() )
        return false;
      m_current = (m_itr->second).begin();
      m_finish = (m_itr->second).end();
      if (m_current == m_finish)
        return false;
      return true;
    }

    const CLeadingTerm_test& Current() const
    {
      return *(*m_current);
    }

    inline bool MoveNext()
    {
      if( m_active )
        ++m_current;
      else
        m_active = true; // for Current()
      for( ; m_current != m_finish; ++m_current )
      {
        if( Current().DivisibilityCheck(m_product, m_not_sev, currRing) )
        {
          return true;
        }
      }
      m_active = false;
      return false;
    }
};

class CDivisorEnumerator2_test
{
  private:
    const CReducerFinder_test& m_reds;
    const poly m_multiplier, m_term;
    const unsigned long m_not_sev;
    const long m_comp;

    CReducerFinder_test::CReducersHash::const_iterator m_itr;
    CReducerFinder_test::TReducers::const_iterator m_current, m_finish;

    bool m_active;

  public:
    CDivisorEnumerator2_test(const CReducerFinder_test& self, const poly m, const poly t):
        m_reds(self),
        m_multiplier(m), m_term(t),
        m_not_sev(~p_GetShortExpVector(m, t, currRing)),
        m_comp(p_GetComp(t, currRing)),
        m_itr(), m_current(), m_finish(),
        m_active(false)
    {
    }

    inline bool Reset()
    {
      m_active = false;
      m_itr = m_reds.m_hash.find(m_comp);
      if( m_itr == m_reds.m_hash.end() )
        return false;
      m_current = (m_itr->second).begin();
      m_finish = (m_itr->second).end();
      if (m_current == m_finish)
        return false;
      return true;
    }

    const CLeadingTerm_test& Current() const
    {
      return *(*m_current);
    }

    inline bool MoveNext()
    {
      if( m_active )
        ++m_current;
      else
        m_active = true;
      // looking for the next good entry
      for( ; m_current != m_finish; ++m_current )
      {
        if( Current().DivisibilityCheck(m_multiplier, m_term, m_not_sev, currRing) )
        {
          return true;
        }
      }
      m_active = false;
      return false;
    }
};

poly CReducerFinder_test::FindReducer(const poly multiplier, const poly t,
                                 const poly syzterm,
                                 const CReducerFinder_test& syz_checker)
{
  const ring r = currRing;
  CDivisorEnumerator2_test itr(m_div, multiplier, t);
  if( !itr.Reset() )
    return NULL;
  long c = 0;
  if (syzterm != NULL)
    c = p_GetComp(syzterm, r) - 1;
  const BOOLEAN to_check = (syz_checker.IsNonempty());
  const poly q = p_New(r);
  pNext(q) = NULL;
  while( itr.MoveNext() )
  {
    const poly p = itr.Current().lt();
    const int k  = itr.Current().label();
    p_ExpVectorSum(q, multiplier, t, r); // q == product == multiplier * t
    p_ExpVectorDiff(q, q, p, r); // (LM(product) / LM(L[k]))
    p_SetComp(q, k + 1, r);
    p_Setm(q, r);
    // cannot allow something like: a*gen(i) - a*gen(i)
    if (syzterm != NULL && (k == c))
    if (p_ExpVectorEqual(syzterm, q, r))
    {
      continue;
    }
    // while the complement (the fraction) is not reducible by leading syzygies
    if( to_check && syz_checker.IsDivisible(q) )
    {
      continue;
    }
    number n = n_Mult( p_GetCoeff(multiplier, r), p_GetCoeff(t, r), r);
    p_SetCoeff0(q, n_InpNeg(n, r), r);
    return q;
  }
  p_LmFree(q, r);
  return NULL;
}

bool CReducerFinder_test::IsDivisible(const poly product) const
{
  CDivisorEnumerator_test itr(*this, product);
  if( !itr.Reset() )
    return false;
  return itr.MoveNext();
}

static poly ReduceTerm_test(poly multiplier, poly term4reduction, poly syztermCheck)
{
  const ring r = currRing;
  poly s = NULL;
  if( m_lcm.Check(multiplier) )
  {
    s = CReducerFinder_test::FindReducer(multiplier, term4reduction, syztermCheck, m_checker);
  }
  if( s == NULL )
  {
    return NULL;
  }
  poly b = leadmonom_test(s, r);
  const int c = p_GetComp(s, r) - 1;
  const poly t = TraverseTail_test(b, c);
  pDelete(&b);
  if( t != NULL )
    s = p_Add_q(s, t, r);
  return s;
}

void CReducerFinder_test::Initialize(const ideal L)
{
  if( m_L == NULL )
    m_L = L;
  if( L != NULL )
  {
    const ring R = currRing;
    for( int k = IDELEMS(L) - 1; k >= 0; k-- )
    {
      const poly a = L->m[k];
      if( a != NULL )
      {
        m_hash[p_GetComp(a, R)].push_back( new CLeadingTerm_test(k, a, R) );
      }
    }
  }
}

/*****************************************************************************/

typedef poly syzHeadFunction(ideal, int, int);

static poly syzHeadFrame(const ideal G, const int i, const int j)
{
    const ring r = currRing;
    const poly f_i = G->m[i];
    const poly f_j = G->m[j];
    poly head = p_Init(r);
    pSetCoeff0(head, n_Init(1, r->cf));
    long exp_i, exp_j, lcm;
    for (int k = (int)r->N; k > 0; k--) {
        exp_i = p_GetExp(f_i, k, r);
        exp_j = p_GetExp(f_j, k, r);
        lcm = si_max(exp_i, exp_j);
        p_SetExp(head, k, lcm-exp_i, r);
    }
    p_SetComp(head, i+1, r);
    p_Setm(head, r);
    return head;
}

static poly syzHeadExtFrame(const ideal G, const int i, const int j)
{
    const ring r = currRing;
    const poly f_i = G->m[i];
    const poly f_j = G->m[j];
    poly head = p_Init(r);
    pSetCoeff0(head, n_Init(1, r->cf));
    poly head_ext = p_Init(r);
    pSetCoeff0(head_ext, n_InpNeg(n_Div(pGetCoeff(f_j), pGetCoeff(f_i), r->cf),
        r->cf));
    long exp_i, exp_j, lcm;
    for (int k = (int)r->N; k > 0; k--) {
        exp_i = p_GetExp(f_i, k, r);
        exp_j = p_GetExp(f_j, k, r);
        lcm = si_max(exp_i, exp_j);
        p_SetExp(head, k, lcm-exp_i, r);
        p_SetExp(head_ext, k, lcm-exp_j, r);
    }
    p_SetComp(head, i+1, r);
    p_Setm(head, r);
    p_SetComp(head_ext, j+1, r);
    p_Setm(head_ext, r);
    head->next = head_ext;
    return head;
}

typedef ideal syzM_i_Function(ideal, int, syzHeadFunction);

static ideal syzM_i_unsorted(const ideal G, const int i,
    const syzHeadFunction *syzHead)
{
    ideal M_i = NULL;
    int comp = pGetComp(G->m[i]);
    int ncols = 0;
    for (int j = i-1; j >= 0; j--) {
        if (pGetComp(G->m[j]) == comp) ncols++;
    }
    if (ncols > 0) {
        M_i = idInit(ncols, G->ncols);
        int k = ncols-1;
        for (int j = i-1; j >= 0; j--) {
            if (pGetComp(G->m[j]) == comp) {
                M_i->m[k] = syzHead(G, i, j);
                k--;
            }
        }
        id_DelDiv(M_i, currRing);
        idSkipZeroes(M_i);
    }
    return M_i;
}

static ideal syzM_i_sorted(const ideal G, const int i,
    const syzHeadFunction *syzHead)
{
    ideal M_i = NULL;
    int comp = pGetComp(G->m[i]);
    int index = i-1;
    while (pGetComp(G->m[index]) == comp) index--;
    index++;
    int ncols = i-index;
    if (ncols > 0) {
        M_i = idInit(ncols, G->ncols);
        for (int j = ncols-1; j >= 0; j--) {
            M_i->m[j] = syzHead(G, i, j+index);
        }
        id_DelDiv(M_i, currRing);
        idSkipZeroes(M_i);
    }
    return M_i;
}

static ideal idConcat(const ideal* M, const int size, const int rank)
{
    int ncols = 0;
    for (int i = size-1; i >= 0; i--) {
        if (M[i] != NULL) {
            ncols += M[i]->ncols;
        }
    }
    if (ncols == 0) return idInit(1, rank);
    ideal result = idInit(ncols, rank);
    int k = ncols-1;
    for (int i = size-1; i >= 0; i--) {
        if (M[i] != NULL) {
            for (int j = M[i]->ncols-1; j >= 0; j--) {
                result->m[k] = M[i]->m[j];
                k--;
            }
        }
    }
    return result;
}

#define SORT_MI 1

#if SORT_MI
static int compare_Mi(const void* a, const void *b)
{
    const ring r = currRing;
    poly p_a = *((poly *)a);
    poly p_b = *((poly *)b);
    int cmp;
    int deg_a = p_Deg(p_a, r);
    int deg_b = p_Deg(p_b, r);
    cmp = (deg_a > deg_b) - (deg_a < deg_b);
    if (cmp != 0) {
         return cmp;
    }
    int comp_a = p_GetComp(p_a, r);
    int comp_b = p_GetComp(p_b, r);
    cmp = (comp_a > comp_b) - (comp_a < comp_b);
    if (cmp != 0) {
         return cmp;
    }
    int exp_a[r->N+1];
    int exp_b[r->N+1];
    p_GetExpV(p_a, exp_a, r);
    p_GetExpV(p_b, exp_b, r);
    for (int i = r->N; i > 0; i--) {
        cmp = (exp_a[i] > exp_b[i]) - (exp_a[i] < exp_b[i]);
        if (cmp != 0) {
            return cmp;
        }
    }
    return 0;
}
#endif   // SORT_MI

static ideal computeFrame(const ideal G, const syzM_i_Function syzM_i,
    const syzHeadFunction *syzHead)
{
    ideal* M = (ideal *)omalloc((G->ncols-1)*sizeof(ideal));
    for (int i = G->ncols-2; i >= 0; i--) {
        M[i] = syzM_i(G, i+1, syzHead);
    }
    ideal frame = idConcat(M, G->ncols-1, G->ncols);
    for (int i = G->ncols-2; i >= 0; i--) {
        if (M[i] != NULL) {
            omFreeSize(M[i]->m, IDELEMS(M[i])*sizeof(poly));
            omFreeBin(M[i], sip_sideal_bin);
        }
    }
    omFree(M);
#if SORT_MI
    qsort(frame->m, IDELEMS(frame), sizeof(poly), compare_Mi);
#endif
    return frame;
}

static void setGlobalVariables(const resolvente res, const int index)
{
    idDelete(&m_idLeads_test);
    idDelete(&m_idTails_test);
    idDelete(&m_syzLeads_test);
    m_idLeads_test = idCopy(res[index-1]);
    m_idTails_test = idInit(IDELEMS(res[index-1]), 1);
    for (int i = IDELEMS(res[index-1])-1; i >= 0; i--) {
        m_idTails_test->m[i] = m_idLeads_test->m[i]->next;
        m_idLeads_test->m[i]->next = NULL;
    }
    m_div.redefine(m_idLeads_test);
    m_lcm.redefine(m_idLeads_test);
    m_syzLeads_test = idCopy(res[index]);
    for (int i = IDELEMS(res[index])-1; i >= 0; i--) {
        pDelete(&m_syzLeads_test->m[i]->next);
        m_syzLeads_test->m[i]->next = NULL;
    }
    m_checker.redefine(m_syzLeads_test);
#if CACHE
    for (TCache_test::iterator it = m_cache_test.begin();
        it != m_cache_test.end(); it++) {
        TP2PCache_test& T = it->second;
        for (TP2PCache_test::iterator vit = T.begin(); vit != T.end(); vit++) {
            p_Delete((&(vit->second)), currRing);
            p_Delete(const_cast<poly*>(&(vit->first)), currRing);
        }
        T.erase(T.begin(), T.end());
    }
    m_cache_test.erase(m_cache_test.begin(), m_cache_test.end());
#endif   // CACHE
}

#define MYLIFT 0

static void computeLiftings(const resolvente res, const int index)
{
#if MYLIFT
    for (int j = res[index]->ncols-1; j >= 0; j--) {
        liftTree(res[index]->m[j], res[index-1]);
    }
#else
    setGlobalVariables(res, index);
    poly p;
    for (int j = res[index]->ncols-1; j >= 0; j--) {
        p = res[index]->m[j];
        pDelete(&res[index]->m[j]->next);
        p->next = NULL;
        res[index]->m[j]->next = TraverseNF_test(p, NULL);
    }
#endif   // MYLIFT
}

static void sortPolysTails(const resolvente res, const int index)
{
    const ring r = currRing;
    for (int j = res[index]->ncols-1; j >= 0; j--) {
        if (res[index]->m[j]->next != NULL) {
            res[index]->m[j]->next->next
                = p_SortAdd(res[index]->m[j]->next->next, r);
        }
    }
}

static int computeResolution(resolvente &res, const int length,
    const syzHeadFunction *syzHead)
{
    int index = 0;
    if (!idIs0(res[index]) && index < length) {
        index++;
        res[index] = computeFrame(res[index-1], syzM_i_unsorted, syzHead);
    }
    while (!idIs0(res[index]) && index < length) {
#if 1
        computeLiftings(res, index);
        sortPolysTails(res, index);
#endif   // LIFT
        index++;
        res[index] = computeFrame(res[index-1], syzM_i_sorted, syzHead);
    }
    if (index < length) {
        res = (resolvente)omReallocSize(res, (length+1)*sizeof(ideal),
            (index+1)*sizeof(ideal));
    }
    return index;
}

static void sortPolys(const resolvente res, const int length)
{
    const ring r = currRing;
    for (int i = length; i > 0; i--) {
        for (int j = res[i]->ncols-1; j >= 0; j--) {
            res[i]->m[j] = p_SortAdd(res[i]->m[j], r, TRUE);
        }
    }
}

syStrategy syFrank(const ideal arg, int &length, const char *method)
{
    syStrategy result = (syStrategy)omAlloc0(sizeof(ssyStrategy));
    resolvente res = (resolvente)omAlloc0((length+1)*sizeof(ideal));
    res[0] = id_Copy(arg, currRing);
    syzHeadFunction *syzHead;
    if (strcmp(method, "frame") == 0 || strcmp(method, "linear strand") == 0) {
        syzHead = syzHeadFrame;
    }
    else {
        syzHead = syzHeadExtFrame;
    }
    length = computeResolution(res, length, syzHead);
    sortPolys(res, length);

    idDelete(&m_idLeads_test);
    idDelete(&m_idTails_test);
    idDelete(&m_syzLeads_test);

    result->fullres = res;
    result->length = length+1;
    return result;
}

