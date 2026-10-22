LIB "tst.lib";
tst_init();

proc mrAssert(int condition, string message)
{
  if (!condition) { ERROR("MONRES_CHECK_FAILED: " + message); }
}

proc mrSameBetti(intmat actual, intmat expected, string label)
{
  mrAssert(nrows(actual)==nrows(expected), label+": Betti row count");
  mrAssert(ncols(actual)==ncols(expected), label+": Betti column count");
  mrAssert(actual==expected, label+": graded Betti table");
}

proc mrCheckComplex(ideal inputIdeal, resolution candidate, string label)
{
  list differentials=list(candidate);
  mrAssert(size(differentials)>0,label+": missing first differential");
  module previous=differentials[1];
  module inputModule=inputIdeal;
  mrAssert(size(reduce(previous,std(inputModule)))==0,label+": image contained in ideal");
  mrAssert(size(reduce(inputModule,std(previous)))==0,label+": ideal contained in image");
  int unitIdeal=(reduce(ideal(1),std(inputIdeal))==0);
  int stage; int rowIndex; int columnIndex;
  matrix differential=matrix(previous,1,max(1,size(previous)));
  if (!unitIdeal)
  {
    for (columnIndex=1;columnIndex<=ncols(differential);columnIndex++)
    {
      mrAssert(jet(differential[1,columnIndex],0)==0,label+": minimal first differential");
    }
  }
  if (size(previous)==0)
  {
    // Singular may materialize a formal 0 <- R <- R tail for the zero
    // ideal. Its minimal Betti table is checked by the caller; no nonzero
    // image exists here, and the compact representation must have rank [1].
    return(1);
  }
  module kernelModule;
  module current;
  matrix nextDifferential;
  for (stage=2;stage<=size(differentials);stage++)
  {
    current=differentials[stage];
    kernelModule=syz(previous);
    mrAssert(size(reduce(current,std(kernelModule)))==0,label+": image contained in kernel at "+string(stage));
    mrAssert(size(reduce(kernelModule,std(current)))==0,label+": kernel contained in image at "+string(stage));
    if (size(current)==0)
    {
      for (stage=stage+1;stage<=size(differentials);stage++)
      {
        mrAssert(size(differentials[stage])==0,label+": nonzero tail after zero");
      }
      return(1);
    }
    nextDifferential=matrix(current,size(previous),size(current));
    mrAssert(size(ideal(differential*nextDifferential))==0,label+": differential product at "+string(stage));
    for (rowIndex=1;rowIndex<=nrows(nextDifferential);rowIndex++)
    {
      for (columnIndex=1;columnIndex<=ncols(nextDifferential);columnIndex++)
      {
        mrAssert(jet(nextDifferential[rowIndex,columnIndex],0)==0,label+": nonminimal entry at "+string(stage));
      }
    }
    previous=current;
    differential=nextDifferential;
  }
  mrAssert(size(syz(previous))==0,label+": last differential is not injective");
  return(1);
}

proc mrCheckBox(ideal inputIdeal, def compact, string label)
{
  intvec ranks=monresRanks(compact);
  mrAssert(ranks[1]==1,label+": rank of F0");
  if (size(inputIdeal)==0) { mrAssert(size(ranks)==1,label+": zero ideal ranks"); }
  resolution explicitResolution=monresExplicit(compact);
  mrSameBetti(betti(explicitResolution),betti(mres(inputIdeal,0)),label+": compact Betti");
  mrCheckComplex(inputIdeal,explicitResolution,label+": compact explicit");
  int stage; int rowIndex; int columnIndex; int variableIndex;
  intmat sourceDegrees; intmat targetDegrees;
  matrix differential;
  vector sourceVector; vector expectedImage; module imageModule;
  intvec entryExponents;
  for (stage=0;stage<size(ranks);stage++)
  {
    sourceDegrees=monresDegrees(compact,stage);
    mrAssert(nrows(sourceDegrees)==nvars(basering),label+": degree rows");
    mrAssert(ncols(sourceDegrees)==ranks[stage+1],label+": degree columns");
    if (stage==0)
    {
      for (variableIndex=1;variableIndex<=nvars(basering);variableIndex++)
      {
        mrAssert(sourceDegrees[variableIndex,1]==0,label+": degree of F0");
      }
    }
    else
    {
      targetDegrees=monresDegrees(compact,stage-1);
      differential=monresMatrix(compact,stage);
      mrAssert(nrows(differential)==ranks[stage],label+": matrix rows");
      mrAssert(ncols(differential)==ranks[stage+1],label+": matrix columns");
      sourceVector=0;
      for (columnIndex=1;columnIndex<=ranks[stage+1];columnIndex++)
      {
        sourceVector=sourceVector+(var(1)^2+columnIndex*var(nvars(basering)))*gen(columnIndex);
        for (rowIndex=1;rowIndex<=ranks[stage];rowIndex++)
        {
          if (differential[rowIndex,columnIndex]!=0)
          {
            mrAssert(size(differential[rowIndex,columnIndex])==1,label+": multigraded matrix entry");
            entryExponents=leadexp(differential[rowIndex,columnIndex]);
            for (variableIndex=1;variableIndex<=nvars(basering);variableIndex++)
            {
              mrAssert(entryExponents[variableIndex]==sourceDegrees[variableIndex,columnIndex]-targetDegrees[variableIndex,rowIndex],label+": multigrading");
            }
          }
        }
      }
      imageModule=module(differential*matrix(sourceVector,ranks[stage+1],1));
      expectedImage=imageModule[1];
      mrAssert(monresApply(compact,stage,sourceVector)==expectedImage,label+": direct apply");
    }
  }
  def copied=compact;
  mrAssert(monresRanks(copied)==ranks,label+": object copy");
  return(1);
}

