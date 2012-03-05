// binomial.cc

// implementation of class binomial

#ifndef BINOMIAL_CC
#define BINOMIAL_CC

#include <climits>
#include "binomial__term_ordering.h"

///////////////////////// constructors and destructor //////////////////////

// For a better overview, the constructor code is separated for
// NO_SUPPORT_DRIVEN_METHODS and SUPPORT_DRIVEN_METHODS.

#ifdef NO_SUPPORT_DRIVEN_METHODS

binomial::binomial(const short& number_of_variables)
    :_number_of_variables(number_of_variables)
{
  exponent_vector=new Integer[_number_of_variables];
}




binomial::binomial(const short& number_of_variables,const Integer* exponents)
    :_number_of_variables(number_of_variables)
{

  // range check for rarely used constructors
  if(_number_of_variables<=0)
  {
    cerr<<"\nWARNING: binomial::binomial(const short&, const Integer*):\n"
      "argument out of range"<<endl;
    exponent_vector=NULL;
    // to avoid problems when deleting
    return;
  }

  // initialization
  exponent_vector=new Integer[_number_of_variables];
  for(short i=0;i<_number_of_variables;i++)
    exponent_vector[i]=exponents[i];
}




binomial::binomial(const short& number_of_variables,const Integer* exponents,
                   const term_ordering& w)
    :_number_of_variables(number_of_variables)
{

  // range check for rarely used constructors
  if(_number_of_variables<=0)
  {
    cerr<<"\nWARNING: binomial::binomial(const short&, const Integer*):\n"
      "argument out of range"<<endl;
    exponent_vector=NULL;
    // to avoid problems when deleting
    return;
  }

  exponent_vector=new Integer[_number_of_variables];

  // determine head and tail
  if(w.compare_to_zero(exponents)>=0)
    for(short i=0;i<_number_of_variables;i++)
      exponent_vector[i]=exponents[i];
  else
    for(short i=0;i<_number_of_variables;i++)
      exponent_vector[i]=-exponents[i];

}




binomial::binomial(const binomial& b)
    :_number_of_variables(b._number_of_variables)
{
  exponent_vector=new Integer[_number_of_variables];
  for(short i=0;i<_number_of_variables;i++)
    exponent_vector[i]=b.exponent_vector[i];
}




#endif  // NO_SUPPORT_DRIVEN_METHODS




#ifdef SUPPORT_DRIVEN_METHODS




binomial::binomial(const short& number_of_variables)
    :_number_of_variables(number_of_variables),head_support(0),tail_support(0)
{
  exponent_vector=new Integer[_number_of_variables];
}




binomial::binomial(const short& number_of_variables, const Integer* exponents)
    :_number_of_variables(number_of_variables),head_support(0),tail_support(0)
{

  // range check for rarely used constructors
  if(_number_of_variables<=0)
  {
    exponent_vector=NULL;
    // to avoid problems when deleting
    cerr<<"\nWARNING: binomial::binomial(const short&, const Integer*):\n"
      "argument out of range"<<endl;
    return;
  }

  exponent_vector=new Integer[_number_of_variables];

  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);
  // number of bits of a long int


  for(short i=0;i<_number_of_variables;i++)
  {

#ifdef SUPPORT_VARIABLES_FIRST

    Integer actual_entry=exponents[i];
    exponent_vector[i]=actual_entry;

#endif  // SUPPORT_VARIABLES_FIRST

#ifdef SUPPORT_VARIABLES_LAST

    short j=_number_of_variables-1-i;
    Integer actual_entry=exponents[j];
    exponent_vector[j]=actual_entry;

#endif  // SUPPORT_VARIABLES_LAST

    if(i<size_of_support_vectors)
      // variable i is considered in the support vectors
      if(actual_entry>0)
        head_support|=(1<<i);
        // bit i of head_support is set to 1 (counting from 0)
      else
        if(actual_entry<0)
          tail_support|=(1<<i);
    // bit i of tail_support is set to 1
  }

}




binomial::binomial(const short& number_of_variables, const Integer* exponents,
                   const term_ordering& w)
    :_number_of_variables(number_of_variables),head_support(0),tail_support(0)
{
  // range check for rarely used constructors
  if(_number_of_variables<=0)
  {
    cerr<<"\nWARNING: binomial::binomial(const short&, const Integer*):\n"
      "argument out of range"<<endl;
    exponent_vector=NULL;
    // to avoid problems when deleting
    return;
  }


  exponent_vector=new Integer[_number_of_variables];

  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);
  // number of bits of a long int

  // determine head and tail
  short sign;
  if(w.compare_to_zero(exponents)>=0)
    sign=1;
  else
    sign=-1;


  for(short i=0;i<_number_of_variables;i++)
  {

#ifdef SUPPORT_VARIABLES_FIRST

    Integer actual_entry=sign*exponents[i];
    exponent_vector[i]=actual_entry;

#endif  // SUPPORT_VARIABLES_FIRST

#ifdef SUPPORT_VARIABLES_LAST

    short j=_number_of_variables-1-i;
    Integer actual_entry=sign*exponents[j];
    exponent_vector[j]=actual_entry;

#endif  // SUPPORT_VARIABLES_LAST

    if(i<size_of_support_vectors)
      // variable i is considered in the support vectors
      if(actual_entry>0)
        head_support|=(1<<i);
        // bit i of head_support is set to 1 (counting from 0)
      else
        if(actual_entry<0)
          tail_support|=(1<<i);
    // bit i of tail_support is set to 1
  }

}




binomial::binomial(const binomial& b)
    :_number_of_variables(b._number_of_variables),
     head_support(b.head_support),tail_support(b.tail_support)
{
  exponent_vector=new Integer[_number_of_variables];
  for(short i=0;i<_number_of_variables;i++)
    exponent_vector[i]=b.exponent_vector[i];
}




