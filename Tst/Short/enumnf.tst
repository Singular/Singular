///////////////////////////////////////////////////////////////////////////////
// enumnf.tst
//
// Run from the directory containing enumnf.lib with:
//   Singular -teq enumnf.tst
// or from a Singular source tree test setup with:
//   ./regress.cmd enumnf.tst
///////////////////////////////////////////////////////////////////////////////

LIB "tst.lib";
tst_init();

ring R = 0, (x,y), ds;
LIB "enumnf.lib";

proc nnb_tst_assert(int cond, string msg)
{
  if (!cond)
  {
    "enumnf.tst assertion failed:";
    msg;
    ERROR("enumnf.tst assertion failed: " + msg);
  }
}

proc nnb_tst_assert_int(int got, int expected, string msg)
{
  if (got != expected)
  {
    ERROR("enumnf.tst assertion failed: " + msg
          + "; got " + string(got) + ", expected " + string(expected));
  }
}

proc nnb_tst_assert_str(string got, string expected, string msg)
{
  if (got != expected)
  {
    ERROR("enumnf.tst assertion failed: " + msg
          + "; got `" + got + "', expected `" + expected + "'");
  }
}

proc nnb_tst_assert_poly(poly got, poly expected, string msg)
{
  if (got != expected)
  {
    ERROR("enumnf.tst assertion failed: " + msg
          + "; got `" + string(got) + "', expected `" + string(expected) + "'");
  }
}

///////////////////////////////////////////////////////////////////////////////
// Finite Milnor-number bound mode.
///////////////////////////////////////////////////////////////////////////////

"enumnf.tst: finite Milnor-bound mode";

list E4 = NNBenumerateRaw(4, -1, 0);
nnb_tst_assert_int(size(E4), 2, "NNBenumerateRaw(4,-1,0) returns list(V,S)");
nnb_tst_assert_int(size(E4[1]), 16, "NNBenumerateRaw(4,-1,0) vertex count");
nnb_tst_assert_int(size(E4[2]), 4, "NNBenumerateRaw(4,-1,0) normal-form bucket count");

list G4 = NNBnormalFormsRaw(E4, 4, 0);
nnb_tst_assert_int(size(G4), 5, "NNBnormalFormsRaw(E4,4,0) returns modalities 0..4");
nnb_tst_assert_int(size(G4[1]), 4, "mu0=4 modality-zero grouped entries");
nnb_tst_assert(typeof(G4[1][1]) == "NNBNormalFormSeries",
               "grouped entries are represented uniformly as series");
nnb_tst_assert_int(size(NNBnormalFormSeriesParameters(G4[1][1])), 0,
                   "concrete grouped entry has no series parameter");
for (int i = 2; i <= 5; i++)
{
  nnb_tst_assert_int(size(G4[i]), 0, "mu0=4 has no grouped entries in positive modality");
}

// The first grouped finite series contains two internal representatives, so it
// is intentionally not directly evaluated by the public series API.
NNBNormalFormSeries f0 = G4[1][1];
nnb_tst_assert_int(size(f0.normalForms), 2,
                   "first finite grouped series has two representatives");
nnb_tst_assert_str(NNBnormalFormSeriesString(f0),
                   "normal form series of modality 0; no series parameter; normal forms: y^(3) + x^(2); y^(5) + x^(2)",
                   "NNBnormalFormSeriesString on first finite grouped series");

"enumnf.tst: NNBprintNormalFormSeries output";
NNBprintNormalFormSeries(f0);

// Use a single-representative series for public evaluation/manipulation.
NNBNormalFormSeries f1 = G4[1][2];
nnb_tst_assert_int(size(f1.normalForms), 1,
                   "second finite grouped series has one representative");
nnb_tst_assert_str(NNBnormalFormSeriesString(f1),
                   "normal form series of modality 0; no series parameter; normal forms: y^(3) + x^(1) * y^(1) + x^(3)",
                   "NNBnormalFormSeriesString on a finite single-representative series");

