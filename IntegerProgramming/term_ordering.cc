/// term_ordering.cc

/// implementation of class term ordering

#ifndef TERM_ORDERING_CC
#define TERM_ORDERING_CC

#include "binomial__term_ordering.h"

////////////////////// constructors and destructor ////////////////////////////////////////////////////

term_ordering::term_ordering(const BOOLEAN& _homogeneous)
    :homogeneous(_homogeneous)
{
  weight_vector=NULL;
  weighted_block_size=0;
  elimination_block_size=0;
}




term_ordering::term_ordering(const short& number_of_weighted_variables,
                             const float* weights,
                             const short& _weighted_ordering,
                             const BOOLEAN& _homogeneous)
    :weighted_block_size(number_of_weighted_variables),
     homogeneous(_homogeneous)
{
  if((_weighted_ordering<4) || (_weighted_ordering>7))
    /// unknown ordering refining the weight, set "error flag"
    weighted_block_size=-1;
  else
    weighted_ordering=_weighted_ordering;

  if(weighted_block_size<0)
    /// argument out of range, set "error flag"
    weighted_block_size=-1;

  if(weighted_block_size>0)
  {
    weight_vector=new float[weighted_block_size];

    BOOLEAN negative_weight=FALSE;
    BOOLEAN zero_weight=FALSE;
    /// for checking the input

    for(short i=0;i<weighted_block_size;i++)
    {
      weight_vector[i]=weights[i];
      /// initialize weight vector with weights

      if(weight_vector[i]<0)
        negative_weight=TRUE;
      if(weight_vector[i]==0)
        zero_weight=TRUE;
    }

    if(negative_weight==TRUE)
      cerr<<"\nWARNING: term_ordering::term_ordering(const short&, "
        "const float*, const short&):\nWeight vector with negative components"
        " does not define a well ordering"<<endl;

    if((weighted_ordering==W_REV_LEX) && (zero_weight==TRUE))
      cerr<<"\nWARNING: term_ordering::term_ordering(const short&, "
        "const float*, const short&):\nZero weights refined by a reverse "
        "lexicographical ordering do not define a well ordering"<<endl;

  }
  else
    if(weighted_block_size<0)
      cerr<<"\nWARNING:term_ordering::term_ordering(const short&, "
        "const float*, const short&):\nBad input in term ordering "
        "constructor"<<endl;

  elimination_block_size=0;

}




term_ordering::term_ordering(const short& number_of_weighted_variables,
                             const float* weights,
                             const short& _weighted_ordering,
                             const short& number_of_elimination_variables,
                             const short& _elimination_ordering,
                             const BOOLEAN& _homogeneous)
    :weighted_block_size(number_of_weighted_variables),
     elimination_block_size(number_of_elimination_variables),
     homogeneous(_homogeneous)
{
  if((_weighted_ordering<4) || (_weighted_ordering>7))
    /// unknown ordering refining the weight, set "error flag"
    weighted_block_size=-1;
  else
    weighted_ordering=_weighted_ordering;

  if((_elimination_ordering<1) || (_elimination_ordering>3))
    /// unknown ordering on the elimination variables, set "error flag"
    weighted_block_size=-1;
  else
    elimination_ordering=_elimination_ordering;

  if((weighted_block_size<0)||(elimination_block_size<0))
    /// argument out of range, set "error flag"
    weighted_block_size=-1;

  if(weighted_block_size>0)
  {
    weight_vector=new float[weighted_block_size];

    BOOLEAN negative_weight=FALSE;
    BOOLEAN zero_weight=FALSE;
    /// for checking the input

    for(short i=0;i<weighted_block_size;i++)
    {
      weight_vector[i]=weights[i];
      /// initialize weight vector with weights

      if(weight_vector[i]<0)
        negative_weight=TRUE;
      if(weight_vector[i]==0)
        zero_weight=TRUE;
    }

    if(negative_weight==TRUE)
      cerr<<"\nWARNING:term_ordering::term_ordering(const short&, "
        "const float*, const short&, const short&, const short&):\n"
        "Weight vector with negative components does not define "
        "a well ordering"<<endl;

    if((weighted_ordering==W_REV_LEX) && (zero_weight==TRUE))
      cerr<<"\nWARNING:term_ordering::term_ordering(const short&, "
        "const float*, const short&, const short&, const short&):\n"
        "Zero weights refined by a reverse lexicographical "
        "ordering do not define a well ordering"<<endl;

  }
  else
    if(weighted_block_size<0)
      cerr<<"\nWARNING:term_ordering::term_ordering(const short&, "
        "const float*, const short&, const short&, const short&):\n"
        "Bad input in term ordering constructor"<<endl;
}




