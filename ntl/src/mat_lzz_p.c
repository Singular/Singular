
#include <NTL/mat_lzz_p.h>

#include <NTL/new.h>

NTL_START_IMPL

NTL_matrix_impl(zz_p,vec_zz_p,vec_vec_zz_p,mat_zz_p)
NTL_io_matrix_impl(zz_p,vec_zz_p,vec_vec_zz_p,mat_zz_p)
NTL_eq_matrix_impl(zz_p,vec_zz_p,vec_vec_zz_p,mat_zz_p)


  
void add(mat_zz_p& X, const mat_zz_p& A, const mat_zz_p& B)  
{  
   long n = A.NumRows();  
   long m = A.NumCols();  
  
   if (B.NumRows() != n || B.NumCols() != m)   
      Error("matrix add: dimension mismatch");  
  
   X.SetDims(n, m);  
  
   long i, j;  
   for (i = 1; i <= n; i++)   
      for (j = 1; j <= m; j++)  
         add(X(i,j), A(i,j), B(i,j));  
}  
  
void sub(mat_zz_p& X, const mat_zz_p& A, const mat_zz_p& B)  
{  
   long n = A.NumRows();  
   long m = A.NumCols();  
  
   if (B.NumRows() != n || B.NumCols() != m)  
      Error("matrix sub: dimension mismatch");  
  
   X.SetDims(n, m);  
  
   long i, j;  
   for (i = 1; i <= n; i++)  
      for (j = 1; j <= m; j++)  
         sub(X(i,j), A(i,j), B(i,j));  
}  
  
void mul_aux(mat_zz_p& X, const mat_zz_p& A, const mat_zz_p& B)  
{  
   long n = A.NumRows();  
   long l = A.NumCols();  
   long m = B.NumCols();  
  
   if (l != B.NumRows())  
      Error("matrix mul: dimension mismatch");  
  
   X.SetDims(n, m);  
  
   long i, j, k;  
   zz_p acc, tmp;  
  
   for (i = 1; i <= n; i++) {  
      for (j = 1; j <= m; j++) {  
         clear(acc);  
         for(k = 1; k <= l; k++) {  
            mul(tmp, A(i,k), B(k,j));  
            add(acc, acc, tmp);  
         }  
         X(i,j) = acc;  
      }  
   }  
}  
  
  
void mul(mat_zz_p& X, const mat_zz_p& A, const mat_zz_p& B)  
{  
   if (&X == &A || &X == &B) {  
      mat_zz_p tmp;  
      mul_aux(tmp, A, B);  
      X = tmp;  
   }  
   else  
      mul_aux(X, A, B);  
}  
  
  
void mul_aux(vec_zz_p& x, const mat_zz_p& A, const vec_zz_p& b)
{
   long n = A.NumRows();
   long l = A.NumCols();

   if (l != b.length())
      Error("matrix mul: dimension mismatch");

   x.SetLength(n);

   long p = zz_p::modulus();
   double pinv = zz_p::ModulusInverse();

   long i, k;
   long acc, tmp;

   const zz_p* bp = b.elts();

   for (i = 0; i < n; i++) {
      acc = 0;
      const zz_p* ap = A[i].elts();

      for (k = 0; k < l; k++) {
         tmp = MulMod(rep(ap[k]), rep(bp[k]), p, pinv);
         acc = AddMod(acc, tmp, p);
      }
 
      x[i].LoopHole() = acc;
   }
}
  
void mul(vec_zz_p& x, const mat_zz_p& A, const vec_zz_p& b)  
{  
   if (&b == &x || A.position1(x) != -1) {
      vec_zz_p tmp;
      mul_aux(tmp, A, b);
      x = tmp;
   }
   else
      mul_aux(x, A, b);

}  

