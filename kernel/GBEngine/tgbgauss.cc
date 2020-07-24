/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: gauss implementation for F4
*/

#include "kernel/mod2.h"
#include "misc/options.h"
#include "kernel/GBEngine/tgbgauss.h"
#include <stdlib.h>
#include "kernel/GBEngine/kutil.h"
#include "kernel/polys.h"
static const int bundle_size=100;

mac_poly mac_p_add_ff_qq(mac_poly a, number f,mac_poly b)
{
  mac_poly erg;
  mac_poly* set_this;
  set_this=&erg;
  while((a!=NULL) &&(b!=NULL))
  {
    if (a->exp<b->exp)
    {
      (*set_this)=a;
      a=a->next;
      set_this= &((*set_this)->next);
    }
    else
    {
      if (a->exp>b->exp)
      {
        mac_poly in =new mac_poly_r();
        in->exp=b->exp;
        in->coef=nMult(b->coef,f);
        (*set_this)=in;
        b=b->next;
        set_this= &((*set_this)->next);
      }
      else
      {
        //a->exp==b->ecp
        number n=nMult(b->coef,f);
        number n2=nAdd(a->coef,n);
        nDelete(&n);
        nDelete(&(a->coef));
        if (nIsZero(n2))
        {
          nDelete(&n2);
          mac_poly ao=a;
          a=a->next;
          delete ao;
          b=b->next;
        }
        else
        {
          a->coef=n2;
          b=b->next;
          (*set_this)=a;
          a=a->next;
          set_this= &((*set_this)->next);
        }
      }
    }
  }
  if((a==NULL)&&(b==NULL))
  {
    (*set_this)=NULL;
    return erg;
  }
  if (b==NULL)
  {
    (*set_this=a);
    return erg;
  }

  //a==NULL
  while(b!=NULL)
  {
    mac_poly mp= new mac_poly_r();
    mp->exp=b->exp;
    mp->coef=nMult(f,b->coef);
    (*set_this)=mp;
    set_this=&(mp->next);
    b=b->next;
  }
  (*set_this)=NULL;
  return erg;
}

void mac_mult_cons(mac_poly p,number c)
{
  while(p)
  {
    number m=nMult(p->coef,c);
    nDelete(&(p->coef));
    p->coef=m;
    p=p->next;
  }
}

int mac_length(mac_poly p)
{
  int l=0;
  while(p){
    l++;
    p=p->next;
  }
  return l;
}

//contrary to delete on the mac_poly_r, the coefficients are also destroyed here
void mac_destroy(mac_poly p)
{
  mac_poly iter=p;
  while(iter)
  {
    mac_poly next=iter->next;
    nDelete(&iter->coef);
    delete iter;
    iter=next;
  }
}

