/******************************************************************
 *
 * File:    MPT_PutPoly.c
 * Purpose: Routines which support Put side of polynomial communications
 * Author:  Olaf Bachman (obachman@mathematik.uni-kl.de)
 * Created: 6/97
 *
 ******************************************************************/
#include "MPT.h"

MP_Status_t MPT_PutDDPoly(MP_Link_pt link,
                          MPT_ExternalPoly_t poly,
                          MPT_PutPolyFncs_pt polyfncs)
{
  MP_Uint32_t nummonomials = polyfncs->GetNumberOfMonomials(poly);
  
  failr(MP_PutCommonOperatorPacket(link,
                                   MP_PolyDict,
                                   MP_CopPolyDenseDistPoly,
                                   4,
                                   nummonomials));
  
  failr(MPT_PutPolyAnnots(link, poly, polyfncs));

  return MPT_PutPolyData(link, poly, nummonomials, polyfncs);
}

MP_Status_t MPT_PutPolyAnnots(MP_Link_pt link,
                              MPT_ExternalPoly_t poly,
                              MPT_PutPolyFncs_pt polyfncs)
{
  MP_Sint32_t characteristic = polyfncs->GetCharacteristic(poly);
  MP_Uint32_t varnum = polyfncs->GetNumberOfVariables(poly);
  MP_Uint32_t i;

  /************************************/
  /* Put the prototype annot first    */
  /************************************/

  /* each polynomial consists of monomials */
  failr(MP_PutAnnotationPacket(link,
                               MP_ProtoDict,
                               MP_AnnotProtoPrototype,
                               MP_AnnotReqValTree));

  /* Each monomial is a struct of two members */
  failr(MP_PutCommonOperatorPacket(link,
                                    MP_ProtoDict,
                                    MP_CopProtoStruct,
                                   0,
                                    2));
  /* First part is the coeff */
  if (characteristic > 0)
  {
    /* prime characterisitc */
    failr(MP_PutCommonMetaTypePacket(link,
                                      MP_ProtoDict,
                                      MP_CmtProtoIMP_Uint32,
                                      1));
    failr(MP_PutAnnotationPacket(link,
                                 MP_NumberDict,
                                 MP_AnnotNumberModulos,
                                 MP_AnnotValuated));
    /* with Modulo */
    failr(MP_PutUint32Packet(link, characteristic, 1));
    /* which is (in our case) always a prime number */
    failr(MP_PutAnnotationPacket(link,
                                 MP_NumberDict,
                                 MP_AnnotNumberIsPrime,
                                 0));
  }
  else
  {
    /* characteristic 0 */
    failr(MP_PutCommonMetaTypePacket(link,
                                        MP_NumberDict,
                                        MP_CmtNumberRational,
                                        1));
    /* are always normalized */
    failr(MP_PutAnnotationPacket(link,
                                 MP_NumberDict,
                                 MP_AnnotNumberIsNormalized,
                                 0));
  }
  /* Second part is the exponent vector */
  failr(MP_PutCommonMetaOperatorPacket(link,
                                       MP_ProtoDict,
                                       MP_CopProtoArray,
                                       1,
                                       varnum));
   failr(MP_PutAnnotationPacket(link,
                                  MP_ProtoDict,
                                  MP_AnnotProtoPrototype,
                                  MP_AnnotReqValNode));
  failr(MP_PutCommonMetaTypePacket(link,
                                   MP_ProtoDict,
                                   MP_CmtProtoIMP_Sint32,
                                   0));


  /************************************/
  /* Put the varnames annot           */
  /************************************/
  failr(MP_PutAnnotationPacket(link,
                                  MP_PolyDict,
                                  MP_AnnotPolyVarNames,
                                  MP_AnnotValuated));
  /* now, the varnames follow */
  failr(MP_PutCommonOperatorPacket(link,
                                   MP_ProtoDict,
                                   MP_CopProtoArray,
                                   1,
                                   varnum));
  failr(MP_PutAnnotationPacket(link,
                               MP_ProtoDict,
                               MP_AnnotProtoPrototype,
                               MP_AnnotReqValNode));
                                 
  failr(MP_PutCommonMetaTypePacket(link,
                                   MP_ProtoDict,
                                   MP_CmtProtoIMP_Identifier,
                                   0));
  for (i=0; i<varnum; i++)
    failr(IMP_PutString(link, polyfncs->GetVarName(poly, i)));


  /************************************/
  /* Put the varnumber annot          */
  /************************************/
  failr(MP_PutAnnotationPacket(link,
                               MP_PolyDict,
                               MP_AnnotPolyVarNumber,
                               MP_AnnotValuated));
  failr(MP_PutUint32Packet(link, varnum, 0));

  /************************************/
  /* Put the Ordering annot           */
  /************************************/
  failr(MP_PutAnnotationPacket(link,
                                  MP_PolyDict,
                                  MP_AnnotPolyOrdering,
                                  MP_AnnotValuated));
  
  failr(MP_PutCommonConstantPacket(link,
                                   MP_PolyDict,
                                   polyfncs->GetMonomialOrdering(poly),
                                   0));

  return MP_Success;
}


MP_Status_t MPT_PutPolyData(MP_Link_pt link,
                            MPT_ExternalPoly_t poly,
                            MP_Uint32_t nummonomials,
                            MPT_PutPolyFncs_pt polyfncs)
{
  MP_Uint32_t varnum = polyfncs->GetNumberOfVariables(poly), i;
  MP_Sint32_t
    *exp_vector = (MP_Sint32_t *) IMP_MemAllocFnc(varnum*sizeof(MP_Sint32_t));
  
  for (i=0; i<nummonomials; i++)
  {
    polyfncs->PutCoeffFillExpVector(link, poly, i, varnum, exp_vector);
    failr(IMP_PutSint32Vector(link, exp_vector, varnum));
  }

  IMP_MemFreeFnc(exp_vector, varnum*sizeof(MP_Sint32_t));
  return MP_Success;
}

