// -----------
// normalform etc
// -----------
ring r= 32003,(x,y,z),lp;
ideal i = x,y;
poly p = x2y + z;
poly noe = noether;
noether = noe;
// noe;

"differentials from ", p;
"d/dx:", diff(p,x);
"d/dxdy:", diff(diff(p,x),y);
"d/dy:", diff(p,y);
"d/dz:", diff(p,z);

newline; "normalform:";
reduce(p,i);

newline; "lead ?";
lead(p);
$