void simple_gauss(tgb_sparse_matrix* mat, slimgb_alg* /*c*/)
{
  int col, row;
  int* row_cache=(int*) omAlloc(mat->get_rows()*sizeof(int));
  col=0;
  row=0;
  int i;
  int pn=mat->get_rows();
  int matcol=mat->get_columns();
  int* area=(int*) omAlloc(sizeof(int)*((matcol-1)/bundle_size+1));
  const int max_area_index=(matcol-1)/bundle_size;
    //rows are divided in areas
  //if row begins with columns col, it is located in [area[col/bundle_size],area[col/bundle_size+1]-1]
  assume(pn>0);
  //first clear zeroes
  for(i=0;i<pn;i++)
  {
    if(mat->zero_row(i))
    {
      mat->perm_rows(i,pn-1);
      pn--;
      if(i!=pn){i--;}
    }

  }
  mat->sort_rows();
  for(i=0;i<pn;i++)
  {
      row_cache[i]=mat->min_col_not_zero_in_row(i);
      // Print("row_cache:%d\n",row_cache[i]);
  }
  int last_area=-1;
  for(i=0;i<pn;i++)
  {
    int this_area=row_cache[i]/bundle_size;
    assume(this_area>=last_area);
    if(this_area>last_area)
    {
      int j;
      for(j=last_area+1;j<=this_area;j++)
        area[j]=i;
      last_area=this_area;
    }
  }
  for(i=last_area+1;i<=max_area_index;i++)
  {
    area[i]=pn;
  }
  while(row<pn-1)
  {
    //row is the row where pivot should be
    // row== pn-1 means we have only to act on one row so no red nec.
    //we assume further all rows till the pn-1 row are non-zero

    //select column

    //col=mat->min_col_not_zero_in_row(row);
    int max_in_area;
    {
      int tai=row_cache[row]/bundle_size;
      assume(tai<=max_area_index);
      if(tai==max_area_index)
        max_in_area=pn-1;
      else
        max_in_area=area[tai+1]-1;
    }
    assume(row_cache[row]==mat->min_col_not_zero_in_row(row));
    col=row_cache[row];

    assume(col!=matcol);
    int found_in_row;

    found_in_row=row;
    BOOLEAN must_reduce=FALSE;
    assume(pn<=mat->get_rows());
    for(i=row+1;i<=max_in_area;i++)
    {
      int first;//=mat->min_col_not_zero_in_row(i);
      assume(row_cache[i]==mat->min_col_not_zero_in_row(i));
      first=row_cache[i];
      assume(first!=matcol);
      if(first<col)
      {
        col=first;
        found_in_row=i;
        must_reduce=FALSE;
      }
      else
      {
        if(first==col)
          must_reduce=TRUE;
      }
    }
    //select pivot
    int act_l=nSize(mat->get(found_in_row,col))*mat->non_zero_entries(found_in_row);
    if(must_reduce)
    {
      for(i=found_in_row+1;i<=max_in_area;i++)
      {
        assume(mat->min_col_not_zero_in_row(i)>=col);
        assume(row_cache[i]==mat->min_col_not_zero_in_row(i));
#ifndef SING_NDEBUG
        int first=row_cache[i];
        assume(first!=matcol);
#endif
        //      if((!(mat->is_zero_entry(i,col)))&&(mat->non_zero_entries(i)<act_l))
        int nz;
        if((row_cache[i]==col)&&((nz=nSize(mat->get(i,col))*mat->non_zero_entries(i))<act_l))
        {
          found_in_row=i;
          act_l=nz;
        }

      }
    }
    mat->perm_rows(row,found_in_row);
    int h=row_cache[row];
    row_cache[row]=row_cache[found_in_row];
    row_cache[found_in_row]=h;

    if(!must_reduce)
    {
      row++;
      continue;
    }
    //reduction
    //must extract content and normalize here
    mat->row_content(row);
    //mat->row_normalize(row); // row_content does normalize

    //for(i=row+1;i<pn;i++){
    for(i=max_in_area;i>row;i--)
    {
      int col_area_index=col/bundle_size;
      assume(col_area_index<=max_area_index);
      assume(mat->min_col_not_zero_in_row(i)>=col);
      assume(row_cache[i]==mat->min_col_not_zero_in_row(i));
#ifndef SING_NDEBUG
      int first=row_cache[i];
      assume(first!=matcol);
#endif
      if(row_cache[i]==col)
      {

        number c1=mat->get(i,col);
        number c2=mat->get(row,col);
        number n1=c1;
        number n2=c2;

        ksCheckCoeff(&n1,&n2,currRing->cf);
        //nDelete(&c1);
        n1=nInpNeg(n1);
        mat->mult_row(i,n2);
        mat->add_lambda_times_row(i,row,n1);
        nDelete(&n1);
        nDelete(&n2);
        assume(mat->is_zero_entry(i,col));
        row_cache[i]=mat->min_col_not_zero_in_row(i);
        assume(mat->min_col_not_zero_in_row(i)>col);
        if(row_cache[i]==matcol)
        {
          int index;
          index=i;
          int last_in_area;
          int this_cai=col_area_index;
          while(this_cai<max_area_index)
          {
            last_in_area=area[this_cai+1]-1;
            int h_c=row_cache[last_in_area];
            row_cache[last_in_area]=row_cache[index];
            row_cache[index]=h_c;
            mat->perm_rows(index,last_in_area);
            index=last_in_area;
            this_cai++;
            area[this_cai]--;
          }
          mat->perm_rows(index,pn-1);
          row_cache[index]=row_cache[pn-1];
          row_cache[pn-1]=matcol;
          pn--;
        }
        else
        {
          int index;
          index=i;
          int last_in_area;
          int this_cai=col_area_index;
          int final_cai=row_cache[index]/bundle_size;
          assume(final_cai<=max_area_index);
          while(this_cai<final_cai)
          {
            last_in_area=area[this_cai+1]-1;
            int h_c=row_cache[last_in_area];
            row_cache[last_in_area]=row_cache[index];
            row_cache[index]=h_c;
            mat->perm_rows(index,last_in_area);
            index=last_in_area;
            this_cai++;
            area[this_cai]--;
          }
        }
      }
      else
        assume(mat->min_col_not_zero_in_row(i)>col);
    }
//     for(i=row+1;i<pn;i++)
//     {
//       assume(mat->min_col_not_zero_in_row(i)==row_cache[i]);
//       // if(mat->zero_row(i))
//       assume(matcol==mat->get_columns());
//       if(row_cache[i]==matcol)
//       {
//         assume(mat->zero_row(i));
//         mat->perm_rows(i,pn-1);
//         row_cache[i]=row_cache[pn-1];
//         row_cache[pn-1]=matcol;
//         pn--;
//         if(i!=pn){i--;}
//       }
//     }
#ifdef TGB_DEBUG
  {
    int last=-1;
    for(i=0;i<pn;i++)
    {
      int act=mat->min_col_not_zero_in_row(i);
      assume(act>last);
    }
    for(i=pn;i<mat->get_rows();i++)
    {
      assume(mat->zero_row(i));
    }
  }
#endif
    row++;
  }
  omFree(area);
  omFree(row_cache);
}

