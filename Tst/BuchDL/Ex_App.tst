LIB "tst.lib";
tst_init();

//======================= Example A.2 ===========================
//=============== Example 4.13 (continued) ======================
ring S = 32003, x(0..4), dp;
module MI=maxideal(1);
attrib(MI,"isHomog",intvec(-1));
resolution kos = nres(MI,0);
matrix alpha0 = random(32002,10,3);
module pres = module(alpha0)+kos[3];
attrib(pres,"isHomog",intvec(1,1,1,1,1,1,1,1,1,1));
resolution fcokernel = mres(pres,0);
module dir = transpose(pres);
intvec w = -1,-1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2;
attrib(dir,"isHomog",w);
resolution fdir = mres(dir,2);
if (not(defined(flatten))){ LIB "matrix.lib"; }
ideal I = groebner(flatten(fdir[2]));
resolution FI = mres(I,0);

LIB "sheafcoh.lib";
module F = FI[2];
intmat B = sheafCoh(F,-2,6);
displayCohom(B,-2,6,nvars(S)-1);
//->          -2   -1    0    1    2    3    4    5    6
//->   -------------------------------------------------
//->     4:    -    -    -    -    -    -    -    -    -
//->     3:    3    -    -    -    -    -    -    -    -
//->     2:    -    -    -    -    -    -    -    -    -
//->     1:    -    -    -    1    -    -    -    -    -
//->     0:    -    -    -    -    -    7   25   60  119
//->   -------------------------------------------------
//->   chi:   -3    0    0   -1    0    7   25   60  119


kill S,B,w;
//=============== Example 4.14 (continued) ======================
if (not(defined(dsum))){ LIB "matrix.lib"; }
ring S = 32003, x(0..4), dp;
resolution kos = nres(maxideal(1),0);
matrix kos5 = kos[5];
matrix tphi = transpose(dsum(kos5,kos5));
matrix kos3 = kos[3];
matrix psi = dsum(kos3,kos3);
matrix beta1 = random(32002,20,2);
matrix tbeta1tilde = transpose(psi*beta1);
matrix tbeta0 = lift(tphi,tbeta1tilde);
if (not(defined(Ext_R))){ LIB "homolog.lib"; }
def E = Ext_R(1,kos[4]);
//->   // dimension of Ext^1:  -1
matrix kos4 = kos[4];
matrix tkos4pluskos4 = transpose(dsum(kos4,kos4));
matrix tgammamin1 = random(32002,20,1);
matrix tgamma0 = tkos4pluskos4*tgammamin1;
matrix talpha0 = concat(tbeta0,tgamma0);
matrix zero[20][1];
matrix tpsi = transpose(psi);
matrix tpresg = concat(tpsi,zero);
matrix pres = module(transpose(talpha0))
                + module(transpose(tpresg));
module dir = transpose(pres);
dir = prune(dir);
homog(dir);
//->   1
intvec deg_dir = attrib(dir,"isHomog");
attrib(dir,"isHomog",deg_dir-2);        // set degrees
resolution fdir = mres(prune(dir),2);
ideal I = groebner(flatten(fdir[2]));
resolution FI = mres(I,0);

if (not(defined(sheafCoh))){ LIB "sheafcoh.lib"; }
module F = FI[2];
intmat B = sheafCoh(F,0,8);
displayCohom(B,0,8,nvars(basering)-1);
//->           0    1    2    3    4    5    6    7    8
//->   -------------------------------------------------
//->     4:    -    -    -    -    -    -    -    -    -
//->     3:    -    -    -    -    -    -    -    -    -
//->     2:    -    2    -    -    -    -    -    -    -
//->     1:    -    -    -    2    -    -    -    -    -
//->     0:    -    -    -    -    1   15   47  105  198
//->   -------------------------------------------------
//->   chi:    0    2    0   -2    1   15   47  105  198


kill S,B,deg_dir;
//======================= Example A.5 ===========================
//=============== Example 4.13 (continued) ======================
ring S = 32003, x(0..4), dp;
module MI=maxideal(1);
attrib(MI,"isHomog",intvec(-1));
resolution kos = nres(MI,0);
matrix alpha0 = random(32002,10,3);
module pres = module(alpha0)+kos[3];
attrib(pres,"isHomog",intvec(1,1,1,1,1,1,1,1,1,1));
resolution fcokernel = mres(pres,0);
module dir = transpose(pres);
intvec w = -1,-1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2;
attrib(dir,"isHomog",w);
resolution fdir = mres(dir,2);
if (not(defined(flatten))){ LIB "matrix.lib"; }
ideal I = groebner(flatten(fdir[2]));
resolution FI = mres(I,0);

if (not(defined(sheafCohBGG))){ LIB "sheafcoh.lib"; }
module F = FI[2];
intmat B = sheafCohBGG(F,-2,6);
displayCohom(B,-2,6,nvars(basering)-1);
//->          -2   -1    0    1    2    3    4    5    6
//->    -------------------------------------------------
//->     4:    -    -    -    -    -    *    *    *    *
//->     3:    *    -    -    -    -    -    *    *    *
//->     2:    *    *    -    -    -    -    -    *    *
//->     1:    *    *    *    1    -    -    -    -    *
//->     0:    *    *    *    *    -    7   25   60  119
//->    -------------------------------------------------
//->   chi:    *    *    *    *    0    *    *    *    *

tst_status(1);$
