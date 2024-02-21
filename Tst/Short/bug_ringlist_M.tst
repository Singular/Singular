LIB "tst.lib";
tst_init();

int nx = 2;
int ny = 2;
int i,j,k,l;

intmat imat[nx+ny][nx+ny];
for(i=1;i<=nx+ny;i++){
        imat[1,i] = -1;
}
for(i=1;i<=ny;i++){
        imat[i+1,ny+nx-i+1] = 1;
}
for(i=1;i<=nx-1;i++){
        imat[i+ny+1,nx-i+1] = 1;
}

ring R = (0,a), (x(1..2),y(1..2)), (M(imat),C);

ideal E = 0;
ideal N = 1;
par2varRing(list(E,N));

tst_status(1);$