static
void mul_aux(vec_zz_p& x, const vec_zz_p& a, const mat_zz_p& B)  
{  
   long n = B.NumRows();  
   long l = B.NumCols();  
  
   if (n != a.length())  
      Error("matrix mul: dimension mismatch");  
  
   x.SetLength(l);  
  
   long i, k;  
   zz_p acc, tmp;  
  
   for (i = 1; i <= l; i++) {  
      clear(acc);  
      for (k = 1; k <= n; k++) {  
         mul(tmp, a(k), B(k,i));
         add(acc, acc, tmp);  
      }  
      x(i) = acc;  
   }  
}  

void mul(vec_zz_p& x, const vec_zz_p& a, const mat_zz_p& B)
{
   if (&a == &x) {
      vec_zz_p tmp;
      mul_aux(tmp, a, B);
      x = tmp;
   }
   else
      mul_aux(x, a, B);
}

     
  
void ident(mat_zz_p& X, long n)  
{  
   X.SetDims(n, n);  
   long i, j;  
  
   for (i = 1; i <= n; i++)  
      for (j = 1; j <= n; j++)  
         if (i == j)  
            set(X(i, j));  
         else  
            clear(X(i, j));  
} 



void determinant(zz_p& d, const mat_zz_p& M_in)
{
   long k, n;
   long i, j;
   long pos;
   zz_p t1, t2, t3;
   zz_p *x, *y;

   mat_zz_p M;
   M = M_in;

   n = M.NumRows();

   if (M.NumCols() != n)
      Error("determinant: nonsquare matrix");

   if (n == 0) {
      set(d);
      return;
   }

   zz_p det;

   set(det);

   long p = zz_p::modulus();
   double pinv = zz_p::ModulusInverse();

   for (k = 0; k < n; k++) {
      pos = -1;
      for (i = k; i < n; i++) {
         if (!IsZero(M[i][k])) {
            pos = i;
            break;
         }
      }

      if (pos != -1) {
         if (k != pos) {
            swap(M[pos], M[k]);
            negate(det, det);
         }

         mul(det, det, M[k][k]);

         inv(t3, M[k][k]);

         for (i = k+1; i < n; i++) {
            // M[i] = M[i] - M[k]*M[i,k]*t3

            mul(t1, M[i][k], t3);
            negate(t1, t1);

            x = M[i].elts() + (k+1);
            y = M[k].elts() + (k+1);

            long T1 = rep(t1);
            double t1pinv = T1*pinv; 
            long T2;

            for (j = k+1; j < n; j++, x++, y++) {
               // *x = *x + (*y)*t1

               T2 = MulMod2(rep(*y), T1, p, t1pinv);
               x->LoopHole() = AddMod(rep(*x), T2, p); 
            }
         }
      }
      else {
         clear(d);
         return;
      }
   }

   d = det;
}




long IsIdent(const mat_zz_p& A, long n)
{
   if (A.NumRows() != n || A.NumCols() != n)
      return 0;

   long i, j;

   for (i = 1; i <= n; i++)
      for (j = 1; j <= n; j++)
         if (i != j) {
            if (!IsZero(A(i, j))) return 0;
         }
         else {
            if (!IsOne(A(i, j))) return 0;
         }

   return 1;
}
            

void transpose(mat_zz_p& X, const mat_zz_p& A)
{
   long n = A.NumRows();
   long m = A.NumCols();

   long i, j;

   if (&X == & A) {
      if (n == m)
         for (i = 1; i <= n; i++)
            for (j = i+1; j <= n; j++)
               swap(X(i, j), X(j, i));
      else {
         mat_zz_p tmp;
         tmp.SetDims(m, n);
         for (i = 1; i <= n; i++)
            for (j = 1; j <= m; j++)
               tmp(j, i) = A(i, j);
         X.kill();
         X = tmp;
      }
   }
   else {
      X.SetDims(m, n);
      for (i = 1; i <= n; i++)
         for (j = 1; j <= m; j++)
            X(j, i) = A(i, j);
   }
}
   