list X0 = NNBtermsAsExponents(f1);
nnb_tst_assert_int(size(X0), 3, "finite single-representative series has three terms");
nnb_tst_assert_str(X0[1][1], "0", "first finite term x exponent");
nnb_tst_assert_str(X0[1][2], "3", "first finite term y exponent");
nnb_tst_assert_str(X0[2][1], "1", "second finite term x exponent");
nnb_tst_assert_str(X0[2][2], "1", "second finite term y exponent");
nnb_tst_assert_str(X0[3][1], "3", "third finite term x exponent");
nnb_tst_assert_str(X0[3][2], "0", "third finite term y exponent");

poly p0 = NNBevaluateNormalFormSeries(f1);
nnb_tst_assert_poly(p0, x^3 + x*y + y^3,
                    "NNBevaluateNormalFormSeries on finite single-representative series");

list h0 = NNBhashFunction(f1);
nnb_tst_assert_int(size(h0), 3, "NNBhashFunction returns three components");
nnb_tst_assert_int(h0[1], 6, "NNBhashFunction area component for x^3+x*y+y^3");
nnb_tst_assert_int(h0[2], 3, "NNBhashFunction term-count component for x^3+x*y+y^3");
nnb_tst_assert_int(h0[3], 8, "NNBhashFunction coordinate-sum component for x^3+x*y+y^3");

list RB0 = NNBregularBasis(f1);
nnb_tst_assert_int(size(RB0), 0, "NNBregularBasis returns no moduli monomials for x^3+x*y+y^3");
list RB0p = NNBregularBasis(p0);
nnb_tst_assert_int(size(RB0p), 0, "NNBregularBasis also accepts an evaluated polynomial");

tst_status("finite-milnor-mode");

///////////////////////////////////////////////////////////////////////////////
// Simultaneous Milnor-number and modality bounds.  This returns individual
// NormalForm objects, not symbolic series; the above-boundary regular basis is
// stored in NormalForm.parameters, matching arnold.lib conventions.
///////////////////////////////////////////////////////////////////////////////

"enumnf.tst: bounded individual normal forms";

list MB4 = NNBenumerateBounded(4, -1, 0);
nnb_tst_assert_int(size(MB4), 5, "NNBenumerateBounded(4,-1,0) returns modalities 0..4");
nnb_tst_assert_int(size(MB4[1]), 5, "Milnor-only bounded mode modality-zero individual entries");
int mbmod;
for (mbmod = 2; mbmod <= size(MB4); mbmod++)
{
  nnb_tst_assert_int(size(MB4[mbmod]), 0,
                     "Milnor-only bounded mode has no positive-modality entries for mu0=4");
}
nnb_tst_assert(typeof(MB4[1][1]) == "NormalForm",
               "Milnor-only bounded mode returns Arnold NormalForm entries");
nnb_tst_assert_int(size(MB4[1][1].parameters), 0,
                   "Milnor-only bounded mode stores the above-boundary basis in parameters");

list B15 = NNBenumerateBounded(15, 2, 0);
nnb_tst_assert_int(size(B15), 3, "NNBenumerateBounded(15,2,0) returns modalities 0..2");
nnb_tst_assert_int(size(B15[1]), 30, "bounded mode modality-zero individual entries");
nnb_tst_assert_int(size(B15[2]), 30, "bounded mode modality-one individual entries");
nnb_tst_assert_int(size(B15[3]), 2, "bounded mode modality-two individual entries");

int bmod;
int bpos;
for (bmod = 1; bmod <= size(B15); bmod++)
{
  for (bpos = 1; bpos <= size(B15[bmod]); bpos++)
  {
    nnb_tst_assert(typeof(B15[bmod][bpos]) == "NormalForm",
                   "bounded mode returns individual NormalForm entries");
  }
}

NormalForm b0 = B15[1][1];
nnb_tst_assert_str(b0.singularityType, "(0,3),(2,0)",
                   "bounded Arnold NormalForm stores the singularity type");
nnb_tst_assert_int(b0.milnorNumber, 2, "bounded Arnold NormalForm stores Milnor number");
nnb_tst_assert_int(size(b0.parameters), 0, "modality-zero bounded form has no parameters");

"enumnf.tst: NormalForm print output from bounded mode";
b0;

NormalForm b2 = B15[3][1];
nnb_tst_assert_str(b2.singularityType, "(0,6),(4,0)",
                   "first bounded modality-two form stores the singularity type");
