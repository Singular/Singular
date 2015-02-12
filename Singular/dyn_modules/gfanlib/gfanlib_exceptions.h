#ifndef GFANLIB_EXCEPTIONS_H
#define GFANLIB_EXCEPTIONS_H

#include <iostream>
#include <exception>

class weightOverflowException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "int overflow in a weight vector";
  }
};

extern weightOverflowException weightOverflow;

class exponentOverflowException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "int overflow in an exponent";
  }
};

extern exponentOverflowException exponentOverflow;

#endif
