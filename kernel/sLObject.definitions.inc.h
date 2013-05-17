//test test...
#define IS_BBA_SHIFTDVEC_CASE

#ifdef IS_BBA_SHIFTDVEC_CASE
  #define CREATE_FUNCTION_NAME(function_name) \
      ShiftDVec::function_name
#else
  #define CREATE_FUNCTION_NAME(function_name) \
      ::function_name
#endif

KINLINE void CREATE_FUNCTION_NAME(sLObject::Normalize)()
{
  if (t_p != NULL)
  {
    pNormalize(t_p);
    if (p != NULL) pSetCoeff0(p, pGetCoeff(t_p));
  }
  else
  {
    pNormalize(p);
  }
}