#endif  // SUPPORT_DRIVEN_METHODS




binomial::~binomial()
{
  delete[] exponent_vector;
}




/////////////////// object information /////////////////////////////////////




short binomial::number_of_variables() const
{
  return _number_of_variables;
}




short binomial::error_status() const
{
  if(_number_of_variables<0)
    return _number_of_variables;
  return 0;
}




//////////////////// assignment and access operators ////////////////////////




binomial& binomial::operator=(const binomial& b)
{

  if(&b==this)
    return *this;


#ifdef SUPPORT_DRIVEN_METHODS

  head_support=b.head_support;
  tail_support=b.tail_support;

#endif  // SUPPORT_DRIVEN_METHODS

  if(_number_of_variables!=b._number_of_variables)
  {
    delete[] exponent_vector;
    _number_of_variables=b._number_of_variables;

    if(_number_of_variables<=0)
    {
      cerr<<"\nWARNING: binomial& binomial::operator=(const binomial&):\n"
        "assignment from corrupt binomial"<<endl;
      exponent_vector=NULL;
      return (*this);
    }

    exponent_vector=new Integer[_number_of_variables];
  }

  for(short i=0;i<_number_of_variables;i++)
    exponent_vector[i]=b.exponent_vector[i];

  return(*this);
}




Integer binomial::operator[](const short& i) const
{
  return exponent_vector[i];
}




//////////////////// comparison operators ///////////////////////////////////




BOOLEAN binomial::operator==(const binomial& b) const
{
  if(this == &b)
    return(TRUE);

#ifdef SUPPORT_DRIVEN_METHODS

  if(head_support!=b.head_support)
    return(FALSE);
  if(tail_support!=b.tail_support)
    return(FALSE);

#endif  // SUPPORT_DRIVEN_METHODS

  for(short i=0;i<_number_of_variables;i++)
    if(exponent_vector[i]!=b.exponent_vector[i])
      return(FALSE);
  return(TRUE);
}




BOOLEAN binomial::operator!=(const binomial& b) const
{
  if(this == &b)
    return(FALSE);

#ifdef SUPPORT_DRIVEN_METHODS

  if(head_support!=b.head_support)
    return(TRUE);
  if(tail_support!=b.tail_support)
    return(TRUE);

#endif  // SUPPORT_DRIVEN_METHODS

  for(short i=0;i<_number_of_variables;i++)
    if(exponent_vector[i]!=b.exponent_vector[i])
      return(TRUE);
  return(FALSE);
}




// operators for efficient comparisons with the zero binomial (comp_value=0)

BOOLEAN binomial::operator==(const Integer comp_value) const
{

#ifdef SUPPORT_DRIVEN_METHODS

  if(comp_value==0)
  {
    if(head_support!=0)
      return(FALSE);
    if(tail_support!=0)
      return(FALSE);
  }

#endif  // SUPPORT_DRIVEN_METHODS

  for(short i=0;i<_number_of_variables;i++)
    if(exponent_vector[i]!=comp_value)
      return(FALSE);
  return(TRUE);
}




BOOLEAN binomial::operator!=(const Integer comp_value) const
{

#ifdef SUPPORT_DRIVEN_METHODS

  if(comp_value==0)
  {
    if(head_support!=0)
      return(TRUE);
    if(tail_support!=0)
      return(TRUE);
  }

#endif  // SUPPORT_DRIVEN_METHODS

  for(short i=0;i<_number_of_variables;i++)
    if(exponent_vector[i]!=comp_value)
      return(TRUE);
  return(FALSE);
}




BOOLEAN binomial::operator<=(const Integer comp_value) const
{

#ifdef SUPPORT_DRIVEN_METHODS

  if(comp_value==0)
    if(head_support!=0)
      return(FALSE);

#endif  // SUPPORT_DRIVEN_METHODS

  for(short i=0;i<_number_of_variables;i++)
    if(exponent_vector[i]>comp_value)
      return(FALSE);
  return(TRUE);
}




BOOLEAN binomial::operator>=(const Integer comp_value) const
{

#ifdef SUPPORT_DRIVEN_METHODS

  if(comp_value==0)
    if(tail_support!=0)
      return(FALSE);

#endif

  for(short i=0;i<_number_of_variables;i++)
    if(exponent_vector[i]<comp_value)
      return(FALSE);
  return(TRUE);
}




////////////// basic routines for Buchbergers's algorithm //////////////////




