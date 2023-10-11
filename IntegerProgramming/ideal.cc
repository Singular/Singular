// ideal.cc

// implementation of some general ideal functions

#ifndef IDEAL_CC
#define IDEAL_CC

#include <climits>
#include "ideal.h"

/////////////////////////////////////////////////////////////////////////////
////////////////// private member functions /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

///////////// subset_tree data structure ////////////////////////////////////

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

void ideal::create_subset_tree()
{
  for(int i=0;i<Number_of_Lists;i++)
  {

// First determine the number of binary vectors whose support is a subset
// of the support of i (i read as binary vector).
// The support of i is a set of cardinality s, where s is the number of
// bits in i that are 1. Hence the desired number is 2^s.

    int s=0;

    for(int k=0;k<List_Support_Variables;k++)
      if( (i&(1<<k)) == (1<<k) )
        // bit k of i is 1
        s++;

    S.number_of_subsets[i]=(1<<s);
    // (1<<s) == 2^s

// Now determine the concrete binary vectors whose support is a subset
// of that of i. This is done in a very simple manner by comparing
// the support of each number between 0 and i (read as binary vector)
// with that of i. (Efficiency considerations are absolutely unimportant
// in this function.)

    S.subsets_of_support[i]=new int[S.number_of_subsets[i]];
    // memory allocation for subsets_of_support[i]

    int index=0;
    for(int j=0;j<Number_of_Lists;j++)
      if((i&j)==j)
        // If the support of j as a bit vector is contained in the support of
        // i as a bit vector, j is saved in the list subsets_of_support[i].
          {
            S.subsets_of_support[i][index]=j;
            index++;
          }
  }
}

void ideal::destroy_subset_tree()
{
  for(int i=0;i<Number_of_Lists;i++)
    delete[] S.subsets_of_support[i];
  // The arrays number_of_subsets and subsets_of_support (the (int*)-array)
  // are not dynamically allocated and do not have to be deleted.
}
#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

/////////// subroutines for Buchberger´s algorithm //////////////////////////

ideal& ideal::add_new_generator(binomial& bin)
{
#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  new_generators[(bin.head_support)%Number_of_Lists].insert(bin);
  // insert the bin according to its support,
  // considering only the first List_Support_Variables variables.

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  new_generators.insert(bin);

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  return(*this);
}

ideal& ideal::add_generator(binomial& bin)
{
// Beside its function as a auxiliary routine for a shorter code, this routine
// offers a good way to hide if SUPPORT_DRIVEN_METHODS_EXTENDED are used or
// not. So the constructors do not have to care about this.

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  generators[(bin.head_support)%Number_of_Lists].insert(bin);
  // insert the bin according to its support,
  // considering only the first List_Support_Variables variables.

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  generators.insert(bin);

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  size++;
  number_of_new_binomials++;

  return(*this);
}

//////////////////// constructor subroutines ////////////////////////////////

ideal& ideal::Conti_Traverso_ideal(matrix& A,const term_ordering& _w)
{

  // A may have negative entries; to model this with binomials, we need an
  // inversion variable.

  w=_w;
  // The argument term ordering should be given by the objective function.

  w.convert_to_elimination_ordering(A.rows+1,LEX);
  // extend term ordering into an elimination ordering of the appropriate
  // size

  Integer *generator=new Integer[A.columns+A.rows+1];
  // A.columns + A.rows +1 is the number of variables for the Conti-Traverso
  // algorithm with "inversion variable".

  // build initial ideal generators
  for(int j=0;j<A.columns;j++)
  {
    for(int k=0;k<A.columns;k++)
      // original variables
      if(j==k)
        generator[k]=-1;
      else
        generator[k]=0;

    for(int i=0;i<A.rows;i++)
      // elimination variables
      generator[A.columns+i]=A.coefficients[i][j];

    generator[A.columns+A.rows]=0;
    // inversion variable

    // Note that the relative order of the variables is important:
    // If the elimination variables do not follow the other variables,
    // the conversion of the term ordering has not the desired effect.

    binomial* bin=new binomial(A.rows+1+A.columns,generator,w);
    add_generator(*bin);
  }

  // now add the "inversion generator"
  for(int j=0;j<A.columns;j++)
    generator[j]=0;
  for(int i=0;i<A.rows+1;i++)
    generator[A.columns+i]=1;
  binomial* bin=new binomial(A.rows+1+A.columns,generator,w);
  add_generator(*bin);

  delete[] generator;
  return *this;
}

