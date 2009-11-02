// Test File for 'zeroset.lib"
//
// Implementation by : Thomas Bayer
// Curretn Adress:
// Institut fuer Informatik, Technische Universitaet Muenchen
// www:    http://wwwmayr.informatik.tu-muenchen.de/personen/bayert/
// email : bayert@in.tum.de
//
// Last change 10.12.2000
// 
// written in the frame of the diploma thesis (advisor: Prof. Gert-Martin Greuel)
// "Computations of moduli spaces of semiquasihomogenous singularities and an
//  implementation in Singular"
// Arbeitsgruppe Algebraische Geometrie, Fachbereich Mathematik,
// Universitaet Kaiserslautern
///////////////////////////////////////////////////////////////////////////////

LIB "tst.lib";
LIB "zeroset.lib";         

tst_init();
tst_ignore("CVS ID $Id$"); 
 


proc TestzerosetLIB(S)
// plug in the elements from the zero-set in the generators of the ideal
// result should be 0,0,...,0.
{
     
        setring S;
        map F;

        print(" #Points = " + string(vdim(std(id))) + "; #Solutions = " + string(size(theZeroset)));
        print(" minpoly = " + string(minpoly)); 
        print(" ideal = " + string(id));
        

        for(int i = 1; i <= size(theZeroset); i++) {
                F = theZeroset[i];
                print(string(theZeroset[i]) + " --> " + string(F(id)));
        }
}

// example 1

ring R = 0,(x,y,z), lp;
ideal I = x2-1,y3-1,z-2x;

def S = zeroSet(I);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;

kill R;
kill S;
//tst_status();
 
// example 2

ring R = 0,(x,y,z), lp;
ideal I = x3-1,3y-x,z4-1;

def S = zeroSet(I);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;
       
kill R;
kill S;
//tst_status();

// example 3

ring R = 0, (x(1..5)), lp;
ideal I = x(1)^2+1,x(2)^4-1,x(3)-x(4),x(4)^2+1,x(5)-x(4);

def S = zeroSet(I);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;

kill R;
kill S;
//tst_status();

// example 4

ring R = (0,a),(x,y), lp;
minpoly = a2+1;
ideal I = x2-a,y-2x;

def S = zeroSet(I);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;

kill R;
kill S;
//tst_status();
         
// example 5

ring R = (0,a),(x,y,z), lp;
minpoly = a2+a+1;
ideal I = x3-1,a*y^2-x,z^2-a*y;

def S = zeroSet(I);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;

kill R;
kill S;
//tst_status();

// example 6

ring R = (0,a),(x,y,z), lp;
minpoly = a2+1;
ideal I = x2-2,a*y^2-2*x,z^2 - a; // 2

def S = zeroSet(I);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;

kill R;
kill S;
//tst_status();
         
// example 7

ring R = (0,a),(x(1..5)), lp;
minpoly = a2+a+1;
ideal I = x(1)^2 - 2,x(2) -a*x(1), x(3)^2 + 1, x(4)^2 + a, a*x(5) - x(1);

def S = zeroSet(I);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;

kill S;

setring R;
def S = zeroSet(I, 1);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;

kill R;
kill S;

//tst_status();

// example 8

ring R = (0,a),(s(1..4)), lp;
minpoly = a2+1;
ideal I = s(3)*s(4),s(2)*s(4),s(1)*s(3),s(1)*s(2),s(3)^8+s(4)^8-1,s(2)^8+s(4)^8-1,s(1)^8-s(4)^8,s(4)^9-s(4),s(1)*s(4)^8-s(1);

def S = zeroSet(I);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;

kill S;

setring R;
def S = zeroSet(I, 1);TestzerosetLIB(S);
setring S;
minpoly;
id;
theZeroset;

kill S;

tst_status(1); $