Integer binomial::head_reductions_by(const binomial& b) const
// Returns the number of possible reductions of the actual binomial´s head
// by the binomial b. This is the minimum of the quotients
// exponent_vector[i]/b.exponent_vector[i]
// where exponent_vector[i]>0 and b.exponent_vector[i]>0
// (0 if there are no such quotients).
// A negative return value means b=0 or head(b)=1.
{


#ifdef NO_SUPPORT_DRIVEN_METHODS

  Integer result=-1;
  Integer new_result=-1;
  // -1 stands for infinitely many reductions

  for(short i=0;i<_number_of_variables;i++)
    // explicit sign tests for all components
  {
    Integer actual_b_component=b.exponent_vector[i];

    if(actual_b_component>0)
      // else variable i is not involved in the head of b
    {
      Integer actual_component=exponent_vector[i];

      if(actual_component<actual_b_component)
        return 0;

      new_result=(Integer) (actual_component/actual_b_component);

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }
 }

#endif  // NO_SUPPORT_DRIVEN_METHODS


#ifdef SUPPORT_DRIVEN_METHODS

  if((head_support&b.head_support)!=b.head_support)
    // head support of b not contained in head support, no reduction possible
    return 0;


  Integer result=-1;
  Integer new_result=-1;
  // -1 stands for infinitely many reductions


  short size_of_support_vectors=CHAR_BIT*sizeof(long);
  // number of bits of a long int
  if(size_of_support_vectors>_number_of_variables)
    size_of_support_vectors=_number_of_variables;
    // number of components of the support vectors


#ifdef SUPPORT_VARIABLES_FIRST

  for(short i=0;i<size_of_support_vectors;i++)
    // test support variables

    if(b.head_support&(1<<i))
      // bit i of b.head_support is 1
    {
      new_result=(Integer) (exponent_vector[i]/b.exponent_vector[i]);
      // remember that exponent_vector[i]>0 !
      // (head support contains that of b)

      if(new_result==0)
        // exponent_vector[i]<b.exponent_vector[i]
        return 0;

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }


  for(short i=size_of_support_vectors;i<_number_of_variables;i++)
    // test non-support variables
    // from now on we need explicit sign tests
  {
    Integer actual_b_component=b.exponent_vector[i];

    if(actual_b_component>0)
      // else variable i is not involved in the head of b
    {
      Integer actual_component=exponent_vector[i];

      if(actual_component<actual_b_component)
        return 0;

      new_result=(Integer) (actual_component/actual_b_component);

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }
  }

#endif  // SUPPORT_VARIABLES_FIRST


#ifdef SUPPORT_VARIABLES_LAST

  for(short i=0;i<size_of_support_vectors;i++)
    // test support variables

    if(b.head_support&(1<<i))
      // bit i of b.head_support is 1
    {
      short j=_number_of_variables-1-i;
      new_result=(Integer) (exponent_vector[j]/ b.exponent_vector[j]);
      // remember that exponent_vector[_number_of_variables-1-i]>0 !
      // (head support contains that of b)

      if(new_result==0)
        // exponent_vector[_number_of_variables-1-i]
        // <b.exponent_vector[_number_of_variables-1-i]
        return 0;

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }


  for(short i=size_of_support_vectors;i<_number_of_variables;i++)
    // test non-support variables
    // from now on we need explicit sign tests
  {
    short j=_number_of_variables-1-i;
    Integer actual_b_component=b.exponent_vector[j];

    if(actual_b_component>0)
      // else variable number_of_variables-1-i is not involved in the head of b
    {
      Integer actual_component=exponent_vector[j];

      if(actual_component<actual_b_component)
        return 0;

      new_result=(Integer) (actual_component/actual_b_component);

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }
  }

#endif  // SUPPORT_VARIABLES_LAST


#endif  // SUPPORT_DRIVEN_METHODS


  return(result);
}




Integer binomial::tail_reductions_by(const binomial& b) const
// Returns the number of possible reductions of the actual binomial´s tail
// by the binomial b. This is the minimum of the quotients
// - exponent_vector[i]/b.exponent_vector[i]
// where exponent_vector[i]<0 and b.exponent_vector[i]>0
// (0 if there are no such quotients).
// A negative return value means b=0 or head(b)=1.
{


#ifdef NO_SUPPORT_DRIVEN_METHODS

  Integer result=-1;
  Integer new_result=-1;
  // -1 stands for infinitely many reductions

  for(short i=0;i<_number_of_variables;i++)
    // explicit sign tests for all components
  {
    Integer actual_b_component=b.exponent_vector[i];

    if(actual_b_component>0)
      // else variable i is not involved in the head of b
    {
      Integer actual_component=-exponent_vector[i];

      if(actual_component<actual_b_component)
        return 0;

      new_result=(Integer) (actual_component/actual_b_component);

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }
 }

#endif  // NO_SUPPORT_DRIVEN_METHODS


#ifdef SUPPORT_DRIVEN_METHODS

  if((tail_support&b.head_support)!=b.head_support)
    // head support of b not contained in tail support, no reduction possible
    return 0;


  Integer result=-1;
  Integer new_result=-1;
  // -1 stands for infinitely many reductions


  short size_of_support_vectors=CHAR_BIT*sizeof(long);
  // number of bits of a long int
  if(size_of_support_vectors>_number_of_variables)
    size_of_support_vectors=_number_of_variables;
    // number of components of the support vectors


#ifdef SUPPORT_VARIABLES_FIRST

  for(short i=0;i<size_of_support_vectors;i++)
    // test support variables

    if(b.head_support&(1<<i))
      // bit i of b.head_support is 1
    {
      new_result=(Integer) (-exponent_vector[i]/b.exponent_vector[i]);
      // remember that exponent_vector[i]<0 !
      // (tail support contains the head support of b)

      if(new_result==0)
        // -exponent_vector[i]<b.exponent_vector[i]
        return 0;

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }


  for(short i=size_of_support_vectors;i<_number_of_variables;i++)
    // test non-support variables
    // from now on we need explicit sign tests
  {
    Integer actual_b_component=b.exponent_vector[i];

    if(actual_b_component>0)
      // else variable i is not involved in the head of b
    {
      Integer actual_component=-exponent_vector[i];

      if(actual_component<actual_b_component)
        return 0;

      new_result=(Integer) (actual_component/actual_b_component);

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }
  }

#endif  // SUPPORT_VARIABLES_FIRST


#ifdef SUPPORT_VARIABLES_LAST

  for(short i=0;i<size_of_support_vectors;i++)
    // test support variables

    if(b.head_support&(1<<i))
      // bit i of b.head_support is 1
    {
      short j=_number_of_variables-1-i;
      new_result=(Integer) (-exponent_vector[j] / b.exponent_vector[j]);
      // remember that exponent_vector[_number_of_variables-1-i]<0 !
      // (tail support contains the head support of b)

      if(new_result==0)
        // -exponent_vector[_number_of_variables-1-i]
        // <b.exponent_vector[_number_of_variables-1-i]
        return 0;

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }


  for(short i=size_of_support_vectors;i<_number_of_variables;i++)
    // test non-support variables
    // from now on we need explicit sign tests
  {
    short j=_number_of_variables-1-i;
    Integer actual_b_component=b.exponent_vector[j];

    if(actual_b_component>0)
      // else variable number_of_variables-1-i is not involved in the head of b
    {
      Integer actual_component=-exponent_vector[j];

      if(actual_component<actual_b_component)
        return 0;

      new_result=(Integer) (actual_component/actual_b_component);

      // new_result>=1
      if((new_result<result) || (result==-1))
        // new (or first) minimum
        result=new_result;
    }
  }

#endif  // SUPPORT_VARIABLES_LAST


#endif  // SUPPORT_DRIVEN_METHODS


  return(result);
}




