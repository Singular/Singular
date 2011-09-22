LIB "tst.lib"; tst_init();
proc milnorNumber (poly g)
{
   "Milnor number:";
   return(vdim(std(jacob(g))));
}

proc tjurinaNumber
{
   "Tjurina number:";
   return(vdim(std(jacob(#[1])+#[1])));
}

proc milnor_tjurina (poly f)
{
   ideal j=jacob(f);
   list L=vdim(std(j)),vdim(std(j+f));
   return(L);
}

proc real_sols (number b, number c)
"USAGE: real_sols (b,c);  b,c number
ASSUME: active basering has characteristic 0
RETURN: list: first entry is an integer (the number of different real
        solutions). If this number is non-negative, the list has as second
        entry a ring in which the list SOL of real solutions of x^2+bx+c=0
        is stored (as floating point number, precision 30 digits).
NOTE:   This procedure calls laguerre_solve from solve.lib.
"
{
  def oldring = basering;  // assign name to the ring active when
                           // calling the procedure
  number disc = b^2-4*c;
  if (disc>0) { int n_of_sols = 2; }
  if (disc==0) { int n_of_sols = 1; }
  string s = nameof(var(1));  // name of first ring variable
  if (disc>=0) {
    execute("ring rinC =(complex,30),("+s+"),lp;");
    if (not(defined(laguerre_solve))) { LIB "solve.lib"; }
    poly f = x2+imap(oldring,b)*x+imap(oldring,c);
                        // f is a local ring-dependent variable
    list SOL = laguerre_solve(f,30);
    export SOL;         // make SOL a global ring-dependent variable
                        // such variables are still accessible when the
                        // ring is among the return values of the proc
    setring oldring;
    return(list(n_of_sols,rinC));
  }
  else {
    return(list(0));
  }
}

//
// We now apply the procedures which are defined by the
// lines of code above:
//
ring r = 0,(x,y),ds;
poly f = x7+y7+(x-y)^2*x2y2;

milnorNumber(f);
tjurinaNumber(f);
milnor_tjurina(f);     // a list containing Milnor and Tjurina number

def L=real_sols(2,1);
L[1];                  // number of real solutions of x^2+2x+1
def R1=L[2];
setring R1;
listvar(R1);           // only global ring-dependent objects are still alive
SOL;                   // the real solutions

setring r;
L=real_sols(1,1);
L[1];                  // number of reals solutions of x^2+x+1

setring r;
L=real_sols(1,-5);
L[1];                  // number of reals solutions of x^2+x-5
def R3=L[2];
setring R3; SOL;       // the real solutions
tst_status(1);$