term_ordering::term_ordering(ifstream& input, const short& _weighted_ordering,
                             const BOOLEAN& _homogeneous)
    :homogeneous(_homogeneous)
{

  if((_weighted_ordering<4) || (_weighted_ordering>7))
    /// unknown ordering refining the weight, set "error flag"
    weighted_block_size=-1;
  else
    weighted_ordering=_weighted_ordering;


  input>>weighted_block_size;
  if(!input)
    /// input failure, set "error flag"
    weighted_block_size=-2;
  if(weighted_block_size<0)
    /// input out of range, set error flag
    weighted_block_size=-1;

  if(weighted_block_size>0)
  {
    weight_vector=new float[weighted_block_size];

    BOOLEAN negative_weight=FALSE;
    BOOLEAN zero_weight=FALSE;
    /// for checking the input

    for(short i=0;i<weighted_block_size;i++)
    {
      input>>weight_vector[i];

      if(!input)
      /// input failure, set "error flag"
      {
        weighted_block_size=-2;
        cerr<<"\nWARNING: term_ordering::term_ordering(ifstream&, const "
          "short&):\nInput failed reading term ordering from ofstream"<<endl;
        break;
      }

      if(weight_vector[i]<0)
        negative_weight=TRUE;
      if(weight_vector[i]==0)
        zero_weight=TRUE;
    }

    if(negative_weight==TRUE)
      cerr<<"\nWARNING: term_ordering::term_ordering(ifstream&, const short&)"
        ":\nWeight vector with negative components does not define "
        "a well ordering"<<endl;

    if((weighted_ordering==W_REV_LEX) && (zero_weight==TRUE))
      cerr<<"\nWARNING: term_ordering::term_ordering(ifstream&, const short&)"
        ":\nZero weights refined by a reverse lexicographical "
        "ordering do not define a well ordering"<<endl;
  }
  else
    if(weighted_block_size<0)
      cerr<<"\nWARNING: term_ordering::term_ordering(ifstream&, const short&)"
        ":\nBuilding a term ordering from a corrupt one"<<endl;

  elimination_block_size=0;
}




term_ordering::term_ordering(const short& n, ifstream& input,
                             const short& _weighted_ordering,
                             const BOOLEAN& _homogeneous)
    :homogeneous(_homogeneous)
{
  if((_weighted_ordering<4) || (_weighted_ordering>7))
    /// unknown ordering refining the weight, set "error flag"
    weighted_block_size=-1;
  else
    weighted_ordering=_weighted_ordering;

  if(n<0)
    /// input out of range, set error flag
    weighted_block_size=-1;
  else
    weighted_block_size=n;

  if(weighted_block_size>0)
  {
    weight_vector=new float[weighted_block_size];

    BOOLEAN negative_weight=FALSE;
    BOOLEAN zero_weight=FALSE;
    /// for checking the input

    for(short i=0;i<weighted_block_size;i++)
    {
      input>>weight_vector[i];

      if(!input)
      /// input failure, set "error flag"
      {
        weighted_block_size=-2;
        cerr<<"\nWARNING: term_ordering::term_ordering(const short&, "
          "ifstream&, const short&):\nInput failed reading term ordering "
          "from ofstream"<<endl;
        break;
      }

      if(weight_vector[i]<0)
      {
        cout << "neg found at i="<<i<<":" <<weight_vector[i] <<"\n";
        negative_weight=TRUE;
      }
      if(weight_vector[i]==0)
        zero_weight=TRUE;
    }

    if(negative_weight==TRUE)
      cerr<<"\nWARNING: term_ordering::term_ordering(const short&, ifstream&, "
        "const short&):\nWeight vector with negative components does not "
        "define a well ordering"<<endl;

    if((weighted_ordering==W_REV_LEX) && (zero_weight==TRUE))
      cerr<<"\nWARNING: term_ordering::term_ordering(const short&, ifstream&, "
        "const short&):\nZero weights refined by a reverse lexicographical "
        "ordering do not define a well ordering"<<endl;
  }
  else
    if(weighted_block_size<0)
      cerr<<"\nWARNING: term_ordering::term_ordering(const short&, ifstream&, "
        "const short&):\n Building a term ordering from a corrupt one"<<endl;

  elimination_block_size=0;
}




