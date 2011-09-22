LIB "tst.lib"; tst_init();
LIB "inout.lib";
ring r= 0,(x,y,z),ds;
poly f=((x+y)*(x-y)*(x+z)*(y+z)^2);
lprint(f,40);
module m = [f*(x-y)],[0,f*(x-y)];
string s=lprint(m); s;"";
execute("matrix M[2][2]="+s+";");      //use the string s as input
module m1 = transpose(M);              //should be the same as m
print(matrix(m)-matrix(m1));
tst_status(1);$
