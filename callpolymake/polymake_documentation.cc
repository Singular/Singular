#include <polymake_conversion.h>

#include <callgfanlib/bbcone.h>
#include <callgfanlib/bbfan.h>
#include <callgfanlib/bbpolytope.h>

#include <Singular/blackbox.h>
#include <Singular/ipshell.h>
#include <Singular/subexpr.h>
#include <Singular/ipid.h>

void init_polymake_help()
{

  std::string polymake_banner = 
    "Welcome to polymake\nCopyright (c) 1997-2012\nEwgenij Gawrilow, Michael Joswig (TU Darmstadt)\nhttp://www.polymake.org\n";

  Print("%s",omStrDup(polymake_banner.c_str()));

  std::string polymake_help = 
    "SHARED LIBRARY: polymake.so  Interface to polymake (http://www.polymake.org)\nAUTHORS: Janko Boehm, boehm@mathematik.uni-kl.de\n         Yue Ren,     ren@mathematik.uni-kl.de\n\nOVERVIEW:\nPolymake is a tool to study the combinatorics \nand the geometry of convex polytopes and polyhedra. \nIt is also capable of dealing with simplicial complexes, \nmatroids, polyhedral fans, graphs, tropical objects.\nThe interface relies on the callable library functionality,\nby Ewgenij Gawrilow.\n\nREFERENCES:\nEwgenij Gawrilow and Michael Joswig. polymake: a framework for analyzing convex polytopes. \nPolytopes—combinatorics and computation (Oberwolfach, 1997), 43–73, DMV Sem., 29, Birkhäuser, Basel, 2000. MR1785292 (2001f:52033)\n\nPROCEDURES:\n  boundaryLatticePoints(polytope p);\n  ehrhartPolynomialCoeff(polytope p);\n  facetVertexLatticeDistances(polytope p);\n  facetWidth(polytope p);\n  facetWidths(polytope p);\n  fVector(polytope p);\n  gorensteinIndex(polytope p);\n  gorensteinVector(polytope p);\n  hilbertBasis(cone c);\n  hStarVector(polytope p);\n  hVector(polytope p);\n  interiorLatticePoints(polytope p);\n  isBounded(polytope p);\n  isCanonical(polytope p);\n  isCompressed(polytope p);\n  isGorenstein(polytope p);\n  isLatticeEmpty(polytope p);\n  isNormal(polytope p);\n  isReflexive(polytope p);\n  isSmooth(polytope p);\n  isVeryAmple(polytope p);\n  latticeCodegree(polytope p);\n  latticeDegree(polytope p);\n  latticePoints(polytope p);\n  latticeVolume(polytope p);\n  maximalFace(polytope p, intvec v);\n  maximalValue(polytope p, intvec v);\n  minimalFace(polytope p, intvec v);\n  minimalValue(polytope p, intvec v);\n  minkowskiSum(polytope p, polytope q);\n  nBoundaryLatticePoints(polytope p);\n  nHilbertBasis(cone c);\n";

  module_help_main("polymake.so",omStrDup(polymake_help.c_str()));


  std::string isReflexive_help = 
    "USAGE:    isReflexive(polytope p)\nRETURN:   int, 1 if p is reflexive and 0 otherwise\nKEYWORDS: polytopes; polymake; reflexive\nEXAMPLE:  example isReflexive shows an example\nexample\n{ \"EXAMPLE: \";\nintmat M[4][4]=1,1,0,0, 1,0,1,0, 1,0,0,1, 1,-1,-1,-1;\npolytope p = polytopeViaVertices(M);\nPolymake::isReflexive(p);\nintmat N[4][4]=1,2,0,0, 1,0,2,0, 1,0,0,2, 1,-2,-2,-2;\nq = polytopeViaVertices(N);\nPolymake::isReflexive(q);\n}\n";

 module_help_proc("polymake.so","isReflexive", omStrDup(isReflexive_help.c_str()));

  std::string isBounded_help = 
    "USAGE:    isBounded(polytope p)\nRETURN:   int, 1 if p is bounded, 0 otherwise.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example isBounded shows an example\n";

 module_help_proc("polymake.so","isBounded", omStrDup(isBounded_help.c_str()));

  std::string isGorenstein_help = 
    "USAGE:    isGorenstein(polytope p)\nRETURN:   int, 1 if p is gorenstein (i.e. reflexive after dilatation and translation), 0 otherwise.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example isGorenstein shows an example\n";

 module_help_proc("polymake.so","isGorenstein", omStrDup(isGorenstein_help.c_str()));

  std::string gorensteinIndex_help = 
    "USAGE:    gorensteinIndex(polytope p)\nRETURN:   int, n if p is reflexive after dilatation by n and translation, 0 otherwise.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example gorensteinIndex shows an example\n";

 module_help_proc("polymake.so","gorensteinIndex", omStrDup(gorensteinIndex_help.c_str()));

  std::string gorensteinVector_help = 
    "USAGE:    gorensteinVector(polytope p)\nRETURN:   intvec, v if p is reflexive after dilatation and translation by v, 0 otherwise.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example gorensteinVector shows an example\n";

 module_help_proc("polymake.so","gorensteinVector", omStrDup(gorensteinVector_help.c_str()));

  std::string isCanonical_help = 
    "USAGE:    isCanonical(polytope p)\nRETURN:   intvec, 1 if p has exactly one interior lattice point, 0 otherwise.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example isCanonical shows an example\n";

 module_help_proc("polymake.so","isCanonical", omStrDup(isCanonical_help.c_str()));

  std::string isTerminal_help = 
    "USAGE:    isLatticeEmpty(polytope p)\nRETURN:   int, 1 if p contains no lattice points other than the vertices, 0 otherwise.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example isLatticeEmpty shows an example\n";

 module_help_proc("polymake.so","isTerminal", omStrDup(isTerminal_help.c_str()));

  std::string latticeVolume_help = 
    "USAGE:    latticeVolume(polytope p)\nRETURN:   int, the normalized lattice volume of p, that is, (dim(P))! times the volume of P.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example latticeVolume shows an example\n";

 module_help_proc("polymake.so","latticeVolume", omStrDup(latticeVolume_help.c_str()));

  std::string latticeDegree_help = 
    "USAGE:    latticeDegree(polytope p)\nRETURN:   int, the lattice degree of p, i.e. degree of the Ehrhart polynomial of P.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example latticeDegree shows an example\n";

 module_help_proc("polymake.so","latticeDegree", omStrDup(latticeDegree_help.c_str()));

  std::string latticeCodegree_help = 
    "USAGE:    latticeCodegree(polytope p)\nRETURN:   int, getDimension(p)+1-latticeDegree(p), which is the smallest number k such that k*p has an interior latt\\nice point.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example latticeCodegree shows an example\n";

 module_help_proc("polymake.so","latticeCodegree", omStrDup(latticeCodegree_help.c_str()));

  std::string ehrhartPolynomialCoeff_help = 
    "USAGE:    ehrhartPolynomialCoeff(polytope p)\nRETURN:   intvec, coefficients of the Ehrhart polynomial of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example ehrhartPolynomialCoeff shows an example\n";

 module_help_proc("polymake.so","ehrhartPolynomialCoeff", omStrDup(ehrhartPolynomialCoeff_help.c_str()));

  std::string hStarVector_help = 
    "USAGE:    hStarVector(polytope p)\nRETURN:   intvec, h*-vector of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example hStarVector shows an example\n";

 module_help_proc("polymake.so","hStarVector", omStrDup(hStarVector_help.c_str()));

  std::string hVector_help = 
    "USAGE:    hVector(polytope p)\nRETURN:   intvec, h-vector of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example hVector shows an example\n";

 module_help_proc("polymake.so","hVector", omStrDup(hVector_help.c_str()));

  std::string fVector_help = 
    "USAGE:    fVector(polytope p)\nRETURN:   intvec, the f-vector of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example fVector shows an example\n";

 module_help_proc("polymake.so","fVector", omStrDup(fVector_help.c_str()));

  std::string isNormal_help = 
    "USAGE:    isNormal(polytope p)\nRETURN:   int, 1 if p is normal, i.e. the projective toric variety defined by p is projectively normal, 0 otherwise.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example isNormal shows an example\n";

 module_help_proc("polymake.so","isNormal", omStrDup(isNormal_help.c_str()));

  std::string facetWidths_help = 
    "USAGE:    facetWidths(polytope p)\nRETURN:   intvec, vector with the integral widths of p with respect to all facet normals.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example facetWidths shows an example\n";

 module_help_proc("polymake.so","facetWidths", omStrDup(facetWidths_help.c_str()));

  std::string facetWidth_help = 
    "USAGE:    facetWidth(polytope p)\nRETURN:   int, maximum of the integral widths of p over all facet normals.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example facetWidth shows an example\n";

 module_help_proc("polymake.so","facetWidth", omStrDup(facetWidth_help.c_str()));

  std::string facetVertexLatticeDistances_help = 
    "USAGE:    facetVertexLatticeDistances(polytope p)\nRETURN:   intmat, matrix of lattice distances between vertices (columns) and facets (rows).\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example facetVertexLatticeDistances shows an example\n";

 module_help_proc("polymake.so","facetVertexLatticeDistances", omStrDup(facetVertexLatticeDistances_help.c_str()));

  std::string isCompressed_help = 
    "USAGE:    isCompressed(polytope p)\nRETURN:   int, 1 if facetWidth(p)=1, 0 otherwise.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example isCompressed shows an example\n";

 module_help_proc("polymake.so","isCompressed", omStrDup(isCompressed_help.c_str()));

  std::string isSmooth_help = 
    "USAGE:    isSmooth(polytope p)\n          isSmooth(cone c)\n          isSmooth(fan F)\nRETURN:   int, 1 if p, c, or F is smooth, 0 otherwise.\nKEYWORDS: polytopes; cones; fans; polymake;\nEXAMPLE:  example isSmooth shows an example\n";

 module_help_proc("polymake.so","isSmooth", omStrDup(isSmooth_help.c_str()));

  std::string isVeryAmple_help = 
    "USAGE:    isVeryAmple(polytope p)\nRETURN:   int, 1 if p is very ample, 0 otherwise.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example isVeryAmple shows an example\n";

 module_help_proc("polymake.so","isVeryAmple", omStrDup(isVeryAmple_help.c_str()));

  std::string latticePoints_help = 
    "USAGE:    latticePoints(polytope p)\nRETURN:   intmat, matrix whose rows are the lattice points of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example latticePoints shows an example\n";

 module_help_proc("polymake.so","latticePoints", omStrDup(latticePoints_help.c_str()));

  std::string nLatticePoints_help = 
    "USAGE:    nLatticePoints(polytope p)\nRETURN:   int, number of lattice points of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example nLatticePoints shows an example\n";

 module_help_proc("polymake.so","nLatticePoints", omStrDup(nLatticePoints_help.c_str()));

  std::string interiorLatticePoints_help = 
    "USAGE:    interiorLatticePoints(polytope p)\nRETURN:   intmat, an matrix whose rows are the lattice points in the relative interior of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example interiorLatticePoints shows an example\n";

 module_help_proc("polymake.so","interiorLatticePoints", omStrDup(interiorLatticePoints_help.c_str()));

  std::string nInteriorLatticePoints_help = 
    "USAGE:    nInteriorLatticePoints(polytope p)\nRETURN:   int, number of lattice points in the relative interior of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example nInteriorLatticePoints shows an example\n";

 module_help_proc("polymake.so","nInteriorLatticePoints", omStrDup(nInteriorLatticePoints_help.c_str()));

  std::string boundaryLatticePoints_help = 
    "USAGE:    boundaryLatticePoints(polytope p)\nRETURN:   intmat, matrix whose rows are the lattice points in the relative boundary of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example boundaryLatticePoints shows an example\n";

 module_help_proc("polymake.so","boundaryLatticePoints", omStrDup(boundaryLatticePoints_help.c_str()));

  std::string nBoundaryLatticePoints_help = 
    "USAGE:    nBoundaryLatticePoints(polytope p)\nRETURN:   int, number of lattice points in the relative boundary of p.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example nBoundaryLatticePoints shows an example\n";

 module_help_proc("polymake.so","nBoundaryLatticePoints", omStrDup(nBoundaryLatticePoints_help.c_str()));

  std::string hilbertBasis_help = 
    "USAGE:    hilbertBasis(cone c)\nRETURN:   intmat, Hilbert basis of the semigroup of c.\nKEYWORDS: cones; polymake;\nEXAMPLE:  example hilbertBasis shows an example\n";

 module_help_proc("polymake.so","hilbertBasis", omStrDup(hilbertBasis_help.c_str()));

  std::string nHilbertBasis_help = 
    "USAGE:    nHilbertBasis(cone c)\nRETURN:   int, size of the Hilbert basis of the semigroup of c.\nKEYWORDS: cones; polymake;\nEXAMPLE:  example nHilbertBasis shows an example\n";

 module_help_proc("polymake.so","nHilbertBasis", omStrDup(nHilbertBasis_help.c_str()));

  std::string minkowskiSum_help = 
    "USAGE:    minkowskiSum(polytope p, polytope q)\nRETURN:   polytope, Minkowski sum of p and q.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example minkowskiSum shows an example\n";

 module_help_proc("polymake.so","minkowskiSum", omStrDup(minkowskiSum_help.c_str()));

  std::string minimalValue_help = 
    "USAGE:    minimalValue(polytope p, intvec v)\nRETURN:   int, the minimal value of the linear form v on p.\n          The first coordinate of v corresponds to a shift of the\n          minimal value since p is considered as a polytope\n          in the plane (first coordinate) = 1.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example minimalValue shows an example\n";

 module_help_proc("polymake.so","minimalValue", omStrDup(minimalValue_help.c_str()));

  std::string maximalValue_help = 
    "USAGE:    maximalValue(polytope p, intvec v)\nRETURN:   int, maximal value of the linear form v on p.\n          The first coordinate of v corresponds to a shift of the\n          maximal value since p is considered as a polytope\n          in the plane (first coordinate) = 1.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example maximalValue shows an example\n";

 module_help_proc("polymake.so","maximalValue", omStrDup(maximalValue_help.c_str()));

  std::string minimalFace_help = 
    "USAGE:    minimalFace(polytope p, intvec v)\nRETURN:   intmat, vertices of the face of p on which the linear form v\n          is minimal.\n          The first coordinate of v corresponds to a shift of the\n          minimal value since p is considered as a polytope\n          in the plane (first coordinate) = 1. Hence\n          the minimal face is independent of the first coordinate of v.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example minimalFace shows an example\n";

 module_help_proc("polymake.so","minimalFace", omStrDup(minimalFace_help.c_str()));

  std::string maximalFace_help = 
    "USAGE:    maximalFace(polytope p, intvec v)\nRETURN:   intmat, vertices of the face of p on which the linear form v\n          is maximal.\n          The first coordinate of v corresponds to a shift of the\n          maximal value since p is considered as a polytope\n          in the plane (first coordinate) = 1. Hence\n          the maximal face is independent of the first coordinate of v.\nKEYWORDS: polytopes; polymake;\nEXAMPLE:  example maximalFace shows an example\n";

 module_help_proc("polymake.so","maximalFace", omStrDup(maximalFace_help.c_str()));

  std::string visual_help = 
    "USAGE:    visual(polytope p)\n          visual(fan F)\nRETURN:   none, draws the polytope p or fan F using jreality.\nKEYWORDS: polytopes; polymake; visualization;\nEXAMPLE:  example visual shows an example\n";

 module_help_proc("polymake.so","visual", omStrDup(visual_help.c_str()));

  std::string normalFan_help =
    "USAGE:    normalFan(polytope p)\nRETURN:   fan,\nKEYWORDS: polytopes; polymake; visualization;\nEXAMPLE:  example visual shows an example\n";

    module_help_proc("polymake.so","normalFan", omStrDup(normalFan_help.c_str()));

}