void simple_gauss2(tgb_matrix* mat)
{
  int col, row;
  col=0;
  row=0;
  int i;
  int pn=mat->get_rows();
  assume(pn>0);
  //first clear zeroes
//   for(i=0;i<pn;i++)
//   {
//     if(mat->zero_row(i))
//     {
//       mat->perm_rows(i,pn-1);
//       pn--;
//       if(i!=pn){i--;}
//     }
//   }
  while((row<pn-1)&&(col<mat->get_columns())){
    //row is the row where pivot should be
    // row== pn-1 means we have only to act on one row so no red nec.
    //we assume further all rows till the pn-1 row are non-zero

    //select column

    //    col=mat->min_col_not_zero_in_row(row);
    assume(col!=mat->get_columns());
    int found_in_row=-1;

    //    found_in_row=row;
    assume(pn<=mat->get_rows());
    for(i=row;i<pn;i++)
    {
      //    int first=mat->min_col_not_zero_in_row(i);
      //  if(first<col)
      if(!(mat->is_zero_entry(i,col)))
      {
        found_in_row=i;
        break;
      }
    }
    if(found_in_row!=-1)
    {
    //select pivot
      int act_l=mat->non_zero_entries(found_in_row);
      for(i=i+1;i<pn;i++)
      {
        int vgl;
        assume(mat->min_col_not_zero_in_row(i)>=col);
        if((!(mat->is_zero_entry(i,col)))
        &&((vgl=mat->non_zero_entries(i))<act_l))
        {
          found_in_row=i;
          act_l=vgl;
        }

      }
      mat->perm_rows(row,found_in_row);

      //reduction
      for(i=row+1;i<pn;i++){
        assume(mat->min_col_not_zero_in_row(i)>=col);
        if(!(mat->is_zero_entry(i,col)))
        {
          number c1=nCopy(mat->get(i,col));
          c1=nInpNeg(c1);
          number c2=mat->get(row,col);
          number n1=c1;
          number n2=c2;

          ksCheckCoeff(&n1,&n2,currRing->cf);
          nDelete(&c1);
          mat->mult_row(i,n2);
          mat->add_lambda_times_row(i,row,n1);
          assume(mat->is_zero_entry(i,col));
        }
        assume(mat->min_col_not_zero_in_row(i)>col);
      }
      row++;
    }
    col++;
    // for(i=row+1;i<pn;i++)
//     {
//       if(mat->zero_row(i))
//       {
//         mat->perm_rows(i,pn-1);
//         pn--;
//         if(i!=pn){i--;}
//       }
//     }
  }
}