int binomial::reduce_head_by(const binomial& b, const term_ordering& w)
{
  Integer reduction_number=head_reductions_by(b);
  if(reduction_number<=0)
    return 0;

  for(short i=0;i<_number_of_variables;i++)
    exponent_vector[i]-=(reduction_number * b.exponent_vector[i]);
  // multiple reduction
  // reduction corresponds to subtraction of vectors

  short sign=w.compare_to_zero(exponent_vector);


#ifdef NO_SUPPORT_DRIVEN_METHODS

  if(sign==0)
    // binomial reduced to zero
    return 2;

  for(short i=0;i<_number_of_variables;i++)
    exponent_vector[i]*=sign;

#endif  // NO_SUPPORT_DRIVEN_METHODS


#ifdef SUPPORT_DRIVEN_METHODS

  head_support=0;
  tail_support=0;

  if(sign==0)
    // binomial reduced to zero
    return 2;

  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);


  // recompute the support vectors

#ifdef SUPPORT_VARIABLES_FIRST

  for(short i=0;i<_number_of_variables;i++)
  {

    Integer& actual_entry=exponent_vector[i];
    // to avoid unnecessary pointer arithmetic

    actual_entry*=sign;

    if(i<size_of_support_vectors)
      if(actual_entry>0)
        head_support|=(1<<i);
      else
        if(actual_entry<0)
          tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_FIRST


#ifdef SUPPORT_VARIABLES_LAST

  for(short i=0;i<_number_of_variables;i++)
  {
    Integer& actual_entry=exponent_vector[_number_of_variables-1-i];
    // to avoid unneccessary pointer arithmetic

    actual_entry*=sign;

    if(i<size_of_support_vectors)
      if(actual_entry>0)
        head_support|=(1<<i);
      else
        if(actual_entry<0)
          tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_LAST


#endif  // SUPPORT_DRIVEN_METHODS

  return 1;
}




int binomial::reduce_tail_by(const binomial& b, const term_ordering& w)
{
  Integer reduction_number=tail_reductions_by(b);
  if(reduction_number<=0)
    return 0;

  for(short i=0;i<_number_of_variables;i++)
    exponent_vector[i]+=(reduction_number * b.exponent_vector[i]);
  // multiple reduction
  // reduction corresponds to addition of vectors

  // a tail reduction does not require a sign check


#ifdef SUPPORT_DRIVEN_METHODS

  head_support=0;
  tail_support=0;

  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);


  // recompute the support vectors

#ifdef SUPPORT_VARIABLES_FIRST

  for(short i=0;i<_number_of_variables;i++)
  {

    Integer& actual_entry=exponent_vector[i];
    // to avoid unnecessary pointer arithmetic

    if(i<size_of_support_vectors)
      if(actual_entry>0)
        head_support|=(1<<i);
      else
        if(actual_entry<0)
          tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_FIRST


#ifdef SUPPORT_VARIABLES_LAST

  for(short i=0;i<_number_of_variables;i++)
  {
    Integer& actual_entry=exponent_vector[_number_of_variables-1-i];
    // to avoid unneccessary pointer arithmetic

    if(i<size_of_support_vectors)
      if(actual_entry>0)
        head_support|=(1<<i);
      else
        if(actual_entry<0)
          tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_LAST


#endif  // SUPPORT_DRIVEN_METHODS

  return 1;
}




binomial& S_binomial(const binomial& a, const binomial& b,
                     const term_ordering& w)
{
  binomial* S_bin=new binomial(a._number_of_variables);
  binomial& result=*S_bin;
  // Note that we allocate memory for the result binomial. We often use
  // pointers or references as argument and return types because the
  // generating binomials of an ideal are kept in lists. For the performance
  // of Buchberger's algorithm it it very important to avoid local copies
  // of binomials, so a lot of attention is paid on the choice of argument
  // and return types. As this choice is done in order to improve performance,
  // it might be a bad choice with respect to code reuse (there are some
  // dangerous constructions).

  for(short i=0;i<result._number_of_variables;i++)
    result.exponent_vector[i]=a.exponent_vector[i]-b.exponent_vector[i];
  // The S-binomial corresponds to the vector difference.

  // compute head and tail
  short sign=w.compare_to_zero(result.exponent_vector);


#ifdef NO_SUPPORT_DRIVEN_METHODS

  if(sign==0)
    // binomial reduced to zero
    return result;

  for(short i=0;i<result._number_of_variables;i++)
    result.exponent_vector[i]*=sign;

#endif  // NO_SUPPORT_DRIVEN_METHODS


#ifdef SUPPORT_DRIVEN_METHODS

  result.head_support=0;
  result.tail_support=0;

  if(sign==0)
    // binomial reduced to zero
    return result;

  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);


  // recompute the support vectors

#ifdef SUPPORT_VARIABLES_FIRST

  for(short i=0;i<result._number_of_variables;i++)
  {

    Integer& actual_entry=result.exponent_vector[i];
    // to avoid unnecessary pointer arithmetic

    actual_entry*=sign;

    if(i<size_of_support_vectors)
      if(actual_entry>0)
        result.head_support|=(1<<i);
      else
        if(actual_entry<0)
          result.tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_FIRST


#ifdef SUPPORT_VARIABLES_LAST

  for(short i=0;i<result._number_of_variables;i++)
  {
    Integer& actual_entry=result.exponent_vector
      [result._number_of_variables-1-i];
    // to avoid unneccessary pointer arithmetic

    actual_entry*=sign;

    if(i<size_of_support_vectors)
      if(actual_entry>0)
        result.head_support|=(1<<i);
      else
        if(actual_entry<0)
          result.tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_LAST


#endif  // SUPPORT_DRIVEN_METHODS


  return result;
}




///////////// criteria for unnecessary S-pairs ///////////////////////////////

// The criteria are programmed in a way that tries to minimize pointer
// arithmetic. Therefore the code may appear a little bit inflated.




BOOLEAN relatively_prime(const binomial& a, const binomial& b)
{

#ifdef NO_SUPPORT_DRIVEN_METHODS

  // look at all variables
  for(short i=0;i<a._number_of_variables;i++)
    if((a.exponent_vector[i]>0) && (b.exponent_vector[i]>0))
      return FALSE;

  return TRUE;

#endif  // NO_SUPPORT_DRIVEN_METHODS


#ifdef SUPPORT_DRIVEN_METHODS

  if((a.head_support & b.head_support)!=0)
  // common support variable in the heads
    return FALSE;

  // no common support variable in the heads, look at remaining variables
  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);


#ifdef SUPPORT_VARIABLES_FIRST

  for(short i=size_of_support_vectors;i<a._number_of_variables;i++)
    if((a.exponent_vector[i]>0) && (b.exponent_vector[i]>0))
      return FALSE;

  return TRUE;

#endif  // SUPPORT_VARIABLES_FIRST


#ifdef SUPPORT_VARIABLES_LAST

  for(short i=a._number_of_variables-1-size_of_support_vectors;i>=0;i--)
    if((a.exponent_vector[i]>0) && (b.exponent_vector[i]>0))
      return FALSE;

  return TRUE;

#endif  // SUPPORT_VARIABLES_LAST


#endif  // SUPPORT_DRIVEN_METHODS

}




BOOLEAN M(const binomial& a, const binomial& b, const binomial& c)
// Returns TRUE iff lcm(head(a),head(c)) divides properly lcm(head(b),head(c)).
// This is checked by comparing the positive components of the exponent
// vectors.
{


#ifdef SUPPORT_DRIVEN_METHODS

  long b_or_c=b.head_support|c.head_support;

  if((a.head_support|b_or_c) != b_or_c)
    return FALSE;
  // The support of lcm(head(a),head(c)) equals the union of the head supports
  // of a and c. The above condition verifies if the support of
  // lcm(head(a),head(c)) is contained in the support of lcm(head(b),head(c))
  // by checking if head a involves a variable that is not involved in
  // head(b) or head(c).

#endif  // SUPPORT_DRIVEN_METHODS


  BOOLEAN properly=FALSE;

  for(short i=0;i<a._number_of_variables;i++)
  {
    Integer a_exponent=a.exponent_vector[i];
    Integer b_exponent=b.exponent_vector[i];
    Integer c_exponent=c.exponent_vector[i];
    Integer m1=MAXIMUM(a_exponent,c_exponent);
    Integer m2=MAXIMUM(b_exponent,c_exponent);

    if(m1>0)
    {
      if(m1>m2)
        return FALSE;
      if(m1<m2)
        properly=TRUE;
    }
  }

  return properly;
}




BOOLEAN F(const binomial& a, const binomial& b, const binomial& c)
// verifies if lcm(head(a),head(c))=lcm(head(b),head(c))
{

#ifdef SUPPORT_DRIVEN_METHODS

  if((a.head_support|c.head_support)!=(b.head_support|c.head_support))
    return FALSE;
  // The above condition verifies if the support of lcm(head(a),head(c))
  // equals the support of lcm(head(b),head(c)).

#endif  // SUPPORT_DRIVEN_METHODS

  for(short i=0;i<a._number_of_variables;i++)
  {
    Integer a_exponent=a.exponent_vector[i];
    Integer b_exponent=b.exponent_vector[i];
    Integer c_exponent=c.exponent_vector[i];
    Integer m1=MAXIMUM(a_exponent,c_exponent);
    Integer m2=MAXIMUM(b_exponent,c_exponent);

    if((m1!=m2) && (m1>0 || m2>0))
      return FALSE;
  }

  return TRUE;
}




BOOLEAN B(const binomial& a, const binomial& b, const binomial& c)
// verifies if head(a) divides lcm(head(b),head(c)) and
// lcm(head(a),head(b))!=lcm(head(b),head(c))!=lcm(head(a),head(c))
{

#ifdef SUPPORT_DRIVEN_METHODS

  long a_or_b=a.head_support|b.head_support;
  long a_or_c=a.head_support|c.head_support;
  long b_or_c=b.head_support|c.head_support;

  if((a.head_support & b_or_c)!=a.head_support)
    return FALSE;
  // The above condition verifies if the support of head(a) is contained in
  // the support of lcm(head(b),head(c)).

  if( (a_or_c != b_or_c) && (a_or_b != b_or_c))
    // Then the inequality conditions are guaranteed...
  {
    for(short i=0;i<a._number_of_variables;i++)
    {
      Integer b_exponent=b.exponent_vector[i];
      Integer c_exponent=c.exponent_vector[i];

      if(a.exponent_vector[i]>MAXIMUM(b_exponent,c_exponent))
        return FALSE;
    }

    return (TRUE);
  }


  if(a_or_b != b_or_c)
    // Then the first inequality conditions is guaranteed...
    // Verifie only the second.
  {
    BOOLEAN not_equal=FALSE;

    for(short i=0;i<a._number_of_variables;i++)
    {
      Integer a_exponent=a.exponent_vector[i];
      Integer b_exponent=b.exponent_vector[i];
      Integer c_exponent=c.exponent_vector[i];
      Integer m=MAXIMUM(b_exponent, c_exponent);

      if(a_exponent>m)
        return FALSE;

      if(MAXIMUM(a_exponent,c_exponent) != m)
         not_equal=TRUE;
    }
    return(not_equal);
  }


  if( a_or_c != b_or_c )
    // Then the second inequality conditions is guaranteed...
    // Verifie only the first.
  {
    BOOLEAN not_equal=FALSE;

    for(short i=0;i<a._number_of_variables;i++)
    {
      Integer a_exponent=a.exponent_vector[i];
      Integer b_exponent=b.exponent_vector[i];
      Integer c_exponent=c.exponent_vector[i];
      Integer m=MAXIMUM(b_exponent, c_exponent);

      if(a_exponent > m)
        return FALSE;

      if(MAXIMUM(a_exponent,b_exponent) != m)
        not_equal=TRUE;
    }
    return(not_equal);
  }

#endif  // SUPPORT_DRIVEN_METHODS


  BOOLEAN not_equal_1=FALSE;
  BOOLEAN not_equal_2=FALSE;

  for(short i=0;i<a._number_of_variables;i++)
  {
    Integer a_exponent=a.exponent_vector[i];
    Integer b_exponent=b.exponent_vector[i];
    Integer c_exponent=c.exponent_vector[i];
    Integer m=MAXIMUM(b_exponent, c_exponent);

    if(a_exponent > m)
      return FALSE;

    if(MAXIMUM(a_exponent,b_exponent) != m)
      not_equal_1=TRUE;
    if(MAXIMUM(a_exponent,c_exponent) != m)
      not_equal_2=TRUE;
  }

  return (not_equal_1 && not_equal_2);

}




BOOLEAN second_crit(const binomial& a, const binomial& b,
                    const binomial& c)
// verifies if head(a) divides lcm(head(b),head(c))
{

#ifdef SUPPORT_DRIVEN_METHODS

  if((a.head_support & (b.head_support|c.head_support))!=a.head_support)
    return FALSE;
  // The above condition verifies if the support of head(a) is contained in
  // the support of lcm(head(b),head(c))

#endif  // SUPPORT_DRIVEN_METHODS.

  for(short i=0;i<a._number_of_variables;i++)
  {
    Integer b_exponent=b.exponent_vector[i];
    Integer c_exponent=c.exponent_vector[i];

    if(a.exponent_vector[i]>MAXIMUM(b_exponent,c_exponent))
      return FALSE;
  }

  return (TRUE);
}




//////// special routines needed by the IP-algorithms ///////////////////////




BOOLEAN binomial::involves_elimination_variables(const term_ordering& w)
{
// The use of support information would require the distinction of various
// cases here (relation between the number of variables to eliminate
// and the number of support variables) and be quite difficult.
// It is doubtful if this would improve performance.
// As this function is not used in Buchberger´s algorithm (and therefore
// rather rarely), I renounce to implement this.

  for(short i=0;i<w.number_of_elimination_variables();i++)
    // elimination variables are always the last ones
    if(exponent_vector[_number_of_variables-1-i]!=0)
      return TRUE;

  return FALSE;
}




BOOLEAN binomial::drop_elimination_variables(const term_ordering& w)
{
  _number_of_variables-=w.number_of_elimination_variables();
  // dangerous (no compatibility check)!!

  // copy components of interest to save memory
  // the leading term has to be recomputed!!

  Integer *aux=exponent_vector;
  exponent_vector=new Integer[_number_of_variables];

  if(w.weight(aux)>=0)
    for(short i=0;i<_number_of_variables;i++)
      exponent_vector[i]=aux[i];
  else
    for(short i=0;i<_number_of_variables;i++)
      exponent_vector[i]=-aux[i];

  delete[] aux;


#ifdef SUPPORT_DRIVEN_METHODS

  // Recompute head and tail.
  // Normally, this routine is only called for binomials that do not involve
  // the variables to eliminate. But if SUPPORT_VARIABLES_LAST is enabled,
  // the support changes in spite of this. Therefore, the support is
  // recomputed... For the same reasons as mentionned in the preceeding
  // routine, the existing support information is not used.

  head_support=0;
  tail_support=0;
  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);
  if(size_of_support_vectors>_number_of_variables)
    size_of_support_vectors=_number_of_variables;


#ifdef SUPPORT_VARIABLES_FIRST

  for(short i=0;i<size_of_support_vectors;i++)
  {
    Integer actual_entry=exponent_vector[i];
    if(actual_entry>0)
      head_support|=(1<<i);
    else
      if(actual_entry[i]<0)
        tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_FIRST


#ifdef SUPPORT_VARIABLES_LAST

  for(short i=0;i<size_of_support_vectors;i++)
  {
    Integer actual_entry=exponent_vector[_number_of_variables-1-i];
    if(actual_entry>0)
      head_support|=(1<<i);
    else
      if(actual_entry<0)
        tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_LAST


#endif  // SUPPORT_DRIVEN_METHODS
  return TRUE;

}




BOOLEAN binomial::drop_last_weighted_variable(const term_ordering& w)
{
  _number_of_variables--;
  // dangerous!!

  // copy components of interest to save memory
  // the leading term has to be recomputed!!

  Integer *aux=exponent_vector;
  exponent_vector=new Integer[_number_of_variables];

  short last_weighted_variable=w.number_of_weighted_variables()-1;
  aux[last_weighted_variable]=0;
  // set last component to zero, so it cannot influence the weight

  if(w.weight(aux)>=0)
  {
    for(short i=0;i<last_weighted_variable;i++)
      exponent_vector[i]=aux[i];
    for(short i=last_weighted_variable;i<_number_of_variables;i++)
      exponent_vector[i]=aux[i+1];
  }
  else
  {
    for(short i=0;i<last_weighted_variable;i++)
      exponent_vector[i]=-aux[i];
     for(short i=last_weighted_variable;i<_number_of_variables;i++)
      exponent_vector[i]=-aux[i+1];
  }

  delete[] aux;


#ifdef SUPPORT_DRIVEN_METHODS

  // Recompute head and tail.
  // Normally, this routine is only called for binomials that do not involve
  // the variable to be dropped. But if SUPPORT_VARIABLES_LAST is enabled,
  // the support changes in spite of this. Therefore, the support is
  // recomputed... For the same reasons as mentionned in the preceeding
  // routines, the existing support information is not used.

  head_support=0;
  tail_support=0;
  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);
  if(size_of_support_vectors>_number_of_variables)
    size_of_support_vectors=_number_of_variables;


#ifdef SUPPORT_VARIABLES_FIRST

  for(short i=0;i<size_of_support_vectors;i++)
  {
    Integer actual_entry=exponent_vector[i];
    if(actual_entry>0)
      head_support|=(1<<i);
    else
      if(actual_entry<0)
        tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_FIRST


#ifdef SUPPORT_VARIABLES_LAST

  for(short i=0;i<size_of_support_vectors;i++)
  {
    Integer actual_entry=exponent_vector[_number_of_variables-1-i];
    if(actual_entry>0)
      head_support|=(1<<i);
    else
      if(actual_entry<0)
        tail_support|=(1<<i);
  }

#endif  // SUPPORT_VARIABLES_LAST

#endif  // SUPPORT_DRIVEN_METHODS
  return TRUE;
}




int binomial::adapt_to_term_ordering(const term_ordering& w)
{

  if(w.compare_to_zero(exponent_vector)<0)
  {
    // then exchange head and tail
    for(short i=0;i<_number_of_variables;i++)
      exponent_vector[i]*=(-1);


#ifdef SUPPORT_DRIVEN_METHODS

    unsigned long swap=head_support;
    head_support=tail_support;
    tail_support=swap;

#endif


    return -1;
    // binomial changed
  }

  else
    return 1;
    // binomial unchanged
}




binomial& binomial::swap_variables(const short& i, const short& j)
{


#ifdef SUPPORT_DRIVEN_METHODS

  // First adjust head_support and tail_support.

  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);
  if(size_of_support_vectors>_number_of_variables)
    size_of_support_vectors=_number_of_variables;


#ifdef SUPPORT_VARIABLES_FIRST

  if(i<size_of_support_vectors)
    // else i is no support variable
  {
    if(exponent_vector[j]>0)
    // bit i will be 1 in the new head_support, 0 in the new tail_support
    {
      head_support|=(1<<i);
      // bit i is set to 1

      tail_support&=~(1<<i);
      // bit i is set to 0
      // (in the complement ~(1<<i) all bits are 1 except from bit i)
    }

    if(exponent_vector[j]==0)
    // bit i will be 0 in the new head_support, 0 in the new tail_support
    {
      head_support&=~(1<<i);
      // bit i is set to 0

      tail_support&=~(1<<i);
      // bit i is set to 0
    }

    if(exponent_vector[j]<0)
    // bit i will be 0 in the new head_support, 1 in the new tail_support
    {
      head_support&=~(1<<i);
      // bit i is set to 0

      tail_support|=(1<<i);
      // bit i is set to 1
    }
  }


  if(j<size_of_support_vectors)
    // else j is no support variable
  {
    if(exponent_vector[i]>0)
    // bit j will be 1 in the new head_support, 0 in the new tail_support
    {
      head_support|=(1<<j);
      // bit j is set to 1

      tail_support&=~(1<<j);
      // bit j is set to 0
      // (in the complement ~(1<<j) all bits are 1 except from bit j)
    }

    if(exponent_vector[i]==0)
    // bit j will be 0 in the new head_support, 0 in the new tail_support
    {
      head_support&=~(1<<j);
      // bit j is set to 0

      tail_support&=~(1<<j);
      // bit j is set to 0
    }

    if(exponent_vector[i]<0)
    // bit j will be 0 in the new head_support, 1 in the new tail_support
    {
      head_support&=~(1<<j);
      // bit j is set to 0

      tail_support|=(1<<j);
      // bit j is set to 1
    }
  }

#endif  // SUPPORT_VARIABLES_FIRST


#ifdef SUPPORT_VARIABLES_LAST

  // Using SUPPORT_VARIABLES_LAST, bit k of the support vectors
  // corresponds to exponent_vector[_number_of_variables-1-k],
  // hence bit _number_of_variables-1-i to exponent_vector[i].

  if(i>=_number_of_variables-size_of_support_vectors)
    // else i is no support variable
  {
    if(exponent_vector[j]>0)
    // bit _number_of_variables-1-i will be 1 in the new head_support,
    // 0 in the new tail_support
    {
      short k=_number_of_variables-1-i;

      head_support|=(1<<k);
      // bit _number_of_variables-1-i is set to 1

      tail_support&=~(1<<k);
      // bit _number_of_variables-1-i is set to 0
      // (in the complement ~(1<<(_number_of_variables-1-i)) all bits are 1
      // except from bit _number_of_variables-1-i)
    }

    if(exponent_vector[j]==0)
    // bit _number_of_variables-1-i will be 0 in the new head_support,
    // 0 in the new tail_support
    {
      short k=_number_of_variables-1-i;

      head_support&=~(1<<k);
      // bit _number_of_variables-1-i is set to 0

      tail_support&=~(1<<k);
      // bit _number_of_variables-1-i is set to 0
    }

    if(exponent_vector[j]<0)
    // bit _number_of_variables-1-i will be 0 in the new head_support,
     // 1 in the new tail_support
    {
      short k=_number_of_variables-1-i;

      head_support&=~(1<<k);
      // bit _number_of_variables-1-i is set to 0

      tail_support|=(1<<k);
      // bit _number_of_variables-1-i is set to 1
    }
  }


  if(j>=_number_of_variables-size_of_support_vectors)
      // else j is no support variable
  {
   if(exponent_vector[i]>0)
    // bit _number_of_variables-1-j will be 1 in the new head_support,
    // 0 in the new tail_support
    {
      short k=_number_of_variables-1-j;

      head_support|=(1<<k);
      // bit _number_of_variables-1-j is set to 1

      tail_support&=~(1<<k);
      // bit _number_of_variables-1-j is set to 0
      // (in the complement ~(1<<(_number_of_variables-1-j)) all bits are 1
      // except from bit _number_of_variables-1-j)
    }

    if(exponent_vector[i]==0)
    // bit _number_of_variables-1-j will be 0 in the new head_support,
    // 0 in the new tail_support
    {
      short k=_number_of_variables-1-j;

      head_support&=~(1<<k);
      // bit _number_of_variables-1-j is set to 0

      tail_support&=~(1<<k);
      // bit _number_of_variables-1-j is set to 0
    }

    if(exponent_vector[i]<0)
    // bit _number_of_variables-1-j will be 0 in the new head_support,
     // 1 in the new tail_support
    {
      short k=_number_of_variables-1-j;

      head_support&=~(1<<k);
      // bit _number_of_variables-1-j is set to 0

      tail_support|=(1<<k);
      // bit _number_of_variables-1-j is set to 1
    }
  }

#endif  // SUPPORT_VARIABLES_LAST

#endif  // SUPPORT_DRIVEN_METHODS


// Now swap the components.

  Integer swap=exponent_vector[j];
  exponent_vector[j]=exponent_vector[i];
  exponent_vector[i]=swap;

  return *this;

}

binomial& binomial::flip_variable(const short& i)
{

  if(exponent_vector[i]==0)
    // binomial does not involve variable to flip
    return *this;


#ifdef SUPPORT_DRIVEN_METHODS

// First adjust head_support and tail_support.

  short size_of_support_vectors=CHAR_BIT*sizeof(unsigned long);
  if(size_of_support_vectors>_number_of_variables)
    size_of_support_vectors=_number_of_variables;


#ifdef SUPPORT_VARIABLES_FIRST

  if(i<size_of_support_vectors)
    // else i is no support variable
  {
    if(exponent_vector[i]>0)
      // variable i will be moved from head to tail
    {
      head_support&=~(1<<i);
      // bit i is set to 0

      tail_support|=(1<<i);
      // bit i is set to 1
    }

    else
      // variable i will be moved from tail to head
      // remember that exponent_vector[i]!=0
    {
      tail_support&=~(1<<i);
      // bit i is set to 0

      head_support|=(1<<i);
      // bit i is set to 1
    }
  }
#endif  // SUPPORT_VARIABLES_FIRST

#ifdef SUPPORT_VARIABLES_LAST

  // Using SUPPORT_VARIABLES_LAST, bit k of the support vectors
  // corresponds to exponent_vector[_number_of_variables-1-k],
  // hence bit _number_of_variables-1-i to exponent_vector[i].

  if(i>=_number_of_variables-size_of_support_vectors)
    // else i is no support variable
  {
    if(exponent_vector[i]>0)
    // variable i will be moved from head to tail
    {
      short k=_number_of_variables-1-i;

      head_support&=~(1<<k);
      // bit _number_of_variables-1-i is set to 0

      tail_support|=(1<<k);
      // bit _number_of_variables-1-i is set to 1

     }

    else
    // variable i will be moved from tail to head
    {
      short k=_number_of_variables-1-i;

      tail_support&=~(1<<k);
      // bit _number_of_variables-1-i is set to 0

      head_support|=(1<<k);
      // bit _number_of_variables-1-i is set to 1

    }
  }
#endif  // SUPPORT_VARIABLES_LAST


#endif  // SUPPORT_DRIVEN_METHODS

  // Now flip the variable.

  exponent_vector[i]*=-1;

}

////////////////////////// output /////////////////////////////////////////

void binomial::print() const
{
  printf("(");
  for(short i=0;i<_number_of_variables-1;i++)
    printf("%6d,",exponent_vector[i]);
  printf("%6d)\n",exponent_vector[_number_of_variables-1]);
}

void binomial::print_all() const
{
  print();

#ifdef SUPPORT_DRIVEN_METHODS

  printf("head: %ld, tail %ld\n",head_support,tail_support);

#endif  // SUPPORT_DRIVEN_METHODS
}

void binomial::print(FILE* output) const
{
  fprintf(output,"(");
  for(short i=0;i<_number_of_variables-1;i++)
    fprintf(output,"%6d,",exponent_vector[i]);
  fprintf(output,"%6d)\n",exponent_vector[_number_of_variables-1]);
}

void binomial::print_all(FILE* output) const
{
  print(output);

#ifdef SUPPORT_DRIVEN_METHODS

  fprintf(output,"head: %ld, tail %ld\n",head_support,tail_support);

#endif  // SUPPORT_DRIVEN_METHODS
}

void binomial::print(ofstream& output) const
{
  output<<"(";
  for(short i=0;i<_number_of_variables-1;i++)
    output<<setw(6)<<exponent_vector[i]<<",";
  output<<setw(6)<<exponent_vector[_number_of_variables-1]<<")"<<endl;
}

void binomial::print_all(ofstream& output) const
{
  print(output);

#ifdef SUPPORT_DRIVEN_METHODS

  output<<"head: "<<setw(16)<<head_support<<", tail: "<<setw(16)
        <<tail_support<<endl;

#endif  // SUPPORT_DRIVEN_METHODS
}

void binomial::format_print(ofstream& output) const
{
  for(short i=0;i<_number_of_variables;i++)
    output<<setw(6)<<exponent_vector[i];
  output<<endl;
}

#endif  // BINOMIAL_CC