term_ordering::term_ordering(const term_ordering& w)
    :weighted_block_size(w.weighted_block_size),
     weighted_ordering(w.weighted_ordering),
     elimination_block_size(w.elimination_block_size),
     elimination_ordering(w.elimination_ordering),
     homogeneous(w.homogeneous)
{
  if(weighted_block_size>0)
  {
    weight_vector=new float[weighted_block_size];
    for(short i=0;i<weighted_block_size;i++)
      weight_vector[i]=w.weight_vector[i];
  }
  else
    if(weighted_block_size<0)
      cerr<<"\nWARNING: term_ordering::term_ordering(const term_ordering&):\n"
        "Building a term ordering from a corrupt one"<<endl;

}




term_ordering::~term_ordering()
{
  if(weighted_block_size>0)
    delete[] weight_vector;
}




////////////////////// object properties /////////////////////////////////////////////////////////////




short term_ordering::number_of_weighted_variables() const
{
  return weighted_block_size;
}



short term_ordering::weight_refinement() const
{
  return weighted_ordering;
}



short term_ordering::number_of_elimination_variables() const
{
  return elimination_block_size;
}



short term_ordering::elimination_refinement() const
{
  return elimination_ordering;
}



BOOLEAN term_ordering::is_homogeneous() const
{
  return homogeneous;
}



short term_ordering::error_status() const
{
  if(weighted_block_size<0)
    return weighted_block_size;
  return 0;
}



BOOLEAN term_ordering::is_nonnegative() const
{
  for(int i=0;i<weighted_block_size;i++)
    if(weight_vector[i]<0)
      return FALSE;
  return TRUE;
}



BOOLEAN term_ordering::is_positive() const
{
  for(int i=0;i<weighted_block_size;i++)
    if(weight_vector[i]<=0)
      return FALSE;
  return TRUE;
}




////////////////////// frequently used comparison functions /////////////////////////////////




double term_ordering::weight(const Integer* v) const
{
  double result=0;
  for(short i=0;i<weighted_block_size;i++)
    result+=(weight_vector[i]*v[i]);
  return(result);
}




short term_ordering::compare_to_zero(const Integer* v) const
{
  unsigned short last_index=weighted_block_size+elimination_block_size;
  double w=0;

  /// First check the elimination variables.

  if(elimination_block_size>0)
    switch(elimination_ordering)
    {
        case LEX:

          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer actual_component=v[i];
            if(actual_component>0)
              return 1;
            if(actual_component<0)
              return -1;
          }

        break;

        case DEG_LEX:

          /// compute the degree
          for(short i=weighted_block_size;i<last_index;i++)
            w+=v[i];

          if(w>0)
            return 1;
          if(w<0)
            return -1;

          /// if the degree is zero:
          /// tie breaking with the lexicographical ordering
          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer actual_component=v[i];
            if(actual_component>0)
              return 1;
            if(actual_component<0)
              return -1;
          }

          break;

        case DEG_REV_LEX:

          ///compute the degree
          for(short i=weighted_block_size;i<last_index;i++)
            w+=v[i];

          if(w>0)
            return 1;
          if(w<0)
            return -1;
          /// if the degree is zero:
          /// tie breaking with the reverse lexicographical ordering
          for(short i=last_index-1;i>=weighted_block_size;i--)
          {
            Integer actual_component=v[i];
            if(actual_component<0)
              return 1;
            if(actual_component>0)
              return -1;
          }
    }


  /// When reaching this line, the vector components corresponding to
  /// elimination variables are all zero.
  /// Compute the weight.
  /// If the term ordering is a homogeneous one, this is superfluous.

  if(!homogeneous)
  {
    w=weight(v);
    if(w>0)
      return 1;
    if(w<0)
      return -1;
  }


  /// When reaching this line, the weight of the vector components corresponding
  /// to weighted variables is zero.
  /// Tie breaking with the term ordering refining the weight.

  switch(weighted_ordering)
  {
      case W_LEX:

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer actual_component=v[i];
          if(actual_component>0)
            return 1;
          if(actual_component<0)
            return -1;
        }

        break;

      case W_REV_LEX:

        for(short i=weighted_block_size-1;i>=0;i--)
        {
          Integer actual_component=v[i];
          if(actual_component<0)
            return 1;
          if(actual_component>0)
            return -1;
        }

        break;

      case W_DEG_LEX:

        for(short i=0;i<weighted_block_size;i++)
          w+=v[i];

        if(w>0)
          return 1;
        if(w<0)
          return -1;

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer actual_component=v[i];
          if(actual_component>0)
            return 1;
          if(actual_component<0)
            return -1;
        }

        break;

      case W_DEG_REV_LEX:

        for(short i=0;i<weighted_block_size;i++)
          w+=v[i];

        if(w>0)
          return 1;
        if(w<0)
          return -1;

        for(short i=weighted_block_size-1;i>=0;i--)
        {
          Integer actual_component=v[i];
          if(actual_component<0)
            return 1;
          if(actual_component>0)
            return -1;
        }

  }

  /// When reaching this line, the argument vector is the zero vector.

  return 0;

}