MP_Status_t MPT_PutDDPIdeal(MP_Link_pt link,
                            MPT_ExternalIdeal_t ideal,
                            MPT_PutIdealFncs_pt idealfncs)
{
  MP_Uint32_t numpolys = idealfncs->GetNumberOfPolys(ideal), i, nummonoms;
  MPT_ExternalPoly_t poly;

  failr(MP_PutCommonOperatorPacket(link,
                                   MP_PolyDict,
                                   MP_CopPolyIdeal,
                                   1,
                                   numpolys));
  failr(MP_PutAnnotationPacket(link,
                               MP_ProtoDict,
                               MP_AnnotProtoPrototype,
                               MP_AnnotReqValTree));
  failr(MP_PutCommonMetaOperatorPacket(link,
                                       MP_PolyDict,
                                       MP_CopPolyDenseDistPoly,
                                       4,
                                       0));
  
  failr(MPT_PutPolyAnnots(link,
                          idealfncs->GetPoly(ideal, 0),
                          idealfncs->polyfncs));

  for (i=0; i<numpolys; i++)
  {
    poly = idealfncs->GetPoly(ideal, i);
    nummonoms = idealfncs->polyfncs->GetNumberOfMonomials(poly);
    failr(IMP_PutUint32(link, nummonoms));
    failr(MPT_PutPolyData(link,
                          poly,
                          nummonoms,
                          idealfncs->polyfncs));
  }

  return MP_Success;
}

MP_Status_t MPT_PutDense2DMatrix(MP_Link_pt link,
                                 MPT_External2DMatrix_t matrix,
                                 MPT_Put2DMatrixFncs_pt matrixfncs)
{
  MP_Uint32_t m, n, mn, i, j;

  matrixfncs->GetDimension(matrix, &m, &n);

  mn = m*n;

  failr(MP_PutCommonOperatorPacket(link,
                                   MP_MatrixDict,
                                   MP_CopMatrixDenseMatrix,
                                   (matrixfncs->PutElType != NULL ? 1 : 2),
                                   mn));

  /* Put dimension annot */
  failr(MP_PutAnnotationPacket(link,
                                  MP_MatrixDict,
                                  MP_AnnotMatrixDimension,
                                  MP_AnnotReqValNode));
  /* which specifies the dimesnion of the matrix */
  failr(MP_PutCommonOperatorPacket(link,
                                      MP_BasicDict,
                                      MP_CopBasicList,
                                      0, 2));
  failr(MP_PutUint32Packet(link, (MP_Uint32_t) m, 0));
  failr(MP_PutUint32Packet(link, (MP_Uint32_t) n, 0));

  /* Now put the prototype annotation, if necessary */
  if (matrixfncs->PutElType != NULL)
  {
    failr(MP_PutAnnotationPacket(link,
                                 MP_ProtoDict,
                                 MP_AnnotProtoPrototype,
                                 MP_AnnotReqValNode));
    failr(matrixfncs->PutElType(link, matrix));
  }

  /* Now we are ready to Put the elements */
  for (i=0; i<m; i++)
    for (j=0; j<m; j++)
      failr(matrixfncs->PutEl(link, matrix, i, j));

  return MP_Success;
}

MP_Status_t MPT_Put2DPolyMatrix(MP_Link_pt link,
                                MPT_External2DPolyMatrix_t matrix,
                                MPT_Put2DPolyMatrixFncs_pt mfncs)
{
  MP_Uint32_t m,n, i, j, nummonoms;
  MPT_ExternalPoly_t poly;

  mfncs->GetDimension(matrix, &m, &n);
  failr(MP_PutCommonOperatorPacket(link,
                                   MP_MatrixDict,
                                   MP_CopMatrixDenseMatrix,
                                   2,
                                   m*n));

  /* Put the prototype annot */
  failr(MP_PutAnnotationPacket(link,
                               MP_ProtoDict,
                               MP_AnnotProtoPrototype,
                               MP_AnnotReqValTree));
  failr(MP_PutCommonMetaOperatorPacket(link,
                                       MP_PolyDict,
                                       MP_CopPolyDenseDistPoly,
                                       4,
                                       0));
  
  failr(MPT_PutPolyAnnots(link,
                          mfncs->GetPoly(matrix, 0, 0),
                          mfncs->polyfncs));
  
  
  /* Put dimension annot */
  failr(MP_PutAnnotationPacket(link,
                                  MP_MatrixDict,
                                  MP_AnnotMatrixDimension,
                                  MP_AnnotReqValNode));
  /* which specifies the dimesnion of the matrix */
  failr(MP_PutCommonOperatorPacket(link,
                                      MP_BasicDict,
                                      MP_CopBasicList,
                                      0, 2));
  failr(MP_PutUint32Packet(link, (MP_Uint32_t) m, 0));
  failr(MP_PutUint32Packet(link, (MP_Uint32_t) n, 0));

  for (i=0; i<m; i++)
  {
    for (j=0; j<n; j++)
    {
      poly = mfncs->GetPoly(matrix, i, j);
      nummonoms = mfncs->polyfncs->GetNumberOfMonomials(poly);
      failr(IMP_PutUint32(link, nummonoms));
      failr(MPT_PutPolyData(link,
                            poly,
                            nummonoms,
                            mfncs->polyfncs));
    }
  }

  return MP_Success;
}

  
  