tgb_matrix::tgb_matrix(int i, int j)
{
  n=(number**) omAlloc(i*sizeof (number*));;
  int z;
  int z2;
  for(z=0;z<i;z++)
  {
    n[z]=(number*)omAlloc(j*sizeof(number));
    for(z2=0;z2<j;z2++)
    {
      n[z][z2]=nInit(0);
    }
  }
  columns=j;
  rows=i;
  free_numbers=FALSE;
}

tgb_matrix::~tgb_matrix()
{
  int z;
  for(z=0;z<rows;z++)
  {
    if(n[z])
    {
      if(free_numbers)
      {
        int z2;
        for(z2=0;z2<columns;z2++)
        {
          nDelete(&(n[z][z2]));
        }
      }
      omFree(n[z]);
    }
  }
  omfree(n);
}

void tgb_matrix::print()
{
  int i;
  int j;
  PrintLn();
  for(i=0;i<rows;i++)
  {
    PrintS("(");
    for(j=0;j<columns;j++)
    {
      StringSetS("");
      n_Write(n[i][j],currRing->cf);
      char *s=StringEndS();
      PrintS(s);
      omFree(s);
      PrintS("\t");
    }
    PrintS(")\n");
  }
}

//transfers ownership of n to the matrix
void tgb_matrix::set(int i, int j, number nn)
{
  assume(i<rows);
  assume(j<columns);
  n[i][j]=nn;
}

int tgb_matrix::get_rows()
{
  return rows;
}

int tgb_matrix::get_columns()
{
  return columns;
}

number tgb_matrix::get(int i, int j)
{
  assume(i<rows);
  assume(j<columns);
  return n[i][j];
}

BOOLEAN tgb_matrix::is_zero_entry(int i, int j)
{
  return (nIsZero(n[i][j]));
}

void tgb_matrix::perm_rows(int i, int j)
{
  number* h;
  h=n[i];
  n[i]=n[j];
  n[j]=h;
}

int tgb_matrix::min_col_not_zero_in_row(int row)
{
  int i;
  for(i=0;i<columns;i++)
  {
    if(!(nIsZero(n[row][i])))
      return i;
  }
  return columns;//error code
}

int tgb_matrix::next_col_not_zero(int row,int pre)
{
  int i;
  for(i=pre+1;i<columns;i++)
  {
    if(!(nIsZero(n[row][i])))
      return i;
  }
  return columns;//error code
}

BOOLEAN tgb_matrix::zero_row(int row)
{
  int i;
  for(i=0;i<columns;i++)
  {
    if(!(nIsZero(n[row][i])))
      return FALSE;
  }
  return TRUE;
}

int tgb_matrix::non_zero_entries(int row)
{
  int i;
  int z=0;
  for(i=0;i<columns;i++)
  {
    if(!(nIsZero(n[row][i])))
      z++;
  }
  return z;
}

//row add_to=row add_to +row summand*factor
void tgb_matrix::add_lambda_times_row(int add_to,int summand,number factor)
{
  int i;
  for(i=0;i<columns;i++)
  {
    if(!(nIsZero(n[summand][i])))
    {
      number n1=n[add_to][i];
      number n2=nMult(factor,n[summand][i]);
      n[add_to][i]=nAdd(n1,n2);
      nDelete(&n1);
      nDelete(&n2);
    }
  }
}

