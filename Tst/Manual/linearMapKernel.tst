LIB "tst.lib"; tst_init();
LIB "central.lib";
ring AA = 0,(e,f,h),dp;
matrix D[3][3]=0;
D[1,2]=-h;  D[1,3]=2*e;  D[2,3]=-2*f;
def A = nc_algebra(1,D); // this algebra is U(sl_2)
setring A;
// Let us consider the linear map Ad_{e} from A_2 into A.
// Compute the PBW basis of A_2:
ideal Basis = PBW_maxDeg( 2 ); Basis;
// Compute images of basis elements under the linear map Ad_e:
ideal Image = applyAdF( Basis, e ); Image;
// Now we have a linear map given by: Basis_i --> Image_i
// Let's compute its kernel K:
// 1. compute syzygy module C:
module C = linearMapKernel( Image ); C;
// 2. compute corresponding combinations of basis vectors:
ideal K = linearCombinations(Basis, C); K;
// Let's check that Ad_e(K) is zero:
ideal Z = applyAdF( K, e ); Z;
// Now linearMapKernel will return a single integer 0:
def CC  = linearMapKernel(Z); typeof(CC); CC;
tst_status(1);$
