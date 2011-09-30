LIB "tst.lib"; tst_init();
ring r = (0,A,B,dt,dx,dy),(Tx,Ty,Tt),(c,dp);
r;
matrix M[4][4] =
    // direct equation:
    -A^2, -B^2, 1, 0,
    // central difference u_tt
    0, 0,  -dt^2*Tt, (Tt-1)^2,
    // central difference u_xx
    -dx^2*Tx, 0, 0, (Tx-1)^2,
    // central difference u_yy
    0, -dy^2*Ty, 0, (Ty-1)^2;

print(M);

//=========================================
// CHECK THE CORRECTNESS OF EQUATIONS AS INPUT:
ring rX = (0,A,B,dt,dx,dy,Tx,Ty,Tt),(Uxx, Uyy,Utt, U),(c,Dp);
rX;
def M = imap(r,M); // T? -> params! // BUG!

print(M);

vector X = [Uxx, Uyy, Utt, U];
print(M*X);

// END CHECK
//=========================================
tst_status(1);$