short term_ordering::compare(const binomial& bin1, const binomial& bin2) const
{
  unsigned short last_index=weighted_block_size+elimination_block_size;
  double w1=0;
  double w2=0;

  Integer* v1=bin1.exponent_vector;
  Integer* v2=bin2.exponent_vector;

  /// First compare the heads of the input binomials.
  /// The code is analogous to the routine compare_to_zero(...), except
  /// from the fact that we must consider the sign of the vector components.

  /// First check the elimination variables.

  if(elimination_block_size>0)
    switch(elimination_ordering)
    {
        case LEX:

          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer comp1=v1[i];
            Integer comp2=v2[i];

            if(comp1>0 || comp2>0)
            {
              if(comp1>comp2)
                return 1;
              if(comp1<comp2)
                return -1;
            }
          }

        break;

        case DEG_LEX:

          /// compute the degree of the heads in the elimination variables
          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer comp1=v1[i];
            Integer comp2=v2[i];

            if(comp1>0)
              w1+=comp1;
            if(comp2>0)
              w2+=comp2;
          }

          if(w1>w2)
            return 1;
          if(w1<w2)
            return -1;

          /// if the degree is equal:
          /// tie breaking with the lexicographical ordering
          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer comp1=v1[i];
            Integer comp2=v2[i];

            if(comp1>0 || comp2>0)
            {
              if(comp1>comp2)
                return 1;
              if(comp1<comp2)
                return -1;
            }
          }

          break;

        case DEG_REV_LEX:

          ///compute the degree of the heads in the elimination variables
          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer comp1=v1[i];
            Integer comp2=v2[i];

            if(comp1>0)
              w1+=comp1;
            if(comp2>0)
              w2+=comp2;
          }

          if(w1>w2)
            return 1;
          if(w1<w2)
            return -1;
          /// if the degree is equal:
          /// tie breaking with the reverse lexicographical ordering
          for(short i=last_index-1;i>=weighted_block_size;i--)
          {
            Integer comp1=v1[i];
            Integer comp2=v2[i];

            if(comp1>0 || comp2>0)
            {
              if(comp1<comp2)
                return 1;
              if(comp1>comp2)
                return -1;
            }
          }
    }


  /// When reaching this line, the heads are equal in the elimination
  /// variables.
  /// Compute the weight of the heads.

  w1=0;
  for(short i=0;i<weighted_block_size;i++)
  {
    Integer actual_component=v1[i];
    if(actual_component>0)
      w1+=actual_component*weight_vector[i];
  }

  w2=0;
  for(short i=0;i<weighted_block_size;i++)
  {
    Integer actual_component=v2[i];
    if(actual_component>0)
      w2+=actual_component*weight_vector[i];
  }

  if(w1>w2)
    return 1;
  if(w1<w2)
    return -1;


  /// When reaching this line, the weight of the heads in the weighted
  /// variables are equal.
  /// Tie breaking with the term ordering refining the weight.

  switch(weighted_ordering)
  {
      case W_LEX:

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer comp1=v1[i];
          Integer comp2=v2[i];

          if(comp1>0 || comp2>0)
          {
            if(comp1>comp2)
              return 1;
            if(comp1<comp2)
              return -1;
          }
        }

        break;

      case W_REV_LEX:

        for(short i=weighted_block_size-1;i>=0;i--)
        {
          Integer comp1=v1[i];
          Integer comp2=v2[i];

          if(comp1>0 || comp2>0)
          {
            if(comp1<comp2)
              return 1;
            if(comp1>comp2)
              return -1;
          }
        }

        break;

      case W_DEG_LEX:

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer comp1=v1[i];
          Integer comp2=v2[i];

          if(comp1>0)
            w1+=comp1;
          if(comp2>0)
            w2+=comp2;
        }

        if(w1>w2)
          return 1;
        if(w1<w2)
          return -1;

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer comp1=v1[i];
          Integer comp2=v2[i];

          if(comp1>0 || comp2>0)
          {
            if(comp1>comp2)
              return 1;
            if(comp1<comp2)
              return -1;
          }
        }

        break;

      case W_DEG_REV_LEX:

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer comp1=v1[i];
          Integer comp2=v2[i];

          if(comp1>0)
            w1+=comp1;
          if(comp2>0)
            w2+=comp2;
        }

        if(w1>w2)
          return 1;
        if(w1<w2)
          return -1;

        for(short i=weighted_block_size-1;i>=0;i--)
        {
          Integer comp1=v1[i];
          Integer comp2=v2[i];

          if(comp1>0 || comp2>0)
          {
            if(comp1<comp2)
              return 1;
            if(comp1>comp2)
              return -1;
          }
        }
  }


  /// When reaching this line, the heads of the binomials are equal.
  /// Now we decide by the tails.
  /// This part of the code could also be omitted in the current context:
  /// The compare(...)-function is only called when dealing with ordered
  /// lists. This is done in two cases:
  /// - When computing with ordered S-pair lists, it doesn't really matter
  ///   if such similar binomials are in the right order.
  /// - When outputting a reduced Groebner basis, it cannot happen that two
  ///   heads are equal.

  w1=0;
  w2=0;

  /// First check the elimination variables.

  if(elimination_block_size>0)
    switch(elimination_ordering)
    {
        case LEX:

          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer comp1=-v1[i];
            Integer comp2=-v2[i];

            if(comp1>0 || comp2>0)
            {
              if(comp1>comp2)
                return 1;
              if(comp1<comp2)
                return -1;
            }
          }

        break;

        case DEG_LEX:

          /// compute the degree of the tails in the elimination variables
          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer comp1=-v1[i];
            Integer comp2=-v2[i];

            if(comp1>0)
              w1+=comp1;
            if(comp2>0)
              w2+=comp2;
          }

          if(w1>w2)
            return 1;
          if(w1<w2)
            return -1;

          /// if the degree is equal:
          /// tie breaking with the lexicographical ordering
          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer comp1=-v1[i];
            Integer comp2=-v2[i];

            if(comp1>0 || comp2>0)
            {
              if(comp1>comp2)
                return 1;
              if(comp1<comp2)
                return -1;
            }
          }

          break;

        case DEG_REV_LEX:

          /// compute the degree of the tails in the elimination variables
          for(short i=weighted_block_size;i<last_index;i++)
          {
            Integer comp1=-v1[i];
            Integer comp2=-v2[i];

            if(comp1>0)
              w1+=comp1;
            if(comp2>0)
              w2+=comp2;
          }

          if(w1>w2)
            return 1;
          if(w1<w2)
            return -1;
          /// if the degree is equal:
          /// tie breaking with the reverse lexicographical ordering
          for(short i=last_index-1;i>=weighted_block_size;i--)
          {
            Integer comp1=-v1[i];
            Integer comp2=-v2[i];

            if(comp1>0 || comp2>0)
            {
              if(comp1<comp2)
                return 1;
              if(comp1>comp2)
                return -1;
            }
          }
    }


  /// When reaching this line, the tails are equal in the elimination
  /// variables.
  /// Compute the weight of the tails.

  w1=0;
  for(short i=0;i<weighted_block_size;i++)
  {
    Integer actual_component=-v1[i];
    if(actual_component>0)
      w1+=actual_component*weight_vector[i];
  }

  w2=0;
  for(short i=0;i<weighted_block_size;i++)
  {
    Integer actual_component=-v2[i];
    if(actual_component>0)
      w2+=actual_component*weight_vector[i];
  }

  if(w1>w2)
    return 1;
  if(w1<w2)
    return -1;


  /// When reaching this line, the weight of the tails in the weighted
  /// variables are equal.
  /// Tie breaking with the term ordering refining the weight.

  switch(weighted_ordering)
  {
      case W_LEX:

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer comp1=-v1[i];
          Integer comp2=-v2[i];

          if(comp1>0 || comp2>0)
          {
            if(comp1>comp2)
              return 1;
            if(comp1<comp2)
              return -1;
          }
        }

        break;

      case W_REV_LEX:

        for(short i=weighted_block_size-1;i>=0;i--)
        {
          Integer comp1=-v1[i];
          Integer comp2=-v2[i];

          if(comp1>0 || comp2>0)
          {
            if(comp1<comp2)
              return 1;
            if(comp1>comp2)
              return -1;
          }
        }

        break;

      case W_DEG_LEX:

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer comp1=-v1[i];
          Integer comp2=-v2[i];

          if(comp1>0)
            w1+=comp1;
          if(comp2>0)
            w2+=comp2;
        }

        if(w1>w2)
          return 1;
        if(w1<w2)
          return -1;

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer comp1=-v1[i];
          Integer comp2=-v2[i];

          if(comp1>0 || comp2>0)
          {
            if(comp1>comp2)
              return 1;
            if(comp1<comp2)
              return -1;
          }
        }

        break;

      case W_DEG_REV_LEX:

        for(short i=0;i<weighted_block_size;i++)
        {
          Integer comp1=-v1[i];
          Integer comp2=-v2[i];

          if(comp1>0)
            w1+=comp1;
          if(comp2>0)
            w2+=comp2;
        }

        if(w1>w2)
          return 1;
        if(w1<w2)
          return -1;

        for(short i=weighted_block_size-1;i>=0;i--)
        {
          Integer comp1=-v1[i];
          Integer comp2=-v2[i];

          if(comp1>0 || comp2>0)
          {
            if(comp1<comp2)
              return 1;
            if(comp1>comp2)
              return -1;
          }
        }
  }

  return 0;

}




