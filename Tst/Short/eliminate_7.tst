LIB "tst.lib"; tst_init(); option(prot);

ring testh4 = 31991,(u,v,w,x,y,z),dp;  
ideal i =
xy+y2+uz-wz-xz+yz,
x2-y2+uz-wz-xz+z2,
wy, wx-uz+yz,
w2-y2+uz-z2,
vz-yz-z2, 
vy-y2-wz+xz+yz+z2,
vx-y2+uz+yz-z2,
vw-y2+uz+yz-z2,
v2+y2+uz-xz-yz-z2,
uy+y2+yz, 
ux-y2+wz-xz-z2,
uw-y2+uz+yz-z2,
uv-y2-xz+yz,
u2+yz; 
ideal k = i^2; 
eliminate(k,uvw);
tst_status(1);$
