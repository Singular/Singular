#ifndef GFANLIB_EXCEPTIONS_H
#define GFANLIB_EXCEPTIONS_H

#include <exception>

class weightOverflowException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "int overflow in a weight vector";
  }
};

EXTERN_INST_VAR weightOverflowException weightOverflow;

class exponentOverflowException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "int overflow in an exponent";
  }
};

EXTERN_INST_VAR exponentOverflowException exponentOverflow;

#endif