///////////// operators and routines needed by the IP-algorithms ////////////////////////
///////////// to manipulate the term ordering ////////////////////////////////////////////////




term_ordering& term_ordering::operator=(const term_ordering& w)
{
  if(&w==this)
    return *this;

  if(weighted_block_size>0)
    delete[] weight_vector;

  weighted_block_size=w.weighted_block_size;
  weighted_ordering=w.weighted_ordering;
  elimination_block_size=w.elimination_block_size;
  elimination_ordering=w.elimination_ordering;
  homogeneous=w.homogeneous;

  if(weighted_block_size>0)
  {
    weight_vector=new float[weighted_block_size];
    for(short i=0;i<weighted_block_size;i++)
      weight_vector[i]=w.weight_vector[i];
  }
  else
    if(weighted_block_size<0)
      cerr<<"\nWARNING: term_ordering& term_ordering::"
        "operator=(const term_ordering&):\n"
        "assignment from corrupt term ordering"<<endl;

  return(*this);
}




float term_ordering::operator[](const short& i) const
{
  if((i<0) || (i>=weighted_block_size))
  {
    cerr<<"\nWARNING: float term_ordering::operator[](const short& i):\n"
      "access to invalid weight vector component"<<endl;
    return FLT_MAX;
  }
  else
    return weight_vector[i];
}