ideal& ideal::Positive_Conti_Traverso_ideal(matrix& A,const term_ordering& _w)
{

  // A is assumed to have only nonnegative entries;then we need no
  // "inversion variable".

  w=_w;
  // The argument term ordering should be given by the objective function.

  w.convert_to_elimination_ordering(A.rows, LEX);
  // extend term ordering into an elimination ordering of the appropriate
  // size

  Integer *generator=new Integer[A.columns+A.rows];
  // A.columns + A.rows is the number of variables for the Conti-Traverso
  // algorithm without "inversion variable".

  // build the initial ideal generators
  for(int j=0;j<A.columns;j++)
  {
    for(int k=0;k<A.columns;k++)
      // original variables
      if(j==k)
        generator[k]=-1;
      else
        generator[k]=0;

    for(int i=0;i<A.rows;i++)
      // elimination variables
      generator[A.columns+i]=A.coefficients[i][j];

    // Note that the relative order of the variables is important:
    // If the elimination variables do not follow the other variables,
    // the conversion of the term ordering has not the desired effect.

    binomial* bin=new binomial(A.rows+A.columns,generator,w);
    add_generator(*bin);
  }
  delete[] generator;
  return *this;
}

ideal& ideal::Pottier_ideal(matrix& A, const term_ordering& _w)
{

  w=_w;
  // The argument term_ordering should be given by the objective function.

  w.convert_to_elimination_ordering(1,LEX);
  // add one elimination variable used to saturate the ideal

  if(A._kernel_dimension==-2)
    // kernel of A not yet computed, do this now
    A.LLL_kernel_basis();

  if((A._kernel_dimension==-1) &&  (A.columns<0))
    // error occurred in kernel computation or matrix corrupt
  {
    cout<<"\nWARNING: ideal& ideal::Pottier_ideal(matrix&, const "
      "term_ordering&):\ncannot build ideal from a corrupt input matrix"<<endl;
    size=-1;
    return *this;
  }

  Integer *generator=new Integer[A.columns+1];
  // This is the number of variables needed for Pottier's algorithm.


  // compute initial generating system from the kernel of A
  for(int j=0;j<A._kernel_dimension;j++)
  {

    for(int k=0;k<A.columns;k++)
    {

      // We should first verify if the components of the LLL-reduced lattice
      // basis fit into the basic data type (Integer as defined in globals.h).
      // This overflow control does of course not detect overflows in the
      // course of the LLL-algorithm!

#ifdef _SHORT_

      if(((A.H)[j][k]>(const BigInt &)SHRT_MAX) || ((A.H)[j][k]<(const BigInt &)SHRT_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::Pottier_ideal(matrix&, const "
          "term_ordering&):\n"
          "LLL-reduced kernel basis does not fit into the used "
          "basic data type int."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif // _SHORT_

#ifdef _INT_

      if(((A.H)[j][k]>(const BigInt&)INT_MAX) || ((A.H)[j][k]<(const BigInt&)INT_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::Pottier_ideal(matrix&, const "
          "term_ordering&):\n"
          "LLL-reduced kernel basis does not fit into the used "
          "basic data type int."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif  // _INT_

#ifdef _LONG_

      if(((A.H)[j][k]>(const BigInt&)LONG_MAX) || ((A.H)[j][k]<(const BigInt&)LONG_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::Pottier_ideal(matrix&, const "
          "term_ordering&):\n"
          "LLL-reduced kernel basis does not fit into the used "
          "basic data type long."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif  // _LONG_

      generator[k]=(A.H)[j][k];
    }

    generator[A.columns]=0;
    // elimination variable

    // Note that the relative order of the variables is important:
    // If the elimination variable does not follow the other variables,
    // the conversion of the term ordering has not the desired effect.

    binomial* bin=new binomial(A.columns+1,generator,w);
    add_generator(*bin);
  }

  // build "saturation generator"


  // The use of the hosten_shapiro procedure is useful here because the head
  // of the computed saturation generator is smaller if less variables are
  // involved.
  int* sat_var = NULL;
  int number_of_sat_var = A.hosten_shapiro(sat_var);
  if( (number_of_sat_var == 0) || (sat_var == NULL) )
  {
    delete[] generator;
    return *this;
  }

  for(int j=0;j<A.columns;j++)
    generator[j]=0;

  for(int k=0;k<number_of_sat_var;k++)
    generator[sat_var[k]]=1;

  generator[A.columns]=1;

  binomial* bin=new binomial(A.columns+1,generator,w);
  add_generator(*bin);
  // The "saturation generator" seems to be a monomial, but is interpreted
  // as a binomial with tail 1 by the designed data structures.

  delete[] sat_var;
  delete[] generator;

  return *this;
}

ideal& ideal::Hosten_Sturmfels_ideal(matrix& A, const term_ordering& _w)
{

  // check term ordering
  if((_w.weight_refinement()!=W_REV_LEX) && (_w.is_positive()==FALSE))
    cerr<<"\nWARNING: ideal& ideal::Hosten_Sturmfels_ideal(matrix&, const "
      "term_ordering&):\nargument term ordering should be a weighted reverse"
      "lexicographical \nwith positive weights"<<endl;

  w=_w;
  // The argument term_ordering should be given by a homogeneous grading.

  if(A._kernel_dimension==-2)
    // kernel of A not yet computed, do this now
    A.LLL_kernel_basis();

  if((A._kernel_dimension==-1) &&  (A.columns<0))
    // error occurred in kernel computation or matrix corrupt
  {
    cout<<"\nWARNING: ideal& ideal::Hosten_Sturmfels_ideal(matrix&, const "
      "term_ordering&):\ncannot build ideal from a corrupt input matrix"<<endl;
    size=-1;
    return *this;
  }

  Integer * generator=new Integer[A.columns];
  // The algorithm of Hosten and Sturmfels does not need supplementary
  // variables.


  // compute initial generating system from the kernel of A
  for(int j=0;j<A._kernel_dimension;j++)
  {

    for(int k=0;k<A.columns;k++)
    {

      // We should first verify if the components of the LLL-reduced lattice
      // basis fit into the basic data type (Integer as defined in globals.h).
      // This overflow control does of course not detect overflows in the
      // course of the LLL-algorithm!

#ifdef _SHORT_

      if(((A.H)[j][k]>(const BigInt &)SHRT_MAX) || ((A.H)[j][k]<(const BigInt &)SHRT_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::Hosten_Sturmfels_ideal(matrix&, const "
          "term_ordering&):\nLLL-reduced kernel basis does not fit "
          "into the used basic data type int."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif // _SHORT_

#ifdef _INT_

      if(((A.H)[j][k]>(const BigInt&)INT_MAX) || ((A.H)[j][k]<(const BigInt&)INT_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::Hosten_Sturmfels_ideal(matrix&, const "
          "term_ordering&):\nLLL-reduced kernel basis does not fit "
          "into the used basic data type int."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif  // _INT_

#ifdef _LONG_

      if(((A.H)[j][k]>(const BigInt&)LONG_MAX) || ((A.H)[j][k]<(const BigInt&)LONG_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::Hosten_Sturmfels_ideal(matrix&, const "
          "term_ordering&):\nLLL-reduced kernel basis does not fit "
          "into the used basic data type long."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif  // _LONG_

      generator[k]=(A.H)[j][k];
    }

    // verify term ordering
    if(w.weight(generator)!=0)
      cerr<<"\nWARNING: ideal& ideal::Hosten_Sturmfels_ideal(matrix&, "
        "const term_ordering&):\nInvalid row space vector does not induce "
        "homogeneous grading."<<endl;

    binomial* bin=new binomial(A.columns,generator,w);
    add_generator(*bin);
  }

  delete[] generator;
  return *this;
}

ideal& ideal::DiBiase_Urbanke_ideal(matrix& A, const term_ordering& _w)
{
  w=_w;

  if(A._kernel_dimension==-2)
    // kernel of A not yet computed, do this now
    A.LLL_kernel_basis();

  if((A._kernel_dimension==-1) &&  (A.columns<0))
    // error occurred in kernel computation or matrix corrupt
  {
    cout<<"\nWARNING: ideal& ideal::DiBiase_Urbanke_ideal(matrix&, const "
      "term_ordering&):\ncannot build ideal from a corrupt input matrix"<<endl;
    size=-1;
    return *this;
  }

  // now compute flip variables

  int* F;
  // set of flip variables
  // If F[i]==j, x_j will be flipped.

  int r=A.compute_flip_variables(F);
  // number of flip variables

  if(r<0)
  {
    cout<<"Kernel of the input matrix contains no vector with nonzero "
      "components.\nPlease use another algorithm."<<endl;
    size=-1;
    return *this;
  }

  // check term ordering (as far as possible)
  BOOLEAN ordering_okay=TRUE;

  if(_w.weight_refinement()!=W_LEX)
    ordering_okay=FALSE;

  if(r>0)
  {
    for(int i=0;i<_w.number_of_weighted_variables();i++)
      if((_w[i]!=0) && (i!=F[0]))
        ordering_okay=FALSE;
  }

  if(ordering_okay==FALSE)
    cerr<<"\nWARNING: ideal& ideal::DiBiase_Urbanke_ideal(matrix&, const "
      "term_ordering&):\nargument term ordering might be inappropriate"<<endl;

  Integer *generator=new Integer[A.columns];
  // The algorithm of DiBiase and Urbanke does not need supplementary
  // variables.

  // compute initial generating system from the kernel of A
  for(int j=0;j<A._kernel_dimension;j++)
  {

    for(int k=0;k<A.columns;k++)
    {

      // We should first verify if the components of the LLL-reduced lattice
      // basis fit into the basic data type (Integer as defined in globals.h).
      // This overflow control does of course not detect overflows in the
      // course of the LLL-algorithm!

#ifdef _SHORT_

      if(((A.H)[j][k]>(const BigInt &)SHRT_MAX) || ((A.H)[j][k]<(const BigInt &)SHRT_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::DiBiase_Urbanke_ideal(matrix&, const "
          "term_ordering&):\nLLL-reduced kernel basis does not fit "
          "into the used basic data type int."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif // _SHORT_

#ifdef _INT_

      if(((A.H)[j][k]>(const BigInt&)INT_MAX) || ((A.H)[j][k]<(const BigInt&)INT_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::DiBiase_Urbanke_ideal(matrix&, const "
          "term_ordering&):\nLLL-reduced kernel basis does not fit "
          "into the used basic data type int."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif  // _INT_

#ifdef _LONG_

      if(((A.H)[j][k]>(const BigInt&)LONG_MAX) || ((A.H)[j][k]<(const BigInt&)LONG_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::DiBiase_Urbanke_ideal(matrix&, const "
          "term_ordering&):\nLLL-reduced kernel basis does not fit "
          "into the used basic data type long."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif  // _LONG_
      generator[k]=(A.H)[j][k];
    }

    // flip variables
    for(int l=0;l<r;l++)
      generator[F[l]]*=-1;

    binomial* bin=new binomial(A.columns,generator,w);
    add_generator(*bin);
  }
  delete[] F;
  delete[] generator;
  return *this;
}

ideal& ideal::Bigatti_LaScala_Robbiano_ideal(matrix& A,const term_ordering& _w)
{

  // check term ordering
  if((_w.weight_refinement()!=W_REV_LEX) && (_w.is_positive()==FALSE))
    cerr<<"\nWARNING: ideal& ideal::Bigatti_LaScala_Robbiano_ideal(matrix&, "
      "const term_ordering&):\nargument term ordering should be a weighted  "
      "reverse lexicographical \nwith positive weights"<<endl;

  w=_w;
  // The argument term_ordering should be given by a homogeneous grading.

  if(A._kernel_dimension==-2)
    // kernel of A not yet computed, do this now
    A.LLL_kernel_basis();

  if((A._kernel_dimension==-1) &&  (A.columns<0))
    // error occurred in kernel computation or matrix corrupt
  {
    cout<<"\nWARNING: ideal& ideal::Bigatti_LaScala_Robbiano_ideal(matrix&, "
      "const term_ordering&):\n"
      "cannot build ideal from a corrupt input matrix"<<endl;
    size=-1;
    return *this;
  }

  // now compute saturation variables

  // The techniques for computing a small set of saturation variables are
  // useful here for the following two reasons:
  // - The head of the saturation generator involves less variables, is
  //   smaller in term ordering.
  // - The weight of the pseudo-elimination variable is smaller.
  int* sat_var;
  int number_of_sat_var=A.hosten_shapiro(sat_var);

  float weight=0;
  for(int i=0;i<number_of_sat_var;i++)
    weight+=w[sat_var[i]];

  w.append_weighted_variable(weight);
  // one supplementary variable used to saturate the ideal

  Integer *generator=new Integer[A.columns+1];
  // The algorithm of Bigatti, LaScala and Robbiano needs one supplementary
  // weighted variable.

  // first build "saturation generator"
  for(int k=0;k<A.columns;k++)
    generator[k]=0;
  for(int i=0;i<number_of_sat_var;i++)
    generator[sat_var[i]]=1;
  generator[A.columns]=-1;

  delete[] sat_var;

  binomial* bin=new binomial(A.columns+1,generator,w);
  add_generator(*bin);

  // compute initial generating system from the kernel of A
  for(int j=0;j<A._kernel_dimension;j++)
  {
    for(int k=0;k<A.columns;k++)
    {
      // We should first verify if the components of the LLL-reduced lattice
      // basis fit into the basic data type (Integer as defined in globals.h).
      // This overflow control does of course not detect overflows in the
      // course of the LLL-algorithm!

#ifdef _SHORT_

      if(((A.H)[j][k]>(const BigInt &)SHRT_MAX) || ((A.H)[j][k]<(const BigInt &)SHRT_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::Bigatti_LaScala_Robbiano_ideal"
          "(matrix&, const term_ordering&):\nLLL-reduced kernel basis does "
          "not fit into the used basic data type int."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif // _SHORT_

#ifdef _INT_

      if(((A.H)[j][k]>(const BigInt&)INT_MAX) || ((A.H)[j][k]<(const BigInt&)INT_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::Bigatti_LaScala_Robbiano_ideal"
          "(matrix&, const term_ordering&):\nLLL-reduced kernel basis does "
          "not fit into the used basic data type int."<<endl;
        size=-3;
        delete[] generator;
        return *this;
      }

#endif  // _INT_

#ifdef _LONG_

      if(((A.H)[j][k]>(const BigInt&)LONG_MAX) || ((A.H)[j][k]<(const BigInt&)LONG_MIN))
      {
        cerr<<"\nWARNING: ideal& ideal::Bigatti_LaScala_Robbiano_ideal"
          "(matrix&, const term_ordering&):\nLLL-reduced kernel basis does "
          "not fit into the used basic data type long."<<endl;
        size=-3;
        return *this;
      }

#endif  // _LONG_
      generator[k]=(A.H)[j][k];
    }
    generator[A.columns]=0;
    // saturation variable
    // Note that the relative order of the variables is important (because
    // of the reverse lexicographical refinement of the weight).

    if(w.weight(generator)!=0)
      cerr<<"\nWARNING: ideal& ideal::Bigatti_LaScala_Robbiano_ideal(matrix&, "
        "const term_ordering&):\nInvalid row space vector does not induce "
        "homogeneous grading."<<endl;

    binomial* bin=new binomial(A.columns+1,generator,w);
    add_generator(*bin);
    // insert generator
  }
  delete[] generator;
  return *this;
}

/////////////////////////////////////////////////////////////////////////////
//////////////// public member functions ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////// constructors and destructor /////////////////////////////

ideal::ideal(matrix& A, const term_ordering& _w, const int& algorithm)
{

  // check arguments as far as possible

  if(A.error_status()<0)
  {
    cerr<<"\nWARNING: ideal::ideal(matrix&, const term_ordering&, const "
      "int&):\ncannot create ideal from a corrupt input matrix"<<endl;
    size=-1;
    return;
  }

  if(_w.error_status()<0)
  {
    cerr<<"\nWARNING: ideal::ideal(matrix&, const term_ordering&, const "
      "int&):\ncannot create ideal with a corrupt input ordering"<<endl;
    size=-1;
    return;
  }

  if((_w.number_of_elimination_variables()!=0) &&
     (_w.number_of_weighted_variables()!=A.columns))
    cerr<<"\nWARNING: ideal& ideal::ideal(matrix&, const term_ordering&):\n"
      "argument term ordering might be inappropriate"<<endl;

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  create_subset_tree();

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

  size=0;

  // initialize the S-pair flags with the default value
  // (this is not really necessray, but looks nicer when outputting the
  // ideal without having computed a Groebner basis)
  rel_primeness=1;
  M_criterion=2;
  F_criterion=0;
  B_criterion=8;
  second_criterion=0;

  interreduction_percentage=12.0;

  // construct the ideal according to the algorithm
  switch(algorithm)
  {
      case CONTI_TRAVERSO:
        Conti_Traverso_ideal(A,_w);
        break;
      case POSITIVE_CONTI_TRAVERSO:
        Positive_Conti_Traverso_ideal(A,_w);
        break;
      case POTTIER:
        Pottier_ideal(A,_w);
        break;
      case HOSTEN_STURMFELS:
        Hosten_Sturmfels_ideal(A,_w);
        break;
      case DIBIASE_URBANKE:
        DiBiase_Urbanke_ideal(A,_w);
        break;
      case BIGATTI_LASCALA_ROBBIANO:
        Bigatti_LaScala_Robbiano_ideal(A,_w);
        break;
      default:
        cerr<<"\nWARNING: ideal::ideal(matrix&, const term_ordering&, const "
          "int&):\nunknown algorithm for ideal construction"<<endl;
        size=-1;
        return;
  }
  number_of_new_binomials=size;
}

ideal::ideal(const ideal& I)
{

  if(I.error_status()<0)
    cerr<<"\nWARNING: ideal::ideal(const ideal&):\n"
      "trying to create ideal from a corrupt one"<<endl;

  size=0;
  // the size is automatically incremented when copying the generators

  w=I.w;

  rel_primeness=I.rel_primeness;
  M_criterion=I.M_criterion;
  F_criterion=I.F_criterion;
  B_criterion=I.B_criterion;
  second_criterion=I.second_criterion;

  interreduction_percentage=I.interreduction_percentage;

  // copy generators
  // To be sure to get a real copy of the argument ideal, the lists
  // aux_list and new_generators are also copied.
  list_iterator iter;


#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  iter.set_to_list(I.generators);

  while(iter.is_at_end()==FALSE)
  {
    binomial* bin=new binomial(iter.get_element());
    add_generator(*bin);
    iter.next();
  }

  iter.set_to_list(I.new_generators);

  while(iter.is_at_end()==FALSE)
  {
    binomial* bin=new binomial(iter.get_element());
    add_new_generator(*bin);
    iter.next();
  }

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  create_subset_tree();

  for(int i=0;i<Number_of_Lists;i++)
  {
    iter.set_to_list(I.generators[i]);

    while(iter.is_at_end()==FALSE)
    {
      binomial* bin=new binomial(iter.get_element());
      add_generator(*bin);
      iter.next();
    }
  }

  for(int i=0;i<Number_of_Lists;i++)
  {
    iter.set_to_list(I.new_generators[i]);

    while(iter.is_at_end()==FALSE)
    {
      binomial* bin=new binomial(iter.get_element());
      add_new_generator(*bin);
      iter.next();
    }
  }

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

  iter.set_to_list(I.aux_list);

  while(iter.is_at_end()==FALSE)
  {
    binomial* bin=new binomial(iter.get_element());
    aux_list._insert(*bin);
    iter.next();
  }
  number_of_new_binomials=size;
}

ideal::ideal(ifstream& input, const term_ordering& _w, const int&
             number_of_generators)
{
  if(_w.error_status()<0)
  {
    cerr<<"\nWARNING: ideal::ideal(ifstream&, const term_ordering&, const "
      "int&):\ncannot create ideal with a corrupt input ordering"<<endl;
    size=-1;
    return;
  }

  w=_w;

  // initialize the S-pair flags with the default value
  // (this is not really necessray, but looks nicer when outputting the
  // ideal without having computed a Groebner basis)
  rel_primeness=1;
  M_criterion=2;
  F_criterion=0;
  B_criterion=8;
  second_criterion=0;

  interreduction_percentage=12.0;

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  create_subset_tree();

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

  int number_of_variables=
    w.number_of_elimination_variables()+w.number_of_weighted_variables();
  Integer* generator=new Integer[number_of_variables];

  for(long i=0;i<number_of_generators;i++)
  {
    for(int j=0;j<number_of_variables;j++)
    {
      input>>generator[j];

      if(!input)
        // input failure, set "error flag"
      {
        cerr<<"\nWARNING: ideal::ideal(ifstream&, const term_ordering&, "
          "const int&): \ninput failure when reading generator "<<i<<endl;
        size=-2;
        delete[] generator;
        return;
      }
    }
    binomial* bin=new binomial(number_of_variables,generator,w);
    add_generator(*bin);
  }
  size=number_of_generators;
  number_of_new_binomials=size;
  delete[] generator;
}

ideal::~ideal()
{

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  destroy_subset_tree();

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

  // The destructor of the lists is automatically called.
}

///////////////////// object information ////////////////////////////////////

long ideal::number_of_generators() const
{
  return size;
}

int ideal::error_status() const
{
  if(size<0)
    return -1;
  else
    return 0;
}

//////////////////////////// output /////////////////////////////////////////

void ideal::print() const
{
  printf("\nterm ordering:\n");
  w.print();

  printf("\ngenerators:\n");

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  for(int i=0;i<Number_of_Lists;i++)
    generators[i].ordered_print(w);

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

    generators.ordered_print(w);

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  printf("\nnumber of generators: %ld\n",size);
}

void ideal::print_all() const
{
  print();
  cout<<"\nCurrently used S-pair criteria:"<<endl;
  if(rel_primeness)
    cout<<"relatively prime leading terms"<<endl;
  if(M_criterion)
    cout<<"criterion M"<<endl;
  if(F_criterion)
    cout<<"criterion F"<<endl;
  if(B_criterion)
    cout<<"criterion B"<<endl;
  if(second_criterion)
    cout<<"second criterion"<<endl;
  cout<<"\nInterreduction frequency:  "<<setprecision(1)
      <<interreduction_percentage<<" %"<<endl;
}

void ideal::print(FILE *output) const
{
  fprintf(output,"\nterm ordering:\n");
  w.print(output);

  fprintf(output,"\ngenerators:\n");

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  for(int i=0;i<Number_of_Lists;i++)
    generators[i].ordered_print(output,w);

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

    generators.ordered_print(output,w);

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED

  fprintf(output,"\nnumber of generators: %ld\n",size);

  fprintf(output,"\nInterreduction frequency:  %.1f %% \n", interreduction_percentage);
}

void ideal::print_all(FILE* output) const
{
  print(output);
  fprintf(output,"\nCurrently used S-pair criteria:\n");
  if(rel_primeness)
    fprintf(output,"relatively prime leading terms\n");
  if(M_criterion)
    fprintf(output,"criterion M\n");
  if(F_criterion)
    fprintf(output,"criterion F\n");
  if(B_criterion)
    fprintf(output,"criterion B\n");
  if(second_criterion)
    fprintf(output,"second criterion\n");
}

void ideal::print(ofstream& output) const
{
  output<<"\nterm ordering:\n"<<endl;
  w.print(output);

  output<<"\ngenerators:\n"<<endl;

#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  for(int i=0;i<Number_of_Lists;i++)
    generators[i].ordered_print(output,w);

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

    generators.ordered_print(output,w);

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED

    output<<"\nnumber of generators: "<<size<<endl;
}

void ideal::print_all(ofstream& output) const
{
  print(output);
  output<<"\nCurrently used S-pair criteria:"<<endl;
  if(rel_primeness)
    output<<"relatively prime leading terms"<<endl;
  if(M_criterion)
    output<<"criterion M"<<endl;
  if(F_criterion)
    output<<"criterion F"<<endl;
  if(B_criterion)
    output<<"criterion B"<<endl;
  if(second_criterion)
    output<<"second_criterion"<<endl;
  output<<"\nInterreduction frequency:  "<<setprecision(1)
        <<interreduction_percentage<<" %"<<endl;
}

void ideal::format_print(ofstream& output) const
{
#ifdef SUPPORT_DRIVEN_METHODS_EXTENDED

  for(int i=0;i<Number_of_Lists;i++)
    generators[i].ordered_format_print(output,w);

#endif  // SUPPORT_DRIVEN_METHODS_EXTENDED

#ifdef NO_SUPPORT_DRIVEN_METHODS_EXTENDED

    generators.ordered_format_print(output,w);

#endif  // NO_SUPPORT_DRIVEN_METHODS_EXTENDED
}
#endif  // IDEAL_CC
