LIB "tst.lib"; tst_init(); tst_ignore("// ** loaded "); tst_ignore("// ** _ is no standard basis");
LIB "deRham.lib";

//----------------------------------------
//EXAMPLE 1
//----------------------------------------
ring r=0,(x,y),dp;
poly f=y2-x3-2x+3;
list L=deRhamCohomology(f);
L;
kill r, L;

//----------------------------------------
//EXAMPLE 2
//----------------------------------------
ring r=0,(x,y),dp;
poly f=y2-x3-x;
list L=deRhamCohomology(f);
L;
kill r, L;

//----------------------------------------
//EXAMPLE 3
//----------------------------------------
ring r=0,(x,y),dp;
list C=list(x2-1,(x+1)*y,y*(y2+2x+1));
list L=deRhamCohomology(C);
L;
kill r, L;

//----------------------------------------
//EXAMPLE 4
//----------------------------------------
ring r=0,(x,y,z),dp;
list C=list(x*(x-1),y,z*(z-1),z*(x-1));
list L=deRhamCohomology(C);
L;
kill r, L;

//----------------------------------------
//EXAMPLE 5
//----------------------------------------
ring r=0,(x,y,z),dp;
list C=list(x*y,y*z);
list L=deRhamCohomology(C,"Vdres");
L;
kill r, L;

//----------------------------------------
//EXAMPLE 6
//----------------------------------------
ring r=0,(x,y,z,u),dp;
list C=list(x,y,z,u);
list L=deRhamCohomology(C);
L;
kill r, L;

//----------------------------------------
//EXAMPLE 7
//----------------------------------------
ring r=0,(x,y,z),dp;
poly f=x3+y3+z3;
list L=deRhamCohomology(f);
L;
kill r, L;

//----------------------------------------
//EXAMPLE 8
//----------------------------------------
ring r=0,(x,y,z),dp;
poly f=x2+y2+z2;
list L=deRhamCohomology(f,"Vdres");
L;
kill r, L;

//----------------------------------------
//EXAMPLE 9
//----------------------------------------
ring r=0,(x,y,z,u),dp;
list C=list(x2+y2+z2,u);
list L=deRhamCohomology(C);
L;
kill r, L;


//----------------------------------------
//EXAMPLE 10
//----------------------------------------
ring r=0,(x,y,z),dp;
list C=list((x*(y-1),y2-1));
list L=deRhamCohomology(C);
L;
kill r, L;

tst_status(1);$
