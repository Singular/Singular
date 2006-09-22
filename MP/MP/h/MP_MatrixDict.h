/***************************************************************************
 *
 *   HEADER FILE:  MP_MatrixDict.h
 *
 *      The Matrixtype dictionary (definitions and declarations)
 *
 *
 *   Change Log:
 *
 *
 ***************************************************************************/
#ifndef _MP_MatrixDict_h
#define _MP_MatrixDict_h

/* Lets give the Matrix Dictionary the number 11 */

#define MP_MatrixDict 11

/***************************************************************************
 *
 * Annotations
 *
 ***************************************************************************/
#define MP_AnnotMatrixDimension     1


/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/

#define MP_CopMatrixBareiss             1
#define MP_CopMatrixCols                2
#define MP_CopMatrixDet                 3
#define MP_CopMatrixJacobi              4
#define MP_CopMatrixRows                6
#define MP_CopMatrixTrace               7
#define MP_CopMatrixTranspose           8
#define MP_CopMatrixKoszul              9
#define MP_CopMatrixMinor               10
#define MP_CopMatrixWedge               11

#define MP_CopMatrixDenseMatrix         12
#define MP_CopMatrixDenseVector         13

#endif /* _MP_MatrixDict_h */