void solve(zz_p& d, vec_zz_p& X, 
           const mat_zz_p& A, const vec_zz_p& b)

{
   long n = A.NumRows();

   if (A.NumCols() != n)
      Error("solve: nonsquare matrix");


   if (b.length() != n)
      Error("solve: dimension mismatch");

   if (n == 0) {
      set(d);
      X.SetLength(0);
      return;
   }

   long i, j, k, pos;
   zz_p t1, t2, t3;
   zz_p *x, *y;

   mat_zz_p M;
   M.SetDims(n, n+1);
   for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) 
         M[i][j] = A[j][i];
      M[i][n] = b[i];
   }

   zz_p det;
   set(det);

   long p = zz_p::modulus();
   double pinv = zz_p::ModulusInverse();

   for (k = 0; k < n; k++) {
      pos = -1;
      for (i = k; i < n; i++) {
         if (!IsZero(M[i][k])) {
            pos = i;
            break;
         }
      }

      if (pos != -1) {
         if (k != pos) {
            swap(M[pos], M[k]);
            negate(det, det);
         }

         mul(det, det, M[k][k]);

         inv(t3, M[k][k]);
         M[k][k] = t3;


         for (i = k+1; i < n; i++) {
            // M[i] = M[i] - M[k]*M[i,k]*t3

            mul(t1, M[i][k], t3);
            negate(t1, t1);

            x = M[i].elts() + (k+1);
            y = M[k].elts() + (k+1);

            long T1 = rep(t1);
            double t1pinv = T1*pinv;
            long T2;

            for (j = k+1; j <= n; j++, x++, y++) {
               // *x = *x + (*y)*t1

               T2 = MulMod2(rep(*y), T1, p, t1pinv);
               x->LoopHole() = AddMod(rep(*x), T2, p);
            }
         }
      }
      else {
         clear(d);
         return;
      }
   }

   X.SetLength(n);
   for (i = n-1; i >= 0; i--) {
      clear(t1);
      for (j = i+1; j < n; j++) {
         mul(t2, X[j], M[i][j]);
         add(t1, t1, t2);
      }
      sub(t1, M[i][n], t1);
      mul(X[i], t1, M[i][i]);
   }

   d = det;
}

void inv(zz_p& d, mat_zz_p& X, const mat_zz_p& A)
{
   long n = A.NumRows();
   if (A.NumCols() != n)
      Error("inv: nonsquare matrix");

   if (n == 0) {
      set(d);
      X.SetDims(0, 0);
      return;
   }

   long i, j, k, pos;
   zz_p t1, t2, t3;
   zz_p *x, *y;

   mat_zz_p M;
   M.SetDims(n, 2*n);
   for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
         M[i][j] = A[i][j];
         clear(M[i][n+j]);
      }
      set(M[i][n+i]);
   }

   zz_p det;
   set(det);

   long p = zz_p::modulus();
   double pinv = zz_p::ModulusInverse();

   for (k = 0; k < n; k++) {
      pos = -1;
      for (i = k; i < n; i++) {
         if (!IsZero(M[i][k])) {
            pos = i;
            break;
         }
      }

      if (pos != -1) {
         if (k != pos) {
            swap(M[pos], M[k]);
            negate(det, det);
         }

         mul(det, det, M[k][k]);

         inv(t3, M[k][k]);
         M[k][k] = t3;

         for (i = k+1; i < n; i++) {
            // M[i] = M[i] - M[k]*M[i,k]*t3

            mul(t1, M[i][k], t3);
            negate(t1, t1);

            x = M[i].elts() + (k+1);
            y = M[k].elts() + (k+1);

            long T1 = rep(t1);
            double t1pinv = T1*pinv;
            long T2;

            for (j = k+1; j < 2*n; j++, x++, y++) {
               // *x = *x + (*y)*t1

               T2 = MulMod2(rep(*y), T1, p, t1pinv);
               x->LoopHole() = AddMod(rep(*x), T2, p);
            }
         }
      }
      else {
         clear(d);
         return;
      }
   }

   X.SetDims(n, n);
   for (k = 0; k < n; k++) {
      for (i = n-1; i >= 0; i--) {
         clear(t1);
         for (j = i+1; j < n; j++) {
            mul(t2, X[j][k], M[i][j]);
            add(t1, t1, t2);
         }
         sub(t1, M[i][n+k], t1);
         mul(X[i][k], t1, M[i][i]);
      }
   }

   d = det;
}

