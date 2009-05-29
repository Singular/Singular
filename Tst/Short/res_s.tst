LIB "tst.lib";
tst_init();
option(prot);
//option(notRegularity);
//ring r=31991,(t,x,y,z,w),(c,ls);
//ideal i=x2+y2yz,y2+z2zw,t2+x2xy,x3+y3+t3,xyz;
//ideal ii=x3+yzw,y3+zwt,z3+wtx,w3+txy,t2+xy;
//ideal iii=x3+yzw,y3+zwt,z3+wtx,w2+txy,t2+xy;
//ideal tt=t2x2+tx2y+x2yz,t2y2+ty2z+y2zw,t2z2+tz2w+xz2w,t2w2+txw2+xyw2;
//ideal T=std(tt);
//timer = 1;
//res(T,0,t1);
//sres(T,0,t2);
//mres(T,0,t3);
//--------------------------------------------------------------
ring an=32003,(w,x,y,z),(dp,C);
ideal i=
1w2xy+1w2xz+1w2yz+1wxyz+1x2yz+1xy2z+1xyz2,
1w4x+1w4z+1w3yz+1w2xyz+1wx2yz+1x2y2z+1xy2z2,
1w6+1w5z+1w4xz+1w3xyz+1w2xy2z+1wx2y2z+1x2y2z2;
ideal j=std(i);
list jres=sres(j,0);
list jmin=minres(jres);
print(betti(jmin),"betti");
list jres1=mres(i,0);
print(betti(jres1),"betti");
list jres2=nres(i,0);
list jmin2=minres(jres2);
print(betti(jmin2),"betti");
kill an;
//--------------------------------------------------------------
ring pa2=32003,(w,t,u,x,y,z),(dp,C);
ideal i=
1w10t2u2-2w5tu2x6-2w5tu2y6+2w5tu2z6+1u2x12+2u2x6y6-2u2x6z6+1u2y12-2u2y6z6+1u2z12,
-1w5t5u-1w5tux4-1w5tuy4-1w5tuz4+1t4ux6+1t4uy6-1t4uz6+1ux10+1ux6y4+1ux6z4
+1ux4y6-1ux4z6+1uy10+1uy6z4-1uy4z6-1uz10,
1t8+2t4x4+2t4y4+2t4z4+1x8+2x4y4+2x4z4+1y8+2y4z4+1z8,
1w5t3u-1w5tux2-1w5tuy2+1w5tuz2-1t2ux6-1t2uy6+1t2uz6+1ux8+1ux6y2-1ux6z2
+1ux2y6-1ux2z6+1uy8-1uy6z2-1uy2z6+1uz8,
-1t6+1t4x2+1t4y2-1t4z2-1t2x4-1t2y4-1t2z4+1x6+1x4y2-1x4z2+1x2y4+1x2z4+1y6
-1y4z2+1y2z4-1z6,
1t4-2t2x2-2t2y2+2t2z2+1x4+2x2y2-2x2z2+1y4-2y2z2+1z4;
ideal j=std(i);
list jres=sres(j,0);
list jmin=minres(jres);
print(betti(jmin),"betti");
list jres1=mres(i,0);
print(betti(jres1),"betti");
list jres2=nres(i,0);
list jmin2=minres(jres2);
print(betti(jmin2),"betti");
kill pa2;
//--------------------------------------------------------------
ring sw5=32003,(a,b,c,d,e,h),(dp,C);
ideal i=
-1ab-1b2-2de-2ch,
-1ac-2bc-1e2-2dh,
-1c2-1ad-2bd-2eh,
-2cd-1ae-2be-1h2,
-1d2-2ce-1ah-2bh;
ideal j=std(i);
list jres=sres(j,0);
list jmin=minres(jres);
print(betti(jmin),"betti");
list jres1=mres(i,0);
print(betti(jres1),"betti");
list jres2=nres(i,0);
list jmin2=minres(jres2);
print(betti(jmin2),"betti");
kill sw5;
//--------------------------------------------------------------
ring r=32003,(t,x,y,z,w),(dp,C);
ideal i=
x2w+y2z,
y2x+z2w,
t2w+x2y,
x3+y3+t3;
ideal j=std(i);
list jres=sres(j,0);
list jmin=minres(jres);
print(betti(jmin),"betti");
list jres1=mres(i,0);
print(betti(jres1),"betti");
list jres2=nres(i,0);
list jmin2=minres(jres2);
print(betti(jmin2),"betti");
kill r;
//--------------------------------------------------------------
ring r1=32003,(a,b,x,y,z),ls;
module m1 = gen(1)+x2z3*gen(2),
x4yz3*gen(2);
resolution rr = sres(std(m1),0);
rr;
list(rr);
//--------------------------------------------------------------
example res;


tst_status(1);$