term_ordering& term_ordering::convert_to_weighted_ordering()
{
  elimination_block_size=0;
  return(*this);
}




term_ordering& term_ordering::convert_to_elimination_ordering
(const short& number_of_elimination_variables,
 const short& _elimination_ordering)
{
  if((_elimination_ordering<1) || (_elimination_ordering>3))
    /// unknown ordering on the elimination variables, set "error flag"
    weighted_block_size=-1;
  else
    elimination_ordering=_elimination_ordering;

  if(number_of_elimination_variables<0)
    /// argument out of range, set error flag
    weighted_block_size=-1;
  else
    elimination_block_size=number_of_elimination_variables;

  if(weighted_block_size<0)
    cerr<<"\nWARNING: term_ordering& term_ordering::"
      "convert_to_elimination_ordering(const short&, const short&):\n"
      "argument out of range"<<endl;

  return(*this);
}




term_ordering& term_ordering::append_weighted_variable(const float& weight)
{
  if(weighted_block_size>=0)
  {
    float *aux=weight_vector;
    weight_vector=new float[weighted_block_size+1];

    for(short i=0;i<weighted_block_size;i++)
      weight_vector[i]=aux[i];
    weight_vector[weighted_block_size]=weight;

    if(weighted_block_size>0)
      delete[] aux;

    weighted_block_size++;
  }
  else
    cerr<<"\nWARNING: term_ordering& term_ordering::append_weighted_variable"
      "(const float&):\n"
      "called for a corrupt term ordering, term ordering not changed"
        <<endl;

  return(*this);
}