nnb_tst_assert_int(b2.milnorNumber, 15,
                   "first bounded modality-two form stores the Milnor number");
nnb_tst_assert_int(size(b2.parameters), 2,
                   "x^4+y^6 has two above-boundary regular-basis monomials");
nnb_tst_assert_str(string(b2.parameters[1]), "x^2*y^4",
                   "first above-boundary regular-basis monomial");
nnb_tst_assert_str(string(b2.parameters[2]), "x^2*y^3",
                   "second above-boundary regular-basis monomial");
nnb_tst_assert_str(string(b2.normalForm),
                   "x^4+(a(2))*x^2*y^3+(a(1))*x^2*y^4+y^6",
                   "bounded NormalForm stores the boundary plus Arnold regular basis");

tst_status("bounded-individual-mode");

///////////////////////////////////////////////////////////////////////////////
// Modality-series mode.  This checks symbolic k,l bounds and affine exponents.
///////////////////////////////////////////////////////////////////////////////

"enumnf.tst: modality-series mode";

list E2 = NNBenumerateRaw(-1, 1, 0);
nnb_tst_assert_int(size(E2), 2, "NNBenumerateRaw(-1,1,0) returns list(V,S)");
nnb_tst_assert_int(size(E2[1]), 56, "NNBenumerateRaw(-1,1,0) vertex count");
nnb_tst_assert_int(size(E2[2]), 21, "NNBenumerateRaw(-1,1,0) normal-form bucket count");

list G2 = NNBenumerate(1, 0);
nnb_tst_assert_int(size(G2), 2, "NNBenumerate(1,0) returns modalities 0..1");
nnb_tst_assert_int(size(G2[1]), 8, "m0=1 modality-zero grouped entries");
nnb_tst_assert_int(size(G2[2]), 13, "m0=1 modality-one grouped entries");

nnb_tst_assert(typeof(G2[1][1]) == "NNBNormalFormSeries", "first modality-zero entry is a series");
nnb_tst_assert_int(size(G2[1][1].normalForms), 1, "first modality-zero series has one representative");
nnb_tst_assert_int(size(NNBnormalFormSeriesParameters(G2[1][1])), 1,
                   "symbolic modality-zero series has one series parameter");
nnb_tst_assert_str(NNBnormalFormSeriesParameters(G2[1][1])[1], "l",
                   "symbolic modality-zero series parameter is l");
nnb_tst_assert_str(NNBnormalFormSeriesString(G2[1][1]),
                   "normal form series of modality 0; series parameters: l; normal forms: y^(2*l-1) + x^(2) with l > 1",
                   "NNBNormalFormSeries has a nice show string");
"enumnf.tst: NNBprintNormalFormSeries output";
NNBprintNormalFormSeries(G2[1][1]);

list Y0 = NNBtermsAsExponents(G2[1][1], 0, 2);
nnb_tst_assert_int(size(Y0), 2, "symbolic family has two terms");
nnb_tst_assert_str(Y0[1][1], "0", "symbolic family first term x exponent");
nnb_tst_assert_str(Y0[1][2], "3", "symbolic family first term y exponent after l=2");
nnb_tst_assert_str(Y0[2][1], "2", "symbolic family second term x exponent");
nnb_tst_assert_str(Y0[2][2], "0", "symbolic family second term y exponent");

poly pg = NNBevaluateNormalFormSeries(G2[1][1], 0, 2);
nnb_tst_assert_poly(pg, x^2 + y^3, "NNBevaluateNormalFormSeries after substituting l=2");
nnb_tst_assert_poly(NNBevaluateNormalFormSeries(G2[1][1], 0, 2), x^2 + y^3,
                    "NNBevaluateNormalFormSeries substitutes l into a symbolic series");