proc mrCheck(ideal inputIdeal, int isSquarefree, string label)
{
  resolution referenceM=mres(inputIdeal,0);
  intmat expected=betti(referenceM);
  resolution candidate=monomialResolution(inputIdeal);
  mrSameBetti(betti(candidate),expected,label+": monomialResolution");
  mrCheckComplex(inputIdeal,candidate,label+": monomialResolution");
  mrCheckBox(inputIdeal,monomialResolutionBox(inputIdeal),label+": monomial box");
  if (isSquarefree)
  {
    candidate=squarefreeResolution(inputIdeal);
    mrSameBetti(betti(candidate),expected,label+": squarefreeResolution");
    mrCheckComplex(inputIdeal,candidate,label+": squarefreeResolution");
    mrCheckBox(inputIdeal,squarefreeResolutionBox(inputIdeal),label+": squarefree box");
  }
  print("MONRES_CHECK_OK "+label);
}

ring rationalRing=0,(x(1..6)),dp;
ideal I=x(1)^2*x(2),x(1)*x(3)^3,x(2)^2*x(4)^2,x(1)^3;
mrCheck(I,0,"rational-nonuniform");
I=x(1)*x(2),x(1)*x(3),x(3)*x(4);
mrCheck(I,1,"nonregular-linear-quotients");
I=x(1)^2,x(1)*x(2),x(2)^3,x(1)*x(3)^2,x(2)^2*x(3),x(2)*x(3)^3,x(3)^5;
mrCheck(I,0,"stable-mixed-degrees");
I=x(1)*x(2),x(2)^3;
mrCheck(I,0,"formula-degree-two-quotient");
def formulaGeneralBox=monomialResolutionBox(I,4);
mrAssert(monresRanks(formulaGeneralBox)==intvec(1,2,1),"formula general exact budget ranks");
matrix formulaGeneralD1=monresMatrix(formulaGeneralBox,1);
matrix formulaGeneralD2=monresMatrix(formulaGeneralBox,2);
mrAssert(formulaGeneralD1[1,1]==x(1)*x(2) && formulaGeneralD1[1,2]==x(2)^3,
         "formula general augmentation");
mrAssert(formulaGeneralD2[1,1]==-x(2)^2 && formulaGeneralD2[2,1]==x(1),
         "formula general degree-two quotient");
I=x(1)*x(2),x(1)*x(3),x(1)*x(4),x(2)*x(3)*x(4);
mrCheck(I,1,"squarefree-mixed-degrees");
I=0,x(1)*x(2),-x(1)*x(2),x(1)*x(2)*x(3),x(3)*x(4);
mrCheck(I,1,"redundant-squarefree");
I=0;
mrCheck(I,1,"zero");
I=1,x(1),0;
mrCheck(I,1,"unit");
I=x(1)^137*x(2)^5,x(1)^3*x(3)^211,x(2)^109*x(3)^7;
mrCheck(I,0,"large-exponents");
I=x(1)*x(2)*x(5),x(1)*x(2)*x(6),x(1)*x(3)*x(4),x(1)*x(3)*x(6),x(1)*x(4)*x(5),x(2)*x(3)*x(4),x(2)*x(3)*x(5),x(2)*x(4)*x(6),x(3)*x(5)*x(6),x(4)*x(5)*x(6);
mrCheck(I,1,"RP2-rational");
intmat characteristicBetti=betti(squarefreeResolution(I));
mrAssert(ncols(characteristicBetti)==4,"RP2 rational projective dimension");

ring binaryRing=2,(x(1..6)),dp;
ideal I=imap(rationalRing,I);
mrCheck(I,1,"RP2-binary");
intmat characteristicBetti=betti(squarefreeResolution(I));
mrAssert(ncols(characteristicBetti)==5,"RP2 characteristic two torsion");

ring smallPrimeRing=3,(x(1..4)),lp;
ideal I=x(1)^3,x(1)*x(2),x(2)*x(3)^2,x(4)^2;
mrCheck(I,0,"prime-lex-order");

ring wideRing=32003,(x(1..70)),dp;
ideal I=x(1)*x(65),x(2)*x(66),x(65)*x(70);
mrCheck(I,1,"more-than-64-variables");
I=x(70),x(1)*x(2);
mrCheck(I,1,"formula-second-support-word");
def formulaSquarefreeBox=squarefreeResolutionBox(I,4);
mrAssert(monresRanks(formulaSquarefreeBox)==intvec(1,2,1),"formula squarefree exact budget ranks");
matrix formulaSquarefreeD2=monresMatrix(formulaSquarefreeBox,2);
mrAssert(formulaSquarefreeD2[1,1]==-x(1)*x(2) && formulaSquarefreeD2[2,1]==x(70),
         "formula squarefree second support word");
intmat formulaSquarefreeDegree=monresDegrees(formulaSquarefreeBox,2);
mrAssert(formulaSquarefreeDegree[1,1]==1 && formulaSquarefreeDegree[2,1]==1
         && formulaSquarefreeDegree[70,1]==1,
         "formula squarefree multidegree across support words");

// The known rejection output is part of the standard regression reference.
"reject a nonmonomial generator";
monomialResolution(ideal(x(1)+x(2)));
"reject a nonsquarefree generator";
squarefreeResolution(ideal(x(1)^2));
"reject an invalid construction budget";
monomialResolution(I,0);

tst_status(1);$