term_ordering& term_ordering::delete_last_weighted_variable()
{
  if(weighted_block_size>0)
  {
    float *aux=weight_vector;

    if(weighted_block_size>1)
      weight_vector=new float[weighted_block_size-1];

    for(short i=0;i<weighted_block_size-1;i++)
      weight_vector[i]=aux[i];
    weighted_block_size--;

    delete[] aux;
  }
  else
    cerr<<"\nWARNING: term_ordering& term_ordering::"
      "delete_last_weighted_variable():\n"
      "called for a maybe corrupt term ordering without weighted variables,\n"
      "term ordering not changed"<<endl;

  return(*this);
}




term_ordering& term_ordering::swap_weights(const short& i, const short& j)
{
  if((i<0) || (i>=weighted_block_size) || (j<0) || (j>=weighted_block_size))
  {
    cout<<"\nWARNING: term_ordering& termordering::swap_weights"
      "(const short, const short):\nindex out of range"<<endl;
    return *this;
  }

  float swap=weight_vector[j];
  weight_vector[j]=weight_vector[i];
  weight_vector[i]=swap;

  return *this;
}




//////////////////////////// output //////////////////////////////////////////////////////////////////////




void term_ordering::print_weight_vector() const
{
  if(weighted_block_size<0)
  {
    printf("\nWARNING: void term_ordering::print_weight_vector():\n"
           "cannot print corrupt term ordering\n");
    return;
  }

  printf("(");
  for(short i=0;i<weighted_block_size-1;i++)
    printf("%6.2f,",weight_vector[i]);
  printf("%6.2f)\n",weight_vector[weighted_block_size-1]);
}




void term_ordering::print() const
{
  if(weighted_block_size<0)
  {
    printf("\n\nWARNING: void term_ordering::print():\n"
           "cannot print corrupt term ordering\n");
    return;
  }

  printf("\nelimination variables:%4d\n",elimination_block_size);
  printf("weighted variables:   %4d\n",weighted_block_size);

  printf("weight vector:\n");
  print_weight_vector();

  if(elimination_block_size>0)
  {
    printf("ordering on elimination variables: ");
    switch(elimination_ordering)
    {
        case LEX:
          printf("LEX\n");
          break;
        case DEG_LEX:
          printf("DEG_LEX\n");
          break;
        case DEG_REV_LEX:
          printf("DEG_REV_LEX\n");
          break;
    }
  }

  printf("ordering refining the weight:      ");
  switch(weighted_ordering)
  {
      case W_LEX:
        printf("W_LEX\n\n");
        break;
      case W_REV_LEX:
        printf("W_REV_LEX\n\n");
        break;
      case W_DEG_LEX:
        printf("W_DEG_LEX\n\n");
        break;
      case W_DEG_REV_LEX:
        printf("W_DEG_REV_LEX\n\n");
        break;
  }

}