long gauss(mat_zz_p& M, long w)
{
   long k, l;
   long i, j;
   long pos;
   zz_p t1, t2, t3;
   zz_p *x, *y;

   long n = M.NumRows();
   long m = M.NumCols();

   if (w < 0 || w > m)
      Error("gauss: bad args");

   long p = zz_p::modulus();
   double pinv = zz_p::ModulusInverse();
   long T1, T2;
   double T1pinv;

   l = 0;
   for (k = 0; k < w && l < n; k++) {

      pos = -1;
      for (i = l; i < n; i++) {
         if (!IsZero(M[i][k])) {
            pos = i;
            break;
         }
      }

      if (pos != -1) {
         swap(M[pos], M[l]);

         inv(t3, M[l][k]);
         negate(t3, t3);

         for (i = l+1; i < n; i++) {
            // M[i] = M[i] + M[l]*M[i,k]*t3

            mul(t1, M[i][k], t3);

            T1 = rep(t1);
            T1pinv = ((double) T1)*pinv;

            clear(M[i][k]);

            x = M[i].elts() + (k+1);
            y = M[l].elts() + (k+1);

            for (j = k+1; j < m; j++, x++, y++) {
               // *x = *x + (*y)*t1

               T2 = MulMod2(rep(*y), T1, p, T1pinv);
               T2 = AddMod(T2, rep(*x), p);
               (*x).LoopHole() = T2;
            }
         }

         l++;
      }
   }

   return l;
}

long gauss(mat_zz_p& M)
{
   return gauss(M, M.NumCols());
}

void image(mat_zz_p& X, const mat_zz_p& A)
{
   mat_zz_p M;
   M = A;
   long r = gauss(M);
   M.SetDims(r, M.NumCols());
   X = M;
}

void kernel(mat_zz_p& X, const mat_zz_p& A)
{
   long m = A.NumRows();
   long n = A.NumCols();

   mat_zz_p M;
   long r;

   transpose(M, A);
   r = gauss(M);

   X.SetDims(m-r, m);

   long i, j, k, s;
   zz_p t1, t2;

   vec_long D;
   D.SetLength(m);
   for (j = 0; j < m; j++) D[j] = -1;

   vec_zz_p inverses;
   inverses.SetLength(m);

   j = -1;
   for (i = 0; i < r; i++) {
      do {
         j++;
      } while (IsZero(M[i][j]));

      D[j] = i;
      inv(inverses[j], M[i][j]); 
   }

   for (k = 0; k < m-r; k++) {
      vec_zz_p& v = X[k];
      long pos = 0;
      for (j = m-1; j >= 0; j--) {
         if (D[j] == -1) {
            if (pos == k)
               set(v[j]);
            else
               clear(v[j]);
            pos++;
         }
         else {
            i = D[j];

            clear(t1);

            for (s = j+1; s < m; s++) {
               mul(t2, v[s], M[i][s]);
               add(t1, t1, t2);
            }

            mul(t1, t1, inverses[j]);
            negate(v[j], t1);
         }
      }
   }
}
   
void mul(mat_zz_p& X, const mat_zz_p& A, zz_p b)
{
   long n = A.NumRows();
   long m = A.NumCols();

   X.SetDims(n, m);

   long i, j;
   for (i = 0; i < n; i++)
      for (j = 0; j < m; j++)
         mul(X[i][j], A[i][j], b);
}