list hg = NNBhashFunction(G2[1][1], 0, 2);
nnb_tst_assert_int(hg[1], 6, "NNBhashFunction after substituting l=2");
nnb_tst_assert_int(hg[2], 2, "NNBhashFunction term-count after substituting l=2");
nnb_tst_assert_int(hg[3], 5, "NNBhashFunction coordinate-sum after substituting l=2");
list RBg = NNBregularBasis(G2[1][1], 0, 2);
nnb_tst_assert_int(size(RBg), 0, "NNBregularBasis after substituting l=2");
NormalForm g0nf = NNBevaluateNormalFormSeriesToNormalForm(G2[1][1], 0, 2);
nnb_tst_assert(typeof(g0nf) == "NormalForm",
               "symbolic series can be converted after exponent substitution");
nnb_tst_assert_str(g0nf.singularityType, "(0,3),(2,0)",
                   "converted symbolic modality-zero series stores the singularity type");
nnb_tst_assert_int(size(g0nf.parameters), 0,
                   "converted symbolic modality-zero series has no regular-basis parameters");

"enumnf.tst: exponent-parameter validation";
NNBevaluateNormalFormSeries(G2[1][1]);
NNBevaluateNormalFormSeries(G2[1][3],0);

tst_status("modality-series-mode");

///////////////////////////////////////////////////////////////////////////////
// Modality two.  This is a larger run and guards the modality-2 list shape.
///////////////////////////////////////////////////////////////////////////////

"enumnf.tst: modality-two series";

list E3 = NNBenumerateRaw(-1, 2, 0);
nnb_tst_assert_int(size(E3), 2, "NNBenumerateRaw(-1,2,0) returns list(V,S)");
nnb_tst_assert_int(size(E3[1]), 95, "NNBenumerateRaw(-1,2,0) vertex count");
nnb_tst_assert_int(size(E3[2]), 38, "NNBenumerateRaw(-1,2,0) normal-form bucket count");

list G3 = NNBenumerate(2, 0);
nnb_tst_assert_int(size(G3), 3, "NNBenumerate(2,0) returns modalities 0..2");
nnb_tst_assert_int(size(G3[1]), 8, "m0=2 modality-zero grouped entries");
nnb_tst_assert_int(size(G3[2]), 13, "m0=2 modality-one grouped entries");
nnb_tst_assert_int(size(G3[3]), 17, "m0=2 modality-two grouped entries");

nnb_tst_assert(typeof(G3[3][1]) == "NNBNormalFormSeries",
               "concrete modality-two entry is also a series");
nnb_tst_assert_int(size(NNBnormalFormSeriesParameters(G3[3][1])), 0,
                   "concrete modality-two series has no series parameter");
nnb_tst_assert_str(NNBnormalFormSeriesString(G3[3][1]),
                   "normal form series of modality 2; no series parameter; normal forms: y^(6) + x^(4)",
                   "concrete series show string names the absence of parameters");
nnb_tst_assert_poly(NNBevaluateNormalFormSeries(G3[3][1]), x^4 + y^6,
                    "NNBevaluateNormalFormSeries evaluates a concrete series");
NormalForm m2nf = NNBevaluateNormalFormSeriesToNormalForm(G3[3][1]);
nnb_tst_assert(typeof(m2nf) == "NormalForm",
               "concrete series can be converted to Arnold NormalForm");
nnb_tst_assert_str(m2nf.singularityType, "(0,6),(4,0)",
                   "converted concrete series stores the singularity type");
nnb_tst_assert_int(size(m2nf.parameters), 2,
                   "converted concrete series stores regular-basis parameters");
list X2 = NNBtermsAsExponents(G3[3][1]);
nnb_tst_assert_int(size(X2), 2, "first modality-two series has two terms");
nnb_tst_assert_str(X2[1][1], "0", "first modality-two first term x exponent");
nnb_tst_assert_str(X2[1][2], "6", "first modality-two first term y exponent");
nnb_tst_assert_str(X2[2][1], "4", "first modality-two second term x exponent");
nnb_tst_assert_str(X2[2][2], "0", "first modality-two second term y exponent");
list H2 = NNBhashFunction(G3[3][1]);
nnb_tst_assert_int(H2[1], 24, "first modality-two hash area component");
nnb_tst_assert_int(H2[2], 2, "first modality-two hash term-count component");
nnb_tst_assert_int(H2[3], 10, "first modality-two hash coordinate-sum component");
list RB2 = NNBregularBasis(G3[3][1]);
nnb_tst_assert_int(size(RB2), 2,
                   "NNBregularBasis uses arnold.lib for x^4+y^6");
