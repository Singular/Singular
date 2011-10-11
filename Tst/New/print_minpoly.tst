LIB "tst.lib"; tst_init();

proc TestRingPrinting(def r, string n)
{
  def save = basering;
  
  setring r;
  
  "VVVVVVVVVVVV[", n, "]VVVVVVVVVVVV";
  
  "r == currRing (the following type and print should yield the same output!): ";
  "type: "; r;
  "print(ring): "; print(r);  

  ring @temp = 2,@a,ds; setring @temp;
  
  "r != currRing (the following type and print may be different!): ";  
  "type: "; r;
  "print(ring): "; print(r);
  
  "^^^^^^^^^^^^[", n, "]^^^^^^^^^^^^";
  kill @temp;
  
  setring save;
}
int i;
ring R = (9,a),x,dp;

TestRingPrinting(R, "commutative polynomial ring over GF(3^2)");

for(i = 0; i<= 20; i++)
{
  "exp: ", i, ", a^exp: ", a^i;
}

kill R;






ring R = (0,a), x,dp; minpoly = a2+1;

TestRingPrinting(R, "commutative polynomial ring over Q[a]/<a2+1>");

for(i = 0; i<= 20; i++)
{
  "exp: ", i, ", a^exp: ", a^i;
}

kill R;




ring R = (complex,30,a), x,dp; 

TestRingPrinting(R, "commutative polynomial ring over GMP complex numbers with parameter: 'a'");

for(i = 0; i<= 20; i++)
{
  "exp: ", i, ", a^exp: ", a^i;
}


kill R;


tst_status(1);$
