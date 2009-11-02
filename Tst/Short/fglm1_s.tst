// $Id$

//
// fglm_s.tst - short tests for fglm and stdfglm
//              fglm: reco1, omdi, cyclic5 in char 32003 and 0
//           stdfglm: caprasse in char 32003 and 0
//


LIB "tst.lib";
tst_init();
tst_ignore("CVS: $Id$");

option(redSB);

// =================
// reco1, char 32003
ring r=32003,(x,y), dp;
ideal i= jacob(x5+y11+xy9+x3y9);
ideal is=std(i);
ring s=32003,(x,y), lp;
fglm(r,is);
kill r,s;

// =============
// reco1, char 0
ring r=0,(x,y), dp;
ideal i= jacob(x5+y11+xy9+x3y9);
ideal is=std(i);
ring s=0,(x,y), lp;
fglm(r,is);
kill r,s;

// ===============================================
// omdi, char 32003, with permutation of variables
ring r=32003,(a,b,c,u,v,w,x,y,z), dp;
ideal i=a+c+v+2x-1, ab+cu+2vw+2xy+2xz-2/3, ab2+cu2+2vw2+2xy2+2xz2-2/5, ab3+cu3+2vw3+2xy3+2xz3-2/7, ab4+cu4+2vw4+2xy4+2xz4-2/9, vw2+2xyz-1/9, vw4+2xy2z2-1/25, vw3+xyz2+xy2z-1/15, vw4+xyz3+xy3z-1/21;
ideal is=std(i);
ring s=32003,(u,v,w,x,y,z,a,b,c), lp;
fglm(r,is);
kill r,s;

// ============
// omdi, char 0
ring r=0,(a,b,c,u,v,w,x,y,z), dp;
ideal i=a+c+v+2x-1, ab+cu+2vw+2xy+2xz-2/3, ab2+cu2+2vw2+2xy2+2xz2-2/5, ab3+cu3+2vw3+2xy3+2xz3-2/7, ab4+cu4+2vw4+2xy4+2xz4-2/9, vw2+2xyz-1/9, vw4+2xy2z2-1/25, vw3+xyz2+xy2z-1/15, vw4+xyz3+xy3z-1/21;
ideal is=std(i);
ring s=0,(a,b,c,u,v,w,x,y,z), lp;
fglm(r,is);
kill r,s;

// ==========================================================
// cyclic5, char 32003, with permutation of variables, Dp->lp
ring r=32003,(a,b,c,d,e), Dp;
ideal i=a+b+c+d+e, ab+bc+cd+ae+de, abc+bcd+abe+ade+cde, abcd+abce+abde+acde+bcde, abcde-1;
ideal is=std(i);
ring s=32003,(b,d,e,c,a), lp;
fglm(r, is);
kill r,s;

// ===============
// cyclic5, char 0
ring r=0,(a,b,c,d,e), dp;
ideal i=a+b+c+d+e, ab+bc+cd+ae+de, abc+bcd+abe+ade+cde, abcd+abce+abde+acde+bcde, abcde-1;
ideal is=std(i);
ring s=0,(a,b,c,d,e), lp;
fglm(r, is);
kill r,s;

// =======================
// cyclic5, char 0, Dp->dp
ring r=0,(a,b,c,d,e), Dp;
ideal i=a+b+c+d+e, ab+bc+cd+ae+de, abc+bcd+abe+ade+cde, abcd+abce+abde+acde+bcde, abcde-1;
ideal is=std(i);
ring s=0,(a,b,c,d,e), dp;
fglm(r, is);
kill r,s;

// =======================================
// caprasse, char 32003, lp, using stdfglm
ring r=32003,(x,y,z,t), dp;
ideal i= y2z+2xyt-2x-z, -x3z+4xy2z+4x2yt+2y3t+4x2-10y2+4xz-10yt+2, 2yzt+xt2-x-2z, -xz3+4yz2t+4xzt2+2yt3+4xz+4z2-10yt-10t2+2;
stdfglm(i);
kill r;

// ===================================
// caprasse, char 0, lp, using stdfglm
ring r=0,(x,y,z,t), dp;
ideal i= y2z+2xyt-2x-z, -x3z+4xy2z+4x2yt+2y3t+4x2-10y2+4xz-10yt+2, 2yzt+xt2-x-2z, -xz3+4yz2t+4xzt2+2yt3+4xz+4z2-10yt-10t2+2;
stdfglm(i);
kill r;

tst_status(1);$