void tgb_matrix::mult_row(int row,number factor)
{
  if (nIsOne(factor))
    return;
  int i;
  for(i=0;i<columns;i++)
  {
    if(!(nIsZero(n[row][i])))
    {
      number n1=n[row][i];
      n[row][i]=nMult(n1,factor);
      nDelete(&n1);
    }
  }
}

void tgb_matrix::free_row(int row, BOOLEAN free_non_zeros)
{
  int i;
  for(i=0;i<columns;i++)
    if((free_non_zeros)||(!(nIsZero(n[row][i]))))
      nDelete(&(n[row][i]));
  omFree(n[row]);
  n[row]=NULL;
}

tgb_sparse_matrix::tgb_sparse_matrix(int i, int j, ring rarg)
{
  mp=(mac_poly*) omAlloc(i*sizeof (mac_poly));;
  int z;
  for(z=0;z<i;z++)
  {
    mp[z]=NULL;
  }
  columns=j;
  rows=i;
  free_numbers=FALSE;
  r=rarg;
}

tgb_sparse_matrix::~tgb_sparse_matrix()
{
  int z;
  for(z=0;z<rows;z++)
  {
    if(mp[z]!=NULL)
    {
      if(free_numbers)
      {
        mac_destroy(mp[z]);
      }
      else {
        while(mp[z]!=NULL)
        {
          mac_poly next=mp[z]->next;
          delete mp[z];
          mp[z]=next;
        }
      }
    }
  }
  omfree(mp);
}

static int row_cmp_gen(const void* a, const void* b)
{
  const mac_poly ap= *((mac_poly*) a);
  const mac_poly bp=*((mac_poly*) b);
  if (ap==NULL) return 1;
  if (bp==NULL) return -1;
  if (ap->exp<bp->exp) return -1;
  return 1;
}

void tgb_sparse_matrix::sort_rows()
{
  qsort(mp,rows,sizeof(mac_poly),row_cmp_gen);
}

void tgb_sparse_matrix::print()
{
  int i;
  int j;
  PrintLn();
  for(i=0;i<rows;i++)
  {
    PrintS("(");
    for(j=0;j<columns;j++)
    {
      StringSetS("");
      number n=get(i,j);
      n_Write(n,currRing->cf);
      char *s=StringEndS();
      PrintS(s);
      omFree(s);
      PrintS("\t");
    }
    PrintS(")\n");
  }
}

//transfers ownership of n to the matrix
void tgb_sparse_matrix::set(int i, int j, number n)
{
  assume(i<rows);
  assume(j<columns);
  mac_poly* set_this=&mp[i];
  //  while(((*set_this)!=NULL)&&((*set_this)\AD>exp<j))
  while(((*set_this)!=NULL) && ((*set_this)->exp<j))
    set_this=&((*set_this)->next);

  if (((*set_this)==NULL)||((*set_this)->exp>j))
  {
    if (nIsZero(n)) return;
    mac_poly old=(*set_this);
    (*set_this)=new mac_poly_r();
    (*set_this)->exp=j;
    (*set_this)->coef=n;
    (*set_this)->next=old;
    return;
  }
  assume((*set_this)->exp==j);
  if(!nIsZero(n))
  {
    nDelete(&(*set_this)->coef);
    (*set_this)->coef=n;
  }
  else
  {
    nDelete(&(*set_this)->coef);
    mac_poly dt=(*set_this);
    (*set_this)=dt->next;
    delete dt;
  }
  return;
}

int tgb_sparse_matrix::get_rows()
{
  return rows;
}

int tgb_sparse_matrix::get_columns()
{
  return columns;
}

number tgb_sparse_matrix::get(int i, int j)
{
  assume(i<rows);
  assume(j<columns);
  mac_poly rr=mp[i];
  while((rr!=NULL)&&(rr->exp<j))
    rr=rr->next;
  if ((rr==NULL)||(rr->exp>j))
  {
    number n=nInit(0);
    return n;
  }
  assume(rr->exp==j);
  return rr->coef;
}