nnb_tst_assert_str(string(RB2[1]), "2,4",
                   "first exponent vector from the Arnold regular basis");
nnb_tst_assert_str(string(RB2[2]), "2,3",
                   "second exponent vector from the Arnold regular basis");

nnb_tst_assert(typeof(G3[3][10]) == "NNBNormalFormSeries", "tenth modality-two entry is a series");
nnb_tst_assert_int(size(G3[3][10].normalForms), 1, "tenth modality-two series has one representative");
nnb_tst_assert_str(NNBnormalFormSeriesParameters(G3[3][10])[1], "l",
                   "tenth modality-two series parameter is l");
nnb_tst_assert_str(NNBnormalFormSeriesString(G3[3][10]),
                   "normal form series of modality 2; series parameters: l; normal forms: y^(2*l) + x^(2) * y^(3) + x^(3) with l > 9/2",
                   "symbolic modality-two series show string names parameter l");
list Y2 = NNBtermsAsExponents(G3[3][10], 0, 5);
nnb_tst_assert_int(size(Y2), 3, "modality-two symbolic series has three terms after substitution");
nnb_tst_assert_str(Y2[1][1], "0", "modality-two symbolic first term x exponent");
nnb_tst_assert_str(Y2[1][2], "10", "modality-two symbolic first term y exponent after l=5");
nnb_tst_assert_str(Y2[2][1], "2", "modality-two symbolic second term x exponent");
nnb_tst_assert_str(Y2[2][2], "3", "modality-two symbolic second term y exponent");
nnb_tst_assert_str(Y2[3][1], "3", "modality-two symbolic third term x exponent");
nnb_tst_assert_str(Y2[3][2], "0", "modality-two symbolic third term y exponent");
poly p2s = NNBevaluateNormalFormSeries(G3[3][10], 0, 5);
nnb_tst_assert_poly(p2s, x^3 + x^2*y^3 + y^10,
                    "NNBevaluateNormalFormSeries evaluates a modality-two symbolic series");
list H2s = NNBhashFunction(G3[3][10], 0, 5);
nnb_tst_assert_int(H2s[1], 29, "modality-two symbolic hash area component after l=5");
nnb_tst_assert_int(H2s[2], 3, "modality-two symbolic hash term-count component after l=5");
nnb_tst_assert_int(H2s[3], 18, "modality-two symbolic hash coordinate-sum component after l=5");
list RB2s = NNBregularBasis(G3[3][10], 0, 5);
nnb_tst_assert_int(size(RB2s), 2,
                   "NNBregularBasis computes the symbolic series basis after l=5");
nnb_tst_assert_str(string(RB2s[1]), "3,1",
                   "first symbolic exponent vector from the Arnold regular basis");
nnb_tst_assert_str(string(RB2s[2]), "2,3",
                   "second symbolic exponent vector from the Arnold regular basis");
list RB2p = NNBregularBasis(p2s);
nnb_tst_assert_int(size(RB2p), 2,
                   "NNBregularBasis on evaluated polynomial agrees in size");
nnb_tst_assert_str(string(RB2p[1]), "3,1",
                   "first polynomial exponent vector from the Arnold regular basis");
nnb_tst_assert_str(string(RB2p[2]), "2,3",
                   "second polynomial exponent vector from the Arnold regular basis");
NormalForm m2snf = NNBevaluateNormalFormSeriesToNormalForm(G3[3][10], 0, 5);
nnb_tst_assert(typeof(m2snf) == "NormalForm",
               "symbolic modality-two series converts to Arnold NormalForm after l=5");
nnb_tst_assert_str(m2snf.singularityType, "(0,10),(2,3),(3,0)",
                   "converted symbolic modality-two series stores the singularity type");
nnb_tst_assert_int(m2snf.milnorNumber, 17,
                   "converted symbolic modality-two series stores the Milnor number");
nnb_tst_assert_int(size(m2snf.parameters), 2,
                   "converted symbolic modality-two series stores regular-basis parameters");

tst_status("modality-two-series");

"enumnf.tst: all tests passed";
kill R;
tst_status(1);$
