LIB "tst.lib"; tst_init();
timer = 1;                              // activate the timer
ring R0 = 0,(x,y),lp;
poly f = x5+y11+xy9+x3y9;
ideal i = jacob(f);
ideal i1 = i,i[1]*i[2];                 // undeformed ideal
ideal i2 = i,i[1]*i[2]+1/1000000*x5y8;  // deformation of i1
i1; i2;
ideal j = std(i1);
j;
// Compute average coefficient length (=51) by
//   - converting j[2] to a string in order to compute the number
//   of characters
//   - divide this by the number of monomials:
size(string(j[2])) div size(j[2]);
vdim(j);
// For a better representation normalize the long coefficients
// of the polynomial j[2] and map it to real:
poly p=(1/12103947791971846719838321886393392913750065060875)*j[2];
ring R1=real,(x,y),lp;
short=0; // force the long output format
poly p=imap(R0,p);
p;
// Compute a standard basis for the deformed ideal:
setring R0; // return to the original ring R0
j = std(i2);
j;
vdim(j);
tst_status(1);$
