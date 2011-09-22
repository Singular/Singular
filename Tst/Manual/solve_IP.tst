LIB "tst.lib"; tst_init();
LIB "intprog.lib";
// 1. call with single right-hand vector
intmat A[2][3]=1,1,0,0,1,1;
intvec b1=1,1;
intvec c=2,2,1;
intvec solution_vector=solve_IP(A,b1,c,"pct");
solution_vector;"";
// 2. call with list of right-hand vectors
intvec b2=-1,1;
list l=b1,b2;
l;
list solution_list=solve_IP(A,l,c,"ct");
solution_list;"";
// 3. call with single initial solution vector
A=2,1,-1,-1,1,2;
b1=3,4,5;
solve_IP(A,b1,c,"du");"";
// 4. call with single initial solution vector
//    and algorithm needing a positive row space vector
solution_vector=solve_IP(A,b1,c,"hs");"";
// 5. call with single initial solution vector
//     and positive row space vector
intvec prsv=1,2,1;
solution_vector=solve_IP(A,b1,c,"hs",prsv);
solution_vector;"";
// 6. call with list of initial solution vectors
//    and positive row space vector
b2=7,8,0;
l=b1,b2;
l;
solution_list=solve_IP(A,l,c,"blr",prsv);
solution_list;
tst_status(1);$
