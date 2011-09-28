LIB "tst.lib"; tst_init();
ring R = (0, I, T, Px, Py, Cx, Cy, Sx, Sy, a, b, dt, dx, dy), (i, t, x, y, cx, cy, sx, sy), (c,lp);
ideal Id;
Id[1]=i^2+1;
Id[2]=cx^2+sx^2-1;
Id[3]=cy^2+sy^2-1;
std(Id); // GB in Q( I,T,Px,Py,Cx,Cy,Sx,Sy,a,b,dt,dx,dy )[ i,t,x,y,cx,cy,sx,sy ]!
tst_status(1);$

