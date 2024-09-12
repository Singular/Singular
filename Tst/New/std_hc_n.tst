LIB "tst.lib"; tst_init();
ring R = 0,(x,y),ds;
poly f = (x2+y2)*(y2-(x-y)^3)*(x^3+y9);
ideal I = f,jacob(f);
vdim(std(I));              //43

ring r=32003,(x,y),ds;
ideal I=imap(R,I);
ideal J=std(I);
poly hc=highcorner(J);
hc;                          //xy13

setring R;
noether=y*imap(r,hc);
vdim(std (I));           //43

ring RR = 0,(x,y),Ds;
poly f = (x2+y2)*(y2-(x-y)^3)*(x^3+y9);
ideal I = f,jacob(f);
vdim(std(I));              //43

ring rr=32003,(x,y),Ds;
ideal I=imap(R,I);
ideal J=std(I);
poly hc=highcorner(J);
hc;                          //xy13

setring RR;
noether=y*imap(rr,hc);
vdim(std (I));           //43

tst_status(1);$