void term_ordering::print_weight_vector(FILE* output) const
{
  if(weighted_block_size<0)
  {
    fprintf(output,"\nWARNING: void term_ordering::print_weight_vector(FILE*)"
            ":\ncannot print corrupt term ordering\n");
    return;
  }

  fprintf(output,"(");
  for(short i=0;i<weighted_block_size-1;i++)
    fprintf(output,"%6.2f,",weight_vector[i]);
  fprintf(output,"%6.2f)\n",weight_vector[weighted_block_size-1]);
}




void term_ordering::print(FILE* output) const
{
  if(weighted_block_size<0)
  {
    fprintf(output,"\n\nWARNING: void term_ordering::print(FILE*):\n"
           "cannot print corrupt term ordering\n");

    return;
  }

  fprintf(output,"\nelimination variables:%4d\n",elimination_block_size);
  fprintf(output,"weighted variables:   %4d\n",weighted_block_size);

  fprintf(output,"weight_vector:\n");
  print_weight_vector(output);

  if(elimination_block_size>0)
  {
    fprintf(output,"ordering on elimination variables: ");
    switch(elimination_ordering)
    {
        case LEX:
          fprintf(output,"LEX\n");
          break;
        case DEG_LEX:
          fprintf(output,"DEG_LEX\n");
          break;
        case DEG_REV_LEX:
          fprintf(output,"DEG_REV_LEX\n");
          break;
    }
  }

  fprintf(output,"ordering refining the weight:      ");
  switch(weighted_ordering)
  {
      case W_LEX:
        fprintf(output,"W_LEX\n\n");
        break;
      case W_REV_LEX:
        fprintf(output,"W_REV_LEX\n\n");
        break;
      case W_DEG_LEX:
        fprintf(output,"W_DEG_LEX\n\n");
        break;
      case W_DEG_REV_LEX:
        fprintf(output,"W_DEG_REV_LEX\n\n");
        break;
  }
}




void term_ordering::print_weight_vector(ofstream& output) const
{
  if(weighted_block_size<0)
  {
    output<<"\nWARNING: void term_ordering::print_weight_vector(ofstream&):\n"
      "cannot print corrupt term ordering"<<endl;
    return;
  }

  output<<"(";
  for(short i=0;i<weighted_block_size-1;i++)
    output<<setw(6)<<setprecision(2)<<weight_vector[i]<<",";
  output<<setw(6)<<setprecision(2)<<weight_vector[weighted_block_size-1]
        <<")"<<endl<<endl;
}




void term_ordering::print(ofstream& output) const
{
  if(weighted_block_size<0)
  {
    output<<"\nWARNING: void term_ordering::print(ofstream&):\n"
      "cannot print corrupt term ordering"<<endl;
    return;
  }

  output<<"\nelimination variables:"<<setw(4)<<elimination_block_size<<endl
        <<"weighted variables:   "<<setw(4)<<weighted_block_size<<endl;

  output<<"weight_vector:"<<endl;
  print_weight_vector(output);

  if(elimination_block_size>0)
  {
    output<<"ordering on elimination variables: ";
    switch(elimination_ordering)
    {
        case LEX:
          output<<"LEX\n"<<endl;
          break;
        case DEG_LEX:
          output<<"DEG_LEX\n"<<endl;
          break;
        case DEG_REV_LEX:
          output<<"DEG_REV_LEX\n"<<endl;
          break;
    }
  }

  output<<"ordering refining the weight:      ";
  switch(weighted_ordering)
  {
      case W_LEX:
        output<<"W_LEX\n"<<endl;
        break;
      case W_REV_LEX:
        output<<"W_REV_LEX\n"<<endl;
        break;
      case W_DEG_LEX:
        output<<"W_DEG_LEX\n"<<endl;
        break;
      case W_DEG_REV_LEX:
        output<<"W_DEG_REV_LEX\n"<<endl;
        break;
  }
}

void term_ordering::format_print_weight_vector(ofstream& output) const
{
  for(short i=0;i<weighted_block_size;i++)
    output<<setw(6)<<setprecision(2)<<weight_vector[i];
  output<<endl;
}
#endif  /// TERM_ORDERING_CC
