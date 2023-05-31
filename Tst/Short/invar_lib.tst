LIB "tst.lib";
tst_init();

////////////////////////////////////////////////////////////////////////////////
// Examples
// FFT (First Fundamental Theorem, not: Fast Fourier Transform)
////////////////////////////////////////////////////////////////////////////////


LIB "matrix.lib";
LIB "invar.lib";

"EXAMPLE 1:";
SL(2);                          // Take the group SL_2
matrix m=dsum(SLrep,SLrep,SLrep,SLrep);
                                // 4 copies of the standard representation
invar(m);                       // empirical evidence for FFT
setring Invar::polyring;
// The reynolds operator is computed using the Omega process.
Invar::reynolds(x(1)*x(4)); 

setring Invar::group;                  

"EXAMPLE 2:";
SL(3);                          // Take the group SL_3
matrix m=dsum(SLrep,SLrep,SLrep,SLrep);
                                // 4 copies of the standard representation
invar(m);                       // more empirical evidence for FFT

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Examples
// some arbitrary SL_2 example
////////////////////////////////////////////////////////////////////////////////


SL(2);

matrix m=dsum(sympower(SLrep,2),SLrep,SLrep);
                                // binary forms of degree 2, plus 2 copies
                                // of the standard representation
invar(m);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Examples
// Tori
////////////////////////////////////////////////////////////////////////////////

"EXAMPLE 1:";

torus(1);                       // Take the 1-dimensional torus, the
                                // multiplicative group.
list weights=-2,-3,7;           // 3-dimensial action with weights -2,-3,7
matrix m=torusrep(weights);     // compute the matrix of the representation
invar(m);                       // compute the invariant ring

"EXAMPLE 2:";

setring Invar::group;                  
weights=-4,-3,-2,1,1,2,3,4;     // a harder example
matrix m=torusrep(weights);
invar(m);

"EXAMPLE 3:";

torus(2);                       // 2-dimensional torus
intvec w1=1,2;                  // (inspired by the jumping of a knight
intvec w2=-1,2;                 // on a chess board)
intvec w3=1,-2;                 // it takes some time
intvec w4=-1,-2;
intvec w5=2,1;
intvec w6=2,-1;
intvec w7=-2,1;
intvec w8=2,-1;
list weights=w1,w2,w3,w4,w5,w6,w7,w8;
matrix m=torusrep(weights);
invar(m);
                                // The invariant x(2)^3*x(4)^5*x(5)^4 says that
                                // 3*(-1,2)+4*(2,1)+5*(-1,-2)=(0,0)
                                // All invariants define cyclic knight-paths.


/////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Examples
// Finite groups
////////////////////////////////////////////////////////////////////////////////

"EXAMPLE 1:";
finite(6);                              // The symmetric group S_3
matrix id=unitmat(3);                   // identity matrix
matrix m3[3][3]=0,1,0,0,0,1,1,0,0;      // corresponds with (1 2 3)
matrix m2[3][3]=0,1,0,1,0,0,0,0,1;      // corresponds with (1 2)
list a=id,m3,m3*m3,m2,m2*m3,m2*m3*m3;   // all elements of S_3
matrix rep=finiterep(a);                // compute matrix of standard repr.
invar(rep);                             // compute the invariant ring

"EXAMPLE 2:";
finite(4);                              // Cyclic group of order 4
matrix id=unitmat(4);                   
matrix m4[4][4]=0,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0;
                                        // matrix of (1 2 3 4)
list a=id,m4,m4*m4,m4*m4*m4;            // all elements of the cyclic group
matrix rep=finiterep(a);                // compute matrix of regular repr.
invar(rep);                             // compute the invariant ring


/////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Examples
// Binary forms
////////////////////////////////////////////////////////////////////////////////

"EXAMPLE 1:";
SL(2);                          // set the current group to SL_2
invar(SLrep);                   // invariants of the standard representation
                                // (of course there are no invariants)

"EXAMPLE 2:";
setring Invar::group;           // 'invar' changed the basering into 'polyring'
                                // now we need the ring 'group' again (because
                                // SLrep is defined there)
matrix bin2=sympower(SLrep,2);  // Let m be the matrix of the second symmetric
                                // power of the standard representation
                                // (binary forms of degree 2)
invar(bin2);                    // compute the invariant(s)
                                // (only one: the discriminant)
                                // OK, let's try some more binary forms:

"EXAMPLE 3:";
setring Invar::group;
matrix bin3=sympower(SLrep,3);
invar(bin3);

"EXAMPLE 4:";
setring Invar::group;
matrix bin4=sympower(SLrep,4);
invar(bin4);

                                // for binary forms of degree 5 you'll need
                                // a day or so ...


///////////////////////////////////////
tst_status(1);$

