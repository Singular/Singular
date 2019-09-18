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

THREAD_INST_VAR extern weightOverflowException weightOverflow;

class exponentOverflowException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "int overflow in an exponent";
  }
};

THREAD_INST_VAR extern exponentOverflowException exponentOverflow;

#endif
