echo = 2;

"ndebug?: ", system("with", "ndebug");
"om_ndebug?: ", system("with", "om_ndebug");

if( system("with", "ndebug") )
{
  "Loading the Release version!";
  LIB("./syzextra.so");
}
else
{
  "Loading the Debug version!";
  LIB("./syzextra.sog");
}

noop();

ring r;

DetailedPrint(r);

poly p = x +y +z;

DetailedPrint(p, 2);

ideal I = maxideal(1);

DetailedPrint(I, 2);

vector v =3 *xyz * [0, 0, 1, x, y, z];

v;
DetailedPrint(v, 2);

leadmonom(v);
leadcomp(v);
leadrawexp(v);




def l = res(I, 0); DetailedPrint(l);

$$$

