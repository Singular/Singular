ring r=32003,(x,y,z),(c,dp);
ideal q=x2,y2;
qring qr=q;
ideal i=x;
syz(i);
i=x,y,z;
syz(i);
mres(i,3,ii);
ii(1);
ii(2);
ii(3);
$

