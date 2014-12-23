LIB "tst.lib";
tst_init();



proc verifyLUSolveResult(A, RHS, solutionData)
{
        if (solutionData[1])
        {
                def solutionPoint = solutionData[2];
                def kernelA = solutionData[3];
                ASSUME(0, A*solutionPoint == RHS );
                ASSUME(0, rank(kernelA) + rank(A) == ncols(A) );
                ASSUME(0, A*kernelA==0 );
        }
}

// test lusolve() for corner case A = 0

ring r= 0,z,dp;

matrix A;

def LD  = ludecomp(A);
def P,L,U = LD[1],LD[2],LD[3];

ASSUME(0, (P*A)==(L*U) );
def RHS = matrix(0);


list solutionData = lusolve(P,L,U,RHS );

ASSUME(0, solutionData[1]==1 );

verifyLUSolveResult(A, RHS, solutionData);


// test lusolve() for the case that the first columns in U are zero.

matrix B[1][2] = 0,1;    
LD =  ludecomp(B);
P,L,U = LD[1],LD[2],LD[3];

RHS = matrix(10);


solutionData = lusolve( P,L,U, RHS );

ASSUME(0, solutionData[1]==1 );

verifyLUSolveResult(B, RHS, solutionData);



tst_status(1); $

