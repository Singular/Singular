LIB "tst.lib"; tst_init();
option(prot);

ring R = 181, x, lp;

matrix w[101][21];
w =        1,  1, 1, 12, 9, 22, 8, 32, 7, 42, 6, 52, 5, 62, 4, 72, 3, 82, 2, 92, 1  ,
           2,  2, 1, 13, 9, 23, 8, 33, 7, 43, 6, 53, 5, 63, 4, 73, 3, 83, 2, 93, 1  ,
           3,  3, 1, 14, 9, 24, 8, 34, 7, 44, 6, 54, 5, 64, 4, 74, 3, 84, 2, 94, 1  ,
           4,  4, 1, 15, 9, 25, 8, 35, 7, 45, 6, 55, 5, 65, 4, 75, 3, 85, 2, 95, 1  ,
           5,  5, 1, 16, 9, 26, 8, 36, 7, 46, 6, 56, 5, 66, 4, 76, 3, 86, 2, 96, 1  ,
           6,  6, 1, 17, 9, 27, 8, 37, 7, 47, 6, 57, 5, 67, 4, 77, 3, 87, 2, 97, 1  ,
           7,  7, 1, 18, 9, 28, 8, 38, 7, 48, 6, 58, 5, 68, 4, 78, 3, 88, 2, 98, 1  ,
           8,  8, 1, 19, 9, 29, 8, 39, 7, 49, 6, 59, 5, 69, 4, 79, 3, 89, 2, 99, 1  ,
           9,  9, 1, 20, 9, 30, 8, 40, 7, 50, 6, 60, 5, 70, 4, 80, 3, 90, 2, 100, 1  ,
          10, 10, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0  ,
          11,  2, 10, 11, 1, 22, 8, 32, 7, 42, 6, 52, 5, 62, 4, 72, 3, 82, 2, 92, 1  ,
          12,  3, 10, 12, 1, 23, 8, 33, 7, 43, 6, 53, 5, 63, 4, 73, 3, 83, 2, 93, 1  ,
          13,  4, 10, 13, 1, 24, 8, 34, 7, 44, 6, 54, 5, 64, 4, 74, 3, 84, 2, 94, 1  ,
          14,  5, 10, 14, 1, 25, 8, 35, 7, 45, 6, 55, 5, 65, 4, 75, 3, 85, 2, 95, 1  ,
          15,  6, 10, 15, 1, 26, 8, 36, 7, 46, 6, 56, 5, 66, 4, 76, 3, 86, 2, 96, 1  ,
          16,  7, 10, 16, 1, 27, 8, 37, 7, 47, 6, 57, 5, 67, 4, 77, 3, 87, 2, 97, 1  ,
          17,  8, 10, 17, 1, 28, 8, 38, 7, 48, 6, 58, 5, 68, 4, 78, 3, 88, 2, 98, 1  ,
          18,  9, 10, 18, 1, 29, 8, 39, 7, 49, 6, 59, 5, 69, 4, 79, 3, 89, 2, 99, 1  ,
          19, 10, 10, 19, 1, 30, 8, 40, 7, 50, 6, 60, 5, 70, 4, 80, 3, 90, 2, 100, 1  ,
          20, 20, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0   ,
          21,  2, 10, 12, 9, 21, 1, 32, 7, 42, 6, 52, 5, 62, 4, 72, 3, 82, 2, 92, 1  ,
          22,  3, 10, 13, 9, 22, 1, 33, 7, 43, 6, 53, 5, 63, 4, 73, 3, 83, 2, 93, 1  ,
          23,  4, 10, 14, 9, 23, 1, 34, 7, 44, 6, 54, 5, 64, 4, 74, 3, 84, 2, 94, 1  ,
          24,  5, 10, 15, 9, 24, 1, 35, 7, 45, 6, 55, 5, 65, 4, 75, 3, 85, 2, 95, 1  ,
          25,  6, 10, 16, 9, 25, 1, 36, 7, 46, 6, 56, 5, 66, 4, 76, 3, 86, 2, 96, 1  ,
          26,  7, 10, 17, 9, 26, 1, 37, 7, 47, 6, 57, 5, 67, 4, 77, 3, 87, 2, 97, 1  ,
          27,  8, 10, 18, 9, 27, 1, 38, 7, 48, 6, 58, 5, 68, 4, 78, 3, 88, 2, 98, 1  ,
          28,  9, 10, 19, 9, 28, 1, 39, 7, 49, 6, 59, 5, 69, 4, 79, 3, 89, 2, 99, 1  ,
          29, 10, 10, 20, 9, 29, 1, 40, 7, 50, 6, 60, 5, 70, 4, 80, 3, 90, 2, 100, 1  ,
          30, 30, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0   ,
          31,  2, 10, 12, 9, 22, 8, 31, 1, 42, 6, 52, 5, 62, 4, 72, 3, 82, 2, 92, 1  ,
          32,  3, 10, 13, 9, 23, 8, 32, 1, 43, 6, 53, 5, 63, 4, 73, 3, 83, 2, 93, 1  ,
          33,  4, 10, 14, 9, 24, 8, 33, 1, 44, 6, 54, 5, 64, 4, 74, 3, 84, 2, 94, 1  ,
          34,  5, 10, 15, 9, 25, 8, 34, 1, 45, 6, 55, 5, 65, 4, 75, 3, 85, 2, 95, 1  ,
          35,  6, 10, 16, 9, 26, 8, 35, 1, 46, 6, 56, 5, 66, 4, 76, 3, 86, 2, 96, 1  ,
          36,  7, 10, 17, 9, 27, 8, 36, 1, 47, 6, 57, 5, 67, 4, 77, 3, 87, 2, 97, 1  ,
          37,  8, 10, 18, 9, 28, 8, 37, 1, 48, 6, 58, 5, 68, 4, 78, 3, 88, 2, 98, 1  ,
          38,  9, 10, 19, 9, 29, 8, 38, 1, 49, 6, 59, 5, 69, 4, 79, 3, 89, 2, 99, 1  ,
          39, 10, 10, 20, 9, 30, 8, 39, 1, 50, 6, 60, 5, 70, 4, 80, 3, 90, 2, 100, 1  ,
          40, 40, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0   ,
          41,  2, 10, 12, 9, 22, 8, 32, 7, 41, 1, 52, 5, 62, 4, 72, 3, 82, 2, 92, 1  ,
          42,  3, 10, 13, 9, 23, 8, 33, 7, 42, 1, 53, 5, 63, 4, 73, 3, 83, 2, 93, 1  ,
          43,  4, 10, 14, 9, 24, 8, 34, 7, 43, 1, 54, 5, 64, 4, 74, 3, 84, 2, 94, 1  ,
          44,  5, 10, 15, 9, 25, 8, 35, 7, 44, 1, 55, 5, 65, 4, 75, 3, 85, 2, 95, 1  ,
          45,  6, 10, 16, 9, 26, 8, 36, 7, 45, 1, 56, 5, 66, 4, 76, 3, 86, 2, 96, 1  ,
          46,  7, 10, 17, 9, 27, 8, 37, 7, 46, 1, 57, 5, 67, 4, 77, 3, 87, 2, 97, 1  ,
          47,  8, 10, 18, 9, 28, 8, 38, 7, 47, 1, 58, 5, 68, 4, 78, 3, 88, 2, 98, 1  ,
          48,  9, 10, 19, 9, 29, 8, 39, 7, 48, 1, 59, 5, 69, 4, 79, 3, 89, 2, 99, 1  ,
          49, 10, 10, 20, 9, 30, 8, 40, 7, 49, 1, 60, 5, 70, 4, 80, 3, 90, 2, 100, 1  ,
          50, 50, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0   ,
          51,  2, 10, 12, 9, 22, 8, 32, 7, 42, 6, 51, 1, 62, 4, 72, 3, 82, 2, 92, 1  ,
          52,  3, 10, 13, 9, 23, 8, 33, 7, 43, 6, 52, 1, 63, 4, 73, 3, 83, 2, 93, 1  ,
          53,  4, 10, 14, 9, 24, 8, 34, 7, 44, 6, 53, 1, 64, 4, 74, 3, 84, 2, 94, 1  ,
          54,  5, 10, 15, 9, 25, 8, 35, 7, 45, 6, 54, 1, 65, 4, 75, 3, 85, 2, 95, 1  ,
          55,  6, 10, 16, 9, 26, 8, 36, 7, 46, 6, 55, 1, 66, 4, 76, 3, 86, 2, 96, 1  ,
          56,  7, 10, 17, 9, 27, 8, 37, 7, 47, 6, 56, 1, 67, 4, 77, 3, 87, 2, 97, 1  ,
          57,  8, 10, 18, 9, 28, 8, 38, 7, 48, 6, 57, 1, 68, 4, 78, 3, 88, 2, 98, 1  ,
          58,  9, 10, 19, 9, 29, 8, 39, 7, 49, 6, 58, 1, 69, 4, 79, 3, 89, 2, 99, 1  ,
          59, 10, 10, 20, 9, 30, 8, 40, 7, 50, 6, 59, 1, 70, 4, 80, 3, 90, 2, 100, 1  ,
          60, 60, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0   ,
          61,  2, 10, 12, 9, 22, 8, 32, 7, 42, 6, 52, 5, 61,   1, 72, 3, 82, 2, 92, 1  ,
          62,  3, 10, 13, 9, 23, 8, 33, 7, 43, 6, 53, 5, 62,   1, 73, 3, 83, 2, 93, 1  ,
          63,  4, 10, 14, 9, 24, 8, 34, 7, 44, 6, 54, 5, 63,   1, 74, 3, 84, 2, 94, 1  ,
          64,  5, 10, 15, 9, 25, 8, 35, 7, 45, 6, 55, 5, 64,   1, 75, 3, 85, 2, 95, 1  ,
          65,  6, 10, 16, 9, 26, 8, 36, 7, 46, 6, 56, 5, 65,   1, 76, 3, 86, 2, 96, 1  ,
          66,  7, 10, 17, 9, 27, 8, 37, 7, 47, 6, 57, 5, 66,   1, 77, 3, 87, 2, 97, 1  ,
          67,  8, 10, 18, 9, 28, 8, 38, 7, 48, 6, 58, 5, 67,   1, 78, 3, 88, 2, 98, 1  ,
          68,  9, 10, 19, 9, 29, 8, 39, 7, 49, 6, 59, 5, 68,   1, 79, 3, 89, 2, 99, 1  ,
          69, 10, 10, 20, 9, 30, 8, 40, 7, 50, 6, 60, 5, 69,   1, 80, 3, 90, 2, 100, 1  ,
          70, 70, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0   ,
          71,  2, 10, 12, 9, 22, 8, 32, 7, 42, 6, 52, 5, 62, 4, 71,   1, 82, 2, 92, 1  ,
          72,  3, 10, 13, 9, 23, 8, 33, 7, 43, 6, 53, 5, 63, 4, 72,   1, 83, 2, 93, 1  ,
          73,  4, 10, 14, 9, 24, 8, 34, 7, 44, 6, 54, 5, 64, 4, 73,   1, 84, 2, 94, 1  ,
          74,  5, 10, 15, 9, 25, 8, 35, 7, 45, 6, 55, 5, 65, 4, 74,   1, 85, 2, 95, 1  ,
          75,  6, 10, 16, 9, 26, 8, 36, 7, 46, 6, 56, 5, 66, 4, 75,   1, 86, 2, 96, 1  ,
          76,  7, 10, 17, 9, 27, 8, 37, 7, 47, 6, 57, 5, 67, 4, 76,   1, 87, 2, 97, 1  ,
          77,  8, 10, 18, 9, 28, 8, 38, 7, 48, 6, 58, 5, 68, 4, 77,   1, 88, 2, 98, 1  ,
          78,  9, 10, 19, 9, 29, 8, 39, 7, 49, 6, 59, 5, 69, 4, 78,   1, 89, 2, 99, 1  ,
          79, 10, 10, 20, 9, 30, 8, 40, 7, 50, 6, 60, 5, 70, 4, 79,   1, 90, 2, 100, 1  ,
          80, 80, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0   ,
          81,  2, 10, 12, 9, 22, 8, 32, 7, 42, 6, 52, 5, 62, 4, 72, 3, 81,   1, 92, 1  ,
          82,  3, 10, 13, 9, 23, 8, 33, 7, 43, 6, 53, 5, 63, 4, 73, 3, 82,   1, 93, 1  ,
          83,  4, 10, 14, 9, 24, 8, 34, 7, 44, 6, 54, 5, 64, 4, 74, 3, 83,   1, 94, 1  ,
          84,  5, 10, 15, 9, 25, 8, 35, 7, 45, 6, 55, 5, 65, 4, 75, 3, 84,   1, 95, 1  ,
          85,  6, 10, 16, 9, 26, 8, 36, 7, 46, 6, 56, 5, 66, 4, 76, 3, 85,   1, 96, 1  ,
          86,  7, 10, 17, 9, 27, 8, 37, 7, 47, 6, 57, 5, 67, 4, 77, 3, 86,   1, 97, 1  ,
          87,  8, 10, 18, 9, 28, 8, 38, 7, 48, 6, 58, 5, 68, 4, 78, 3, 87,   1, 98, 1  ,
          88,  9, 10, 19, 9, 29, 8, 39, 7, 49, 6, 59, 5, 69, 4, 79, 3, 88,   1, 99, 1  ,
          89, 10, 10, 20, 9, 30, 8, 40, 7, 50, 6, 60, 5, 70, 4, 80, 3, 89,   1, 100, 1  ,
          90, 90, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0   ,
          91,  2, 10, 12, 9, 22, 8, 32, 7, 42, 6, 52, 5, 62, 4, 72, 3, 82, 2, 91,   1  ,
          92,  3, 10, 13, 9, 23, 8, 33, 7, 43, 6, 53, 5, 63, 4, 73, 3, 83, 2, 92,   1  ,
          93,  4, 10, 14, 9, 24, 8, 34, 7, 44, 6, 54, 5, 64, 4, 74, 3, 84, 2, 93,   1  ,
          94,  5, 10, 15, 9, 25, 8, 35, 7, 45, 6, 55, 5, 65, 4, 75, 3, 85, 2, 94,   1  ,
          95,  6, 10, 16, 9, 26, 8, 36, 7, 46, 6, 56, 5, 66, 4, 76, 3, 86, 2, 95,   1  ,
          96,  7, 10, 17, 9, 27, 8, 37, 7, 47, 6, 57, 5, 67, 4, 77, 3, 87, 2, 96,   1  ,
          97,  8, 10, 18, 9, 28, 8, 38, 7, 48, 6, 58, 5, 68, 4, 78, 3, 88, 2, 97,   1  ,
          98,  9, 10, 19, 9, 29, 8, 39, 7, 49, 6, 59, 5, 69, 4, 79, 3, 89, 2, 98,   1  ,
          99, 10, 10, 20, 9, 30, 8, 40, 7, 50, 6, 60, 5, 70, 4, 80, 3, 90, 2, 99,   1  ,
          100, 100, 1, 101, 1, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0, 1,0   ,
          101,  1, 10, 11, 9, 21, 8, 31, 7, 41, 6, 51, 5, 61, 4, 71, 3, 81, 2, 91, 1  ;