BOOLEAN tgb_sparse_matrix::is_zero_entry(int i, int j)
{
  assume(i<rows);
  assume(j<columns);
  mac_poly rr=mp[i];
  while((rr!=NULL)&&(rr->exp<j))
    rr=rr->next;
  if ((rr==NULL)||(rr->exp>j))
  {
    return TRUE;
  }
  assume(!nIsZero(rr->coef));
  assume(rr->exp==j);
  return FALSE;
}

int tgb_sparse_matrix::min_col_not_zero_in_row(int row)
{
  if(mp[row]!=NULL)
  {
    assume(!nIsZero(mp[row]->coef));
    return mp[row]->exp;
  }
  else
    return columns;//error code
}

int tgb_sparse_matrix::next_col_not_zero(int row,int pre)
{
  mac_poly rr=mp[row];
  while((rr!=NULL)&&(rr->exp<=pre))
    rr=rr->next;
  if(rr!=NULL)
  {
    assume(!nIsZero(rr->coef));
    return rr->exp;
  }
  return columns;//error code
}

BOOLEAN tgb_sparse_matrix::zero_row(int row)
{
  assume((mp[row]==NULL)||(!nIsZero(mp[row]->coef)));
  if (mp[row]==NULL)
    return TRUE;
  else
    return FALSE;
}

void tgb_sparse_matrix::row_normalize(int row)
{
  if (!rField_has_simple_inverse(r))  /* Z/p, GF(p,n), R, long R/C */
  {
    mac_poly m=mp[row];
    while (m!=NULL)
    {
      #ifndef SING_NDEBUG
      if (currRing==r) {nTest(m->coef);}
      #endif
      n_Normalize(m->coef,r->cf);
      m=m->next;
    }
  }
}

void tgb_sparse_matrix::row_content(int row)
{
  mac_poly ph=mp[row];
  number h,d;
  mac_poly p;

  if(TEST_OPT_CONTENTSB) return;
  if(ph->next==NULL)
  {
    nDelete(&ph->coef);
    ph->coef=nInit(1);
  }
  else
  {
    nNormalize(ph->coef);
    if(!nGreaterZero(ph->coef))
    {
      p=ph;
      while(p!=NULL)
      {
        p->coef=nInpNeg(p->coef);
        p=p->next;
      }
    }
    if (currRing->cf->cfGcd==ndGcd) return;

    h=nCopy(ph->coef);
    p = ph->next;

    while (p!=NULL)
    {
      nNormalize(p->coef);
      d=n_Gcd(h,p->coef,currRing->cf);
      nDelete(&h);
      h = d;
      if(nIsOne(h))
      {
        break;
      }
      p=p->next;
    }
    p = ph;
    //number tmp;
    if(!nIsOne(h))
    {
      while (p!=NULL)
      {
        d = nExactDiv(p->coef,h);
        nDelete(&p->coef);
        p->coef=d;
        p=p->next;
      }
    }
    nDelete(&h);
  }
}
int tgb_sparse_matrix::non_zero_entries(int row)
{
  return mac_length(mp[row]);
}

//row add_to=row add_to +row summand*factor
void tgb_sparse_matrix::add_lambda_times_row(int add_to,int summand,number factor)
{
  mp[add_to]= mac_p_add_ff_qq(mp[add_to], factor,mp[summand]);
}

void tgb_sparse_matrix::mult_row(int row,number factor)
{
  if (nIsZero(factor))
  {
    mac_destroy(mp[row]);
    mp[row]=NULL;

    return;
  }
  if(nIsOne(factor))
    return;
  mac_mult_cons(mp[row],factor);
}

void tgb_sparse_matrix::free_row(int row, BOOLEAN free_non_zeros)
{
  if(free_non_zeros)
    mac_destroy(mp[row]);
  else
  {
    while(mp[row]!=NULL)
    {
      mac_poly next=mp[row]->next;
      delete mp[row];
      mp[row]=next;
    }
  }
  mp[row]=NULL;
}
