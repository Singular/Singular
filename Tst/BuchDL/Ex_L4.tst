LIB "tst.lib";
tst_init();


//======================  Example 4.9 =============================
ring R = 0, (w,x,y,z), dp;
matrix m[2][3] = w,x,y,x,y,z;
ideal I = minor(m,2);
I;
//->   I[1]=-y2+xz
//->   I[2]=xy-wz
//->   I[3]=x2-wy
qring Q = groebner(I);
resolution F=mres(maxideal(1),0);
//->   // ** full resolution in a qring may be infinite, 
//->         setting max length to 6
print(betti(F),"betti");
//->              0     1     2     3     4     5     6
//->   ------------------------------------------------
//->       0:     1     4     9    18    36    72   144
//->   ------------------------------------------------
//->   total:     1     4     9    18    36    72   144


kill F;  //------- Alternatively:
resolution F = mres(maxideal(1),7);
print(betti(F),"betti");
//->              0     1     2     3     4     5     6     7
//->   ------------------------------------------------------
//->       0:     1     4     9    18    36    72   144   288
//->   ------------------------------------------------------
//->   total:     1     4     9    18    36    72   144   288

print(F[1],"");
//->   z,y,x,w
print(F[2]);
//->   y, 0, 0, x, 0, 0, w, 0, 0,
//->   -z,y, 0, 0, x, 0, 0, w, 0,
//->   0, -z,y, -z,0, x, 0, 0, w,
//->   0, 0, -z,0, -z,-y,-z,-y,-x

if (not(defined(Ext))){ LIB "homolog.lib"; }
module M = Ext(7,F[1],F[1]);
//->   // dimension of Ext^7:  0
//->   // vdim of Ext^7:       288


kill Q,R;
//======================  Example 4.11 (new session) =======================
ring S = 0, x(0..3), dp;
resolution kos = mres(maxideal(1),0);
print(betti(kos),"betti");
//->              0     1     2     3     4
//->   ------------------------------------
//->       0:     1     4     6     4     1
//->   ------------------------------------
//->   total:     1     4     6     4     1

matrix alpha1 = random(1000,4,1);  // randomly created intmat
matrix tphi = transpose(kos[4]);
matrix psi = kos[3];
matrix talpha1tilde = transpose(psi*alpha1);
matrix talpha0 = lift(tphi,talpha1tilde);
print(talpha0);


kill S;
//======================  Example 4.13 (new session) =======================
ring S = 32003, x(0..4), dp;
module MI=maxideal(1);
attrib(MI,"isHomog",intvec(-1));  
resolution kos = nres(MI,0);
print(betti(kos),"betti");
//->              0     1     2     3     4     5
//->   ------------------------------------------
//->      -1:     1     5    10    10     5     1
//->   ------------------------------------------
//->   total:     1     5    10    10     5     1

matrix alpha0 = random(32002,10,3);
module pres = module(alpha0)+kos[3];

attrib(pres,"isHomog",intvec(1,1,1,1,1,1,1,1,1,1));
resolution fcokernel = mres(pres,0);
print(betti(fcokernel),"betti");
//->              0     1     2     3
//->   ------------------------------
//->       1:     7    10     5     1
//->   ------------------------------
//->   total:     7    10     5     1

module dir = transpose(pres);
intvec w = -1,-1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2;
attrib(dir,"isHomog",w);
resolution fdir = mres(dir,2);
print(betti(fdir),"betti");
//->              0     1     2
//->   ------------------------
//->      -2:    10     7     -
//->      -1:     -     -     -
//->       0:     -     -     1
//->   ------------------------
//->   total:    10     7     1

if (not(defined(flatten))){ LIB "matrix.lib"; }
ideal I = groebner(flatten(fdir[2]));
resolution FI = mres(I,0);
print(betti(FI),"betti");
//->              0     1     2     3     4
//->   ------------------------------------
//->       0:     1     -     -     -     -
//->       1:     -     -     -     -     -
//->       2:     -     7    10     5     1
//->   ------------------------------------
//->   total:     1     7    10     5     1

int codimI = nvars(S)-dim(I);
codimI;
//->   2
degree(I); 
//->   4
nvars(S)-dim(groebner(minor(jacob(I),codimI) + I));
//->   5


kill S,w,codimI;
//======================  Example 4.14 (new session) =======================
if (not(defined(dsum))){ LIB "matrix.lib"; }
ring S = 32003, x(0..4), dp;
resolution kos = nres(maxideal(1),0);
betti(kos);
//->   1,5,10,10,5,1 
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
print(betti(fdir),"betti");
//->              0     1     2
//->   ------------------------
//->      -2:    20    10     -
//->      -1:     -     1     -
//->       0:     -     -     -
//->       1:     -     -     -
//->       2:     -     -     1
//->   ------------------------
//->   total:    20    11     1
ideal I = groebner(flatten(fdir[2]));
resolution FI = mres(I,0);
print(betti(FI),"betti");
//->              0     1     2     3     4
//->   ------------------------------------
//->       0:     1     -     -     -     -
//->       1:     -     -     -     -     -
//->       2:     -     -     -     -     -
//->       3:     -     1     -     -     -
//->       4:     -    10    18    10     2
//->   ------------------------------------
//->   total:     1    11    18    10     2


//------- check smoothness ----------
int codimI = nvars(S)-dim(I);
codimI;
//-> 2
nvars(S)-dim(groebner(minor(jacob(I),codimI) + I));
//-> 5

tst_status(1);$