matrix m[101][101];

int i, j;

proc fudge (int n)
{
      if (n >= 1)
         {
         return(n);
         }
      else
         {
         return(n + 181);
         }
}

for (i = 1; i <= 101; i++)
   {
   for (j = 1; j <= 10; j++)
      {
      m[i, fudge(int(w[i, 2*j]))] = w[i, 2*j+1];
      }
   }

poly result, a;

tst_InitTimer();
result = det(module(m));
tst_ReportTimer("Pp");
result;

tst_InitTimer();
/*
result = det(m - x*freemodule(101));
tst_ReportTimer("Qp");

size(result);
lead(result);
subst(result, x, 2);
*/
tst_OutTimer(100000, "Qp: killed");

matrix m2[101][101];

for (i = 1; i <= 101; i++)
   {
   for (j = 1; j <= 101; j++)
      {
      m2[i, j] = m[i, j];
      }
   }
for (i = 1; i <= 101; i++)
   {
   a = m2[i, 1];
   for (j = 1; j <= 100; j++)
      {
      m2[i, j] = m2[i, j+1];
      }
   m2[i, 101] = a;
   }
for (i = 1; i <= 101; i++)
   {
   for (j = 1; j <= 101; j++)
      {
      if (m[i, j] <> 0)
         {
         m2[i, j] = m[i, j];
         }
      }
   }

tst_InitTimer();
result = det(module(m2));
tst_ReportTimer("P'p");
result;

tst_InitTimer();
/* this goes nowhere !
result = det(m2 - x*freemodule(101));
tst_ReportTimer("Q'p");

size(result);
lead(result);
subst(result, x, 2);
*/
tst_OutTimer(100000, "Qp': killed");

kill i,j, R;

tst_status(1); $
