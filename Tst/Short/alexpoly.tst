// File: alexpoly.tst
// Tests for alexpoly lib
LIB "tst.lib";
tst_init();
LIB "alexpoly.lib";
ring r=0,(x,y),ds;
//////////////////////////////////////////////////////////////////////////
//  Defining examples.
//////////////////////////////////////////////////////////////////////////
//  Examples of polynomials
/////////////////////////////////////////////////////////////////////////
list f;
f[1] =x2-y2;
f[2] =x2+y+y2;
f[3] =(x2+y3)*(x2+y3+xy2);
f[4] =-x27-x25-15x24y-30x23y2+5x20y3-135x19y4+3x18y5-10x15y6-90x14y7+10x10y9-3x9y10-5x5y12+y15;
f[5] =x5-y11;
f[6] =xy8+y8+x4y6+4x3y6+2x5y5+6x6y4+4x8y3+x10y2+4x9y2+2x11y+x12;
f[7] =(x6-y4);
f[8] =(((y-x2+x3)*(y-x2-x3)));
f[9] =((x7-2x4y2+xy4-1y5)*(x7-4x4y2+4xy4-1y5));
f[10]=((y2-x3)*(y2-x3-x4));
f[11]=((y2-x3-x4)*(y2+x3+x4));
f[12]=(((x2-y)^2+x5)*((2x2-y)^2+x5));
f[13]=((x2-y4)*(x+y4));
f[14]=-x9+x8-6x7y+3x6y2-2x4y3-3x3y4+y6;
f[15]=-x21+x20-8x18y-4x15y2-8x13y3+6x10y4-4x5y6+y8;
f[16]=-x19+x18-12x17y-6x15y2-40x14y3+15x12y4-12x11y5-20x9y6+15x6y8-6x3y10+y12;
f[17]=x22-x21-14x20y+7x18y2-70x17y3-21x15y4-42x14y5+35x12y6-2x11y7-35x9y8+21x6y10-7x3y12+y14;
f[18]=-x17-2x16-x15-20x13y2+5x12y2-10x10y4-10x9y4+10x6y6-5x3y8+y10;
f[19]=(f[16]*f[17]*f[18]);
f[20]=((x2-y3)*(x3-y5)*(x5-y7)*(x7-y11)*(x11-y13));
f[21]=((x3+3x2y-xy4+y10)*(x3-x2y+y8));
f[22]=-x11+x10-4x8y-2x5y2+y4;
f[23]=x7-y8;
f[24]=x15-y16;
f[25]=f[1]*f[2];
f[26]=f[2]*f[3];
f[27]=f[4]*f[5];
f[28]=f[1]*f[2]*f[3]*f[4]*f[5];
f[29]=f[14]*f[15];
f[30]=f[6]*f[7];
f[31]=f[6]*f[8]*f[12];
f[32]=2x2+3xy+4xy3-x2y;
f[33]=(x-y)*(x-2y)*(x-3y)*(x-4y);
f[34]=(x-y)*(x-2y)*(x-3y)*(x-4y)*(x-5y);
f[35]=(x7-y3)*(y4-2x3y2-4x5y+x6-x7)*(x2-y11);
f[36]=-x23-2x22-x21-42x19y2+7x18y2-70x16y4-21x15y4-14x13y6+35x12y6-35x9y8+21x6y10-7x3y12+y14;
f[37]=-x29-x28+7x24y-21x20y2+35x16y3-35x12y4+21x8y5-7x4y6+y7;

poly p_1 = y2+x3;
poly p_2 = p_1^2 + x5y;
poly p_3 = p_2^2 + x^10 *p_1;
poly p_4 = p_3^2 + x^20 *p_2;
poly p_5 = p_4^2 + x^40 *p_3;

f[38]=p_1;
f[39]=p_2;
f[40]=p_3;
f[41]=p_4;
f[42]=p_5;
f[43]=p_1*p_2*p_3;
f[44]=p_3*p_5;

f[45]=(-x7+x6-4x5y-2x3y2+y4)*(-x21+x20-12x19y-30x17y2-4x15y3+3x14y4-48x12y5+6x10y6-3x7y8-4x5y9+y12);
f[46]=f[16]*f[17]*f[18]*f[22]*f[23]*f[24];
f[47]=(x5-y7)*(x10-y17);
f[48]=(x5-y7)*(x13-y23);
f[49]=(x5-y7)*(x4383-y5344);
f[50]=(x5-y7)*(x10-y17)*(x7-y11);