void mul(mat_zz_p& X, const mat_zz_p& A, long b_in)
{
   NTL_zz_pRegister(b);
   b = b_in;
   long n = A.NumRows();
   long m = A.NumCols();

   X.SetDims(n, m);

   long i, j;
   for (i = 0; i < n; i++)
      for (j = 0; j < m; j++)
         mul(X[i][j], A[i][j], b);
}



void diag(mat_zz_p& X, long n, zz_p d)  
{  
   X.SetDims(n, n);  
   long i, j;  
  
   for (i = 1; i <= n; i++)  
      for (j = 1; j <= n; j++)  
         if (i == j)  
            X(i, j) = d;  
         else  
            clear(X(i, j));  
} 

long IsDiag(const mat_zz_p& A, long n, zz_p d)
{
   if (A.NumRows() != n || A.NumCols() != n)
      return 0;

   long i, j;

   for (i = 1; i <= n; i++)
      for (j = 1; j <= n; j++)
         if (i != j) {
            if (!IsZero(A(i, j))) return 0;
         }
         else {
            if (A(i, j) != d) return 0;
         }

   return 1;
}

void negate(mat_zz_p& X, const mat_zz_p& A)
{
   long n = A.NumRows();
   long m = A.NumCols();


   X.SetDims(n, m);

   long i, j;
   for (i = 1; i <= n; i++)
      for (j = 1; j <= m; j++)
         negate(X(i,j), A(i,j));
}

long IsZero(const mat_zz_p& a)
{
   long n = a.NumRows();
   long i;

   for (i = 0; i < n; i++)
      if (!IsZero(a[i]))
         return 0;

   return 1;
}

void clear(mat_zz_p& x)
{
   long n = x.NumRows();
   long i;
   for (i = 0; i < n; i++)
      clear(x[i]);
}


mat_zz_p operator+(const mat_zz_p& a, const mat_zz_p& b)
{
   mat_zz_p res;
   add(res, a, b);
   NTL_OPT_RETURN(mat_zz_p, res);
}

mat_zz_p operator*(const mat_zz_p& a, const mat_zz_p& b)
{
   mat_zz_p res;
   mul_aux(res, a, b);
   NTL_OPT_RETURN(mat_zz_p, res);
}

mat_zz_p operator-(const mat_zz_p& a, const mat_zz_p& b)
{
   mat_zz_p res;
   sub(res, a, b);
   NTL_OPT_RETURN(mat_zz_p, res);
}


mat_zz_p operator-(const mat_zz_p& a)
{
   mat_zz_p res;
   negate(res, a);
   NTL_OPT_RETURN(mat_zz_p, res);
}


vec_zz_p operator*(const mat_zz_p& a, const vec_zz_p& b)
{
   vec_zz_p res;
   mul_aux(res, a, b);
   NTL_OPT_RETURN(vec_zz_p, res);
}

vec_zz_p operator*(const vec_zz_p& a, const mat_zz_p& b)
{
   vec_zz_p res;
   mul_aux(res, a, b);
   NTL_OPT_RETURN(vec_zz_p, res);
}

void inv(mat_zz_p& X, const mat_zz_p& A)
{
   zz_p d;
   inv(d, X, A);
   if (d == 0) Error("inv: non-invertible matrix");
}

void power(mat_zz_p& X, const mat_zz_p& A, const ZZ& e)
{
   if (A.NumRows() != A.NumCols()) Error("power: non-square matrix");

   if (e == 0) {
      ident(X, A.NumRows());
      return;
   }

   mat_zz_p T1, T2;
   long i, k;

   k = NumBits(e);
   T1 = A;

   for (i = k-2; i >= 0; i--) {
      sqr(T2, T1);
      if (bit(e, i))
         mul(T1, T2, A);
      else
         T1 = T2;
   }

   if (e < 0)
      inv(X, T1);
   else
      X = T1;
}

NTL_END_IMPL
