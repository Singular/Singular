LIB "tst.lib"; tst_init();
ring r=0,(x,y),dp;
module mo=[x^2-y^2,1,0,0],[xy+y^2,0,1,0],[y^2,0,0,1];
print(mo);

// load dynamic module - at the same time creating package Kstd
// procedures will be available in the packages Top and Kstd
LIB("partialgb.so");
listvar(package);

// set the number of components to be considered to 1
module mostd=partialStd(mo,1);        // calling procedure in Top
                    // obviously computation ignored pairs with leading
                    // term in the second entry
print(mostd);

// now consider 2 components
module mostd2=Partialgb::partialStd(mo,2); // calling procedure in partialgb
                    // this time the previously unconsidered pair was
                    // treated too
print(mostd2);

tst_status(1);$
