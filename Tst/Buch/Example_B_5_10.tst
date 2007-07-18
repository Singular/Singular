LIB "tst.lib";
tst_init();

// ================================= example B.5.10 ==========================
ring R=2,(z,x,y),lp;
poly f=x2+x+1;          //minimal polynomial of the field
                        //extension
poly g=z2+z+1;          //polynomial to factorize
resultant(f,g,x);
resultant(f,subst(g,z,z+x),x);

poly h=y5+y3+y2+y+1;    //the minimal polynomial for the new 
                        //field extension
poly G=subst(g,z,z+xy); //the transformed polynomial
poly r=resultant(f,G,x);  
r;                      //the norm r of the transformed 
                        //polynomial


poly s=resultant(h,r,y);
s;                      //the norm of r

factorize(s);

ring S=(2,y),(z,x),lp;
minpoly=y5+y3+y2+y+1;
poly r=imap(R,r);
poly f1=gcd(r,z10+z9+z7+z6+z3+z2+1); //the gcd with the first 
                                     //factor
f1;
f1=simplify(f1,1);                   //we normalize f1
f1;
poly f2=gcd(r,z10+z9+z7+z5+z2+z+1); //the gcd with the second 
                                    //factor 
f2=simplify(f2,1);
f2;

r-f1*f2;                  //test the result

f1=subst(f1,z,z+x*y);     //the inverse transformation
f1=simplify(f1,1);
f1;
f2=subst(f2,z,z+x*y);     //the inverse transformation
f2=simplify(f2,1);
f2;

setring R;
ideal I=std(ideal(f,h));
qring T=I;
option(redSB);
poly f1=imap(S,f1);
poly f2=imap(S,f2);
poly g=imap(R,g);
std(ideal(f1,g));       //gcd of g with the first factor

std(ideal(f2,g));       //gcd of g with the second factor

std((x+z+1)*(x+z));     //test the result

tst_status(1);$