list f_irr=x-y,x+y,y-x2+x3,y2-x3-x4,(x2-y)^2+x5,(2x2-y)^2+x5,x-y2,x+y2,x+y4,x3-y5,x5-y7,x7-y11,x11-y13,f[2],f[4],f[5],f[6],f[14],f[15],f[16],f[17],f[18],f[22],f[23],f[24],f[36],f[37],f[38],f[39],f[40],f[41],f[42],(-x7+x6-4x5y-2x3y2+y4),(-x21+x20-12x19y-30x17y2-4x15y3+3x14y4-48x12y5+6x10y6-3x7y8-4x5y9+y12),(x10-y17),(x13-y23),x204-y1111,x4383-y5344;

///////////////////////////////////////////////////////////////////////////////////////////
// Defining the invariants of the above examples.
///////////////////////////////////////////////////////////////////////////////////////////
list FF;
//Polynomial: f[1]=x2-y2
FF[1]=list(intmat(intvec(0,1,1,0),2,2),list(intvec(1),intvec(1)));
//Polynomial: f[2]=y+x2+y2
FF[2]=list(intmat(intvec(0),1,1),list(intvec(1)));
//Polynomial: f[3]=x4+x3y2+2x2y3+xy5+y6
FF[3]=list(intmat(intvec(0,4,4,0),2,2),list(intvec(2,3),intvec(2,3)));
//Polynomial: f[4]=y15-5x5y12+10x10y9-3x9y10-10x15y6-90x14y7+5x20y3-135x19y4+3x18y5-x25-15x24y-30x23y2-x27
FF[4]=list(intmat(intvec(0),1,1),list(intvec(15,25,27)));
//Polynomial: f[5]=x5-y11
FF[5]=list(intmat(intvec(0),1,1),list(intvec(5,11)));
//Polynomial: f[6]=y8+4x3y6+xy8+6x6y4+2x5y5+x4y6+4x9y2+4x8y3+x12+2x11y+x10y2
FF[6]=list(intmat(intvec(0),1,1),list(intvec(8,12,14,15)));
//Polynomial: f[7]=-y4+x6
FF[7]=list(intmat(intvec(0,3,3,0),2,2),list(intvec(2,3),intvec(2,3)));
//Polynomial: f[8]=y2-2x2y+x4-x6
FF[8]=list(intmat(intvec(0,3,3,0),2,2),list(intvec(1),intvec(1)));
//Polynomial: f[9]=4x2y8-5xy9+y10-12x5y6+6x4y7+13x8y4-2x7y5-6x11y2+x14
FF[9]=list(intmat(intvec(0,1,1,1,1,0,1,1,1,1,0,3,1,1,3,0),4,4),list(intvec(1),intvec(1),intvec(4,6,7),intvec(4,6,7)));
//Polynomial: f[10]=y4-2x3y2+x6-x4y2+x7
FF[10]=list(intmat(intvec(0,5,5,0),2,2),list(intvec(2,3),intvec(2,3)));
//Polynomial: f[11]=y4-x6-2x7-x8
FF[11]=list(intmat(intvec(0,3,3,0),2,2),list(intvec(2,3),intvec(2,3)));
//Polynomial: f[12]=y4-6x2y3+13x4y2-12x6y+2x5y2+4x8-6x7y+5x9+x10
FF[12]=list(intmat(intvec(0,2,2,0),2,2),list(intvec(2,5),intvec(2,5)));
//Polynomial: f[13]=x3-xy4+x2y4-y8
FF[13]=list(intmat(intvec(0,2,2,2,0,2,2,2,0),3,3),list(intvec(1),intvec(1),intvec(1)));
//Polynomial: f[14]=y6-2x4y3-3x3y4+x8-6x7y+3x6y2-x9
FF[14]=list(intmat(intvec(0),1,1),list(intvec(6,8,9)));
//Polynomial: f[15]=y8-4x5y6+6x10y4-8x13y3-4x15y2-8x18y+x20-x21
FF[15]=list(intmat(intvec(0),1,1),list(intvec(8,20,21)));
//Polynomial: f[16]=y12-6x3y10+15x6y8-20x9y6+15x12y4-12x11y5-6x15y2-40x14y3+x18-12x17y-x19
FF[16]=list(intmat(intvec(0),1,1),list(intvec(12,18,19)));
//Polynomial: f[17]=y14-7x3y12+21x6y10-35x9y8+35x12y6-2x11y7-21x15y4-42x14y5+7x18y2-70x17y3-x21-14x20y+x22
FF[17]=list(intmat(intvec(0),1,1),list(intvec(14,21,22)));
//Polynomial: f[18]=y10-5x3y8+10x6y6-10x9y4+5x12y2-10x10y4-x15-20x13y2-2x16-x17
FF[18]=list(intmat(intvec(0),1,1),list(intvec(10,15,17)));
//Polynomial: f[19]
FF[19]=list(intmat(intvec(0,9,6,9,0,6,6,6,0),3,3),list(intvec(14,21,22),intvec(12,18,19),intvec(10,15,17)));
//Polynomial: f[20]
FF[20]=list(intmat(intvec(0,4,3,3,3,4,0,3,3,3,3,3,0,3,3,3,3,3,0,4,3,3,3,4,0),5,5),list(intvec(11,13),intvec(5,7),intvec(2,3),intvec(7,11),intvec(3,5)));
//Polynomial: f[21]=x6+2x5y-3x4y2-x4y4+x3y5+x3y8+3x2y9+x3y10-x2y11-xy12+y18
FF[21]=list(intmat(intvec(0,1,1,1,1,1,0,1,1,1,1,1,0,3,3,1,1,3,0,4,1,1,3,4,0),5,5),list(intvec(1),intvec(1),intvec(1),intvec(2,7),intvec(1)));
//Polynomial: f[22]=y4-2x5y2-4x8y+x10-x11
FF[22]=list(intmat(intvec(0),1,1),list(intvec(4,10,11)));
//Polynomial: f[23]=x7-y8
FF[23]=list(intmat(intvec(0),1,1),list(intvec(7,8)));
//Polynomial: f[24]=x15-y16
FF[24]=list(intmat(intvec(0),1,1),list(intvec(15,16)));
//Polynomial: f[25]=x2y-y3+x4-y4
FF[25]=list(intmat(intvec(0,1,1,1,0,1,1,1,0),3,3),list(intvec(1),intvec(1),intvec(1)));
//Polynomial: f[26]=x4y+x6+x4y2+x3y3+2x2y4+x5y2+2x4y3+x3y4+2x2y5+xy6+y7+x3y5+x2y6+xy7+y8
FF[26]=list(intmat(intvec(0,1,1,1,0,4,1,4,0),3,3),list(intvec(1),intvec(2,3),intvec(2,3)));
//Polynomial: f[27]
FF[27]=list(intmat(intvec(0,1,1,0),2,2),list(intvec(15,25,27),intvec(5,11)));
//Polynomial: f[28]
FF[28]=list(intmat(intvec(0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,2,1,1,1,1,1,2,0,1,1,1,1,1,1,1,0,4,2,1,1,1,1,4,0,2,1,1,1,1,2,2,0),7,7),list(intvec(1),intvec(1),intvec(15,25,27),intvec(1),intvec(2,3),intvec(2,3),intvec(5,11)));
//Polynomial: f[29]
FF[29]=list(intmat(intvec(0,2,2,0),2,2),list(intvec(6,8,9),intvec(8,20,21)));
//Polynomial: f[30]=-y12-4x3y10-xy12-5x6y8-2x5y9-x4y10-4x8y7+x7y8+5x12y4+4x15y2+4x14y3+x18+2x17y+x16y2
FF[30]=list(intmat(intvec(0,3,3,3,0,4,3,4,0),3,3),list(intvec(2,3),intvec(2,3),intvec(8,12,14,15)));
//Polynomial: f[31]
FF[31]=list(intmat(intvec(0,2,2,2,2,2,0,2,2,2,2,2,0,3,3,2,2,3,0,3,2,2,3,3,0),5,5),list(intvec(8,12,14,15),intvec(2,5),intvec(2,5),intvec(1),intvec(1)));
//Polynomial: f[32]=2x2+3xy-x2y+4xy3
FF[32]=list(intmat(intvec(0,1,1,0),2,2),list(intvec(1),intvec(1)));
//Polynomial: f[33]=x4-10x3y+35x2y2-50xy3+24y4
FF[33]=list(intmat(intvec(0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,0),4,4),list(intvec(1),intvec(1),intvec(1),intvec(1)));
//Polynomial: f[34]=x5-15x4y+85x3y2-225x2y3+274xy4-120y5
FF[34]=list(intmat(intvec(0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0),5,5),list(intvec(1),intvec(1),intvec(1),intvec(1),intvec(1)));
//Polynomial: f[35]
FF[35]=list(intmat(intvec(0,2,1,2,0,1,1,1,0),3,3),list(intvec(4,6,7),intvec(3,7),intvec(2,11)));
//Polynomial: f[36]=y14-7x3y12+21x6y10-35x9y8+35x12y6-21x15y4-14x13y6+7x18y2-70x16y4-x21-42x19y2-2x22-x23
FF[36]=list(intmat(intvec(0),1,1),list(intvec(14,21,23)));
//Polynomial: f[37]=y7-7x4y6+21x8y5-35x12y4+35x16y3-21x20y2+7x24y-x28-x29
FF[37]=list(intmat(intvec(0),1,1),list(intvec(7,29)));
//Polynomial: f[38]=y2+x3
FF[38]=list(intmat(intvec(0),1,1),list(intvec(2,3)));
//Polynomial: f[39]=y4+2x3y2+x6+x5y
FF[39]=list(intmat(intvec(0),1,1),list(intvec(4,6,7)));
//Polynomial: f[40]=y8+4x3y6+6x6y4+2x5y5+4x9y2+4x8y3+x12+2x11y+2x10y2+x13
FF[40]=list(intmat(intvec(0),1,1),list(intvec(8,12,14,15)));
//Polynomial: f[41]
FF[41]=list(intmat(intvec(0),1,1),list(intvec(16,24,28,30,31)));
//Polynomial: f[42]
FF[42]=list(intmat(intvec(0),1,1),list(intvec(32,48,56,60,62,63)));
//Polynomial: f[43]
FF[43]=list(intmat(intvec(0,4,4,4,0,6,4,6,0),3,3),list(intvec(2,3),intvec(8,12,14,15),intvec(4,6,7)));
//Polynomial: f[44]
FF[44]=list(intmat(intvec(0,8,8,0),2,2),list(intvec(32,48,56,60,62,63),intvec(8,12,14,15)));
//Polynomial: f[45]
FF[45]=list(intmat(intvec(0,3,3,0),2,2),list(intvec(4,6,7),intvec(12,20,21)));
//Polynomial: f[46]
FF[46]=list(intmat(intvec(0,1,1,2,2,2,1,0,8,1,1,1,1,8,0,1,1,1,2,1,1,0,9,6,2,1,1,9,0,6,2,1,1,6,6,0),6,6),list(intvec(4,10,11),intvec(7,8),intvec(15,16),intvec(12,18,19),intvec(14,21,22),intvec(10,15,17)));
//Polynomial: f[47]=(x5-y7)(x10-y17)
FF[47]=list(intmat(intvec(0,3,3,0),2,2),list(intvec(5,7),intvec(10,17)));
//Polynomial: f[48]=(x5-y7)(x13-y23)
FF[48]=list(intmat(intvec(0,3,3,0),2,2),list(intvec(5,7),intvec(13,23)));
//Polynomial: f[49]=(x5-y7)(x4383-y5344)
FF[49]=list(intmat(intvec(0,4,4,0),2,2),list(intvec(5,7),intvec(4383,5344)));
//Polynomial: f[50]=(x5-y7)(x10-y17)(x7-y11)
FF[50]=list(intmat(intvec(0,3,3,3,0,4,3,4,0),3,3),list(intvec(5,7),intvec(10,17),intvec(7,11)));
//Polynomial: f[51]=f[4]*(x2+y3)*f[5];
FF[51]=list(intmat(intvec(0,1,1,1,0,2,1,2,0),3,3),list(intvec(15,25,27),intvec(2,3),intvec(5,11)));

//////////////////////////////////////////////////////////////////////////////////////
/// Examples, created from f_irr 
//////////////////////////////////////////////////////////////////////////////////////
// Consider the product of all the polynomials in f_irr.
// -----------------------------------------------------------------------
// examples:
example resolutiongraph;
example totalmultiplicities;
example alexanderpolynomial;
example semigroup;
example multseq2charexp;
example charexp2multseq;
example charexp2generators;
example charexp2inter;
example charexp2conductor;
example charexp2poly;
example tau_es2;

tst_status(1);$
