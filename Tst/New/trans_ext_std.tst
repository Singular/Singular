LIB "tst.lib"; tst_init();
ring R = (0, I, T, Px, Py, Cx, Cy, Sx, Sy, a, b, dt, dx, dy), (i, t, x, y, cx, cy, sx, sy), (c,lp);
ideal Id;
Id[1]=i^2+1;
Id[2]=cx^2+sx^2-1;
Id[3]=cy^2+sy^2-1;
std(Id); // GB in Q( I,T,Px,Py,Cx,Cy,Sx,Sy,a,b,dt,dx,dy )[ i,t,x,y,cx,cy,sx,sy ]!
ring abP = (0,a,b),(p4,p5),dp;
ideal abE;
abE[1]=-p4+p5+(ab+a+b2+3b);
abE[2]=-p5+(a2+2a);
abE[3]=(a-b)*p4+(-a)*p5+(-a2+b3+3b2);
abE;
option(redSB);
option(redTail);
std(abE);
tst_status(1);$

