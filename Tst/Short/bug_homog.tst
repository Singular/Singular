LIB "tst.lib";
tst_init();

// homogonizations and variable weights:

ring r=0,(x,y,z),M(2,3,1,1,0,0,0,1,0);
poly p=x+y+z;
homog(p,z);
ring s=0,(x,y,z),wp(2,3,1);
poly p=x+y+z;
homog(p,z);

ring rr=0,(x,y,z),M(2,3,4,1,0,0,0,1,0);
poly p=x+y+z;
homog(p,z);
ring ss=0,(x,y,z),wp(2,3,4);
poly p=x+y+z;
homog(p,z);

tst_status(1);$
