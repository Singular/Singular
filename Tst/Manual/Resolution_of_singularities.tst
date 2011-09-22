LIB "tst.lib"; tst_init();
LIB"resolve.lib";                   // load the resolution algorithm
LIB"reszeta.lib";                   // load its application algorithms

ring R=0,(x,y,z),dp;              // define the ring Q[x,y,z]
ideal I=x7+y2-z2;                 // an A6 surface singularity
list L=resolve(I);                // compute the resolution
list iD=intersectionDiv(L);       // compute intersection properties
iD;                               // show the output
// The output is a list whose first entry contains the intersection matrix
// of the exceptional divisors. The second entry is the list of genera
// of these divisors. The third and fourth entry contain the information
// how to find the corresponding divisors in the respective charts.
tst_status(1);$
