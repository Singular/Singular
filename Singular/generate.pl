#!/usr/local/bin/perl
###########################################################################
# $Id: generate.pl,v 1.3 1999-05-26 16:20:19 obachman Exp $

###########################################################################
##
## FILE: generate.pl 
## PURPOSE: Generates Proc's and GetProc's as specified by templates and
## variable @input. Writes result to stdout, warning/errors to stderr.
## AUTHOR: obachman (3/98)
##

###########################################################################
##
## Supporting procedures
##

sub Warning
{
  print STDERR $_[0], "\n";
}


# Each occureence of CALL_<fn>(...) in $string is replaced by 
# `eval fn(..., $argv)'. Modified string is returned. 
sub Expand
{
  local($string, $argv) = @_;
  local(@lines, @call, $call, $prefix, $postfix, $whitespaces, $result);
  local($i, $j);

  @lines = split(/\n/, $_[0]);
  for ($i = 0; $i <= $#lines; $i++)
  {
    if ($lines[$i] =~ /CALL_/)
    {
      ($whitespaces, $prefix, $call, $postfix) = &ParseLine($lines[$i]);
      #insert $argv as last argument to call
      if ($call =~ /\(\s*\)$/)
      {
	$call =~ s/\(\s*\)$/\(\$argv\)/;
      }
      else
      {
	$call =~ s/\)$/\,\$argv\)/;
      }
      $call = "& ".$call.";";
      $call = eval $call;
      @call = split(/\n/,"$whitespaces$prefix$call$postfix");
      for ($j=1; $j <= $#call; $j++)
      {
	$call[$j] = $whitespaces.$call[$j];
      }
      $lines[$i] = join("\n", @call);
    }
  }
  $result = join("\n", @lines);
  if ($call) 
  {
    return (&Expand($result));
  }
  else
  {
    return $result;
  }
}

# takes a line containing CALL_ apart into and returns 
# ($whitespace, $prefix, $call, $postfix)
sub ParseLine
{
  local($line) = @_;
  local($i, $c_start, $c_length, $bcount);
  local($whitespace, $prefix, $call, $postfix);
  
  while(substr($line, $i, 1) =~ /\s/ && $i <= length($line))
  {
    $whitespace = $whitespace.substr($line, $i, 1);
    $i++;
  }
  
  while(substr($line, $i) !~ /^CALL_/  && $i <= length($line))
  {
    $prefix = $prefix.substr($line, $i, 1);
    $i++;
  }
  
  $i = $i+5;
  $c_start = $i;
  while(substr($line, $i, 1) ne "(" && $i <= length($line))
  {
    $i++;
  }
  if (substr($line, $i, 1) eq "(")
  {
    $bcount = 1;
    $i++;
    while ($bcount > 0 && $i <= length($line))
    {
      if (substr($line, $i, 1) eq ")")
      {
	$bcount--;
      }
      elsif (substr($line, $i, 1) eq "(")
      {
	$bcount++;
      }
      elsif (substr($line, $i, 1) eq "\"")
      {
	$i++;
	while ((substr($line, $i, 1) ne "\"" || 
		substr($line, $i-1, 1) eq "\\") && 
	       $i <= length($line))
	{
	  $i++;
	}
      }
      $i++;
    }
  }
  $call = substr($line, $c_start, $i - $c_start);
  $postfix = substr($line, $i);
  return ($whitespace, $prefix, $call, $postfix);
}


###########################################################################
##
## Specification of the properties which determine a Proc
##
## Properties need to have following syntax: propVAL, where prop is
## short "prefix" name of properties, and needs to be in lower caps, and
## VAL is possible value and needs to be in all upper caps.
##
## Furthermore, observe the following conventions
## @Protperty = ("propGEN", "propVAL1", ...)
## $EnumType{prop} = Property
## GetProperty($string) should always return a valid value.

## need to hard-wire properties here, because they are needed in the interface to surrounding C++-code:

@Properties = ("ch", "ot", "hom", "nw");
@Characteristics = ("chGEN", "chMODP");
$EnumType{"ch"} = "Characteristics";
sub GetCharacteristic
{
  foreach $element (split('_', $_[0]))
  {
    return ($element) if ($element =~ /^ch/ &&  
			  grep(/$element/, @Characteristics));
  }
  return $Characteristics[0];
}
  
@OrderingTypes = ("otGEN", "otEXP", "otCOMPEXP", "otEXPCOMP", "otSYZDPC");
$EnumType{"ot"} = "OrderingTypes";
sub GetOrderingType
{
  foreach $element (split('_', $_[0]))
  {
    return ($element) if ($element =~ /^ot/ && 
			 grep(/$element/, @OrderingTypes));
  }
  return $OrderingTypes[0];
}

@Homogs = ("homGEN", "homYES");  
$EnumType{"hom"} = "Homogs";
sub GetHomog
{
  foreach $element (split('_', $_[0]))
  {
    return ($element) if ($element =~ /^hom/ &&
			  grep(/$element/, @Homogs));
  }
  return $Homogs[0];
}

@NumWords = ("nwGEN", "nwONE", "nwTWO", "nwEVEN", "nwODD");
$EnumType{"nw"} = "NumWords";
sub GetNumWords
{
  foreach $element (split('_', $_[0]))
  {
    return ($element) if ($element =~ /^nw/ &&
			  grep(/$element/, @NumWords));
  }
  return $NumWords[0];
}

# given a list of "short" prefix properties, generate enumeration type
# specification for each property
sub Generate_EnumTypes
{
  local(@evalues, $source);
  
  foreach $key (@_)
  {
    $etype = $EnumType{$key};
    if ($etype =~ /\w+/)
    {
      @evalues = eval '@'.$etype;
      if ($#evalues >= 0)
      {
	$source = $source."typedef enum $etype {". $evalues[0] . " = 0";
	shift @evalues;
	foreach $evalue (@evalues)
	{
	  $source = $source.", $evalue";
	}
	$source = $source."} $etype;\n";
      }
      else
      {
	& Warning("No enum values for type: $etype");
      }
    }
    else
    {
      & Warning("Unknown enumeration type index: $key");
    }
  }
  return $source;
}


###########################################################################
##
## "Macros" needed in expansion of Procs
##
sub NCOPYNEG
{
  local($number, $argv) = @_;
  
  return "npNegM($number)" if (& GetCharacteristic($argv) eq "chMODP");
  return "nNeg(nCopy($number))";
}

sub NNEG
{
  local($number, $argv) = @_;

  return "npNegM($number)" if (& GetCharacteristic($argv) eq "chMODP");
  return "nNeg($number)";
}


sub NDELETE
{
  local($number, $argv) = @_;
  
  return "" if (& GetCharacteristic($argv) eq "chMODP");
  return "nDelete($number)";
}

sub NMULT
{
  local($m1, $m2, $argv) = @_;
  
  return "npMultM($m1, $m2)" if (& GetCharacteristic($argv) eq "chMODP");
  return "nMult($m1, $m2)";
}

sub NADD
{
  local($m1, $m2, $argv) = @_;
  
  return "npAddM($m1, $m2)" if (& GetCharacteristic($argv) eq "chMODP");
  return "nAdda($m1, $m2)";
}

sub NSUB
{
  local($m1, $m2, $argv) = @_;
  
  return "npSubM($m1, $m2)" if (& GetCharacteristic($argv) eq "chMODP");
  return "nSub($m1, $m2)";
}

sub NEQUAL
{
  local($m1, $m2, $argv) = @_;
  
  return "npEqualM($m1, $m2)" if (& GetCharacteristic($argv) eq "chMODP");
  return "nEqual($m1, $m2)";
} 

sub NISZERO
{
  local($m1, $argv) = @_;
  
  return "npIsZeroM($m1)" if (& GetCharacteristic($argv) eq "chMODP");
  return "nIsZero($m1)";
}

sub MULTCOPYX
{
  local($p, $monom, $n, $exp, $spNoether, $argv) = @_;
  
  return "spMultCopyX($p, $monom, $n, $exp, $spNoether)" 
    if (& GetCharacteristic($argv) eq "chMODP");
  return "spGMultCopyX($p, $monom, $n, $exp, $spNoether)";
}

sub INITORDER
{
  local($order, $p, $argv) = @_;
  
  return "$order = $p->Order" if (&GetHomog($argv) eq "homYES");
  return "";
}

  
sub PCOPYADDFAST
{
  local($p1, $p2, $p3, $order, $argv) = @_;
  
  return "pCopyAddFastHomog($p1, $p2, $p3, $order)" 
    if (&GetHomog($argv) eq "homYES");
  return "pCopyAddFast0($p1, $p2, $p3)";
}

sub PDELETE1
{
  local($p, $argv) = @_;
  return ""
    if (& GetCharacteristic($argv) eq "chMODP");
  return "pDelete1(&$p)";
}

sub ORDERINIT
{
  local($p1, $p2, $argv) = @_;
  return "int _new_order = pGetOrder($p1) + pGetOrder($p2)"
    if (& GetHomog($argv) eq "homYES");
  return "";
}

sub ORDERADD
{
  local($p1, $p2, $p3, $argv) = @_;
  return "$p1->Order = _new_order" 
    if (& GetHomog($argv) eq "homYES");
  return "$p1->Order = $p2->Order + $p3->Order"
}

sub EXPADD
{
  local($p1, $p2, $p3, $argv) = @_;
  local($nw) = & GetNumWords($argv);
  
  return "memadd_".$nw."((unsigned long*) &($p1->exp[0]),(const unsigned long*) &($p2->exp[0]),(const unsigned long*) &($p3->exp[0]))" 
    if ($nw eq "nwONE" || $nw eq "nwTWO");
  
  return "memadd_".$nw."((unsigned long*) &($p1->exp[0]),(const unsigned long*) &($p2->exp[0]),(const unsigned long*) &($p3->exp[0]), pVariables1W)" 
    if ($nw eq "nwODD" || $nw eq "nwEVEN");
  
  return "memadd((unsigned long*) &($p1->exp[0]),(const unsigned long*) &($p2->exp[0]),(const unsigned long*) &($p3->exp[0]), pVariables1W)";
}

###########################################################################
##
## COMPARE "macro": Besides generating th source code which
## accomplishes monomial comparisons, it also generates the (global)
## string $rargv charcaterising the generated Proc
##
sub COMPARE
{
  local($p1, $p2, $d, $argv) = @_;
  local($ot, $hom, $nw, $res);
  
  $rargv = &GetCharacteristic($argv);
  $ot = &GetOrderingType($argv);
  $rargv =  $rargv."_".$ot;
  if ($ot eq "otCOMPEXP" ||
      $ot eq "otEXPCOMP" ||
      $ot eq "otEXP")
  {
    if ($ot eq "otCOMPEXP")
    {
      $res = "$d = pGetComp($p2) - pGetComp($p1);\n";
      $res = $res."NonZeroTestA($d, pComponentOrder, goto NotEqual);\n";
      $ot = "otEXP";
    }
    $hom = &GetHomog($argv);
    $rargv =  $rargv."_".$hom;
    if ($hom ne "homYES")
    {
      $res = $res."$d = pGetOrder($p1) - pGetOrder($p2);\n";
      $res = $res."NonZeroTestA($d, pOrdSgn, goto NotEqual);\n";
    }
#     $rargv =  $rargv."_nwGEN";
#     $res = join("_", $res, pMonComp, $ot, "nwGEN");
#     $res = $res."($p1, $p2, pVariables1W, $d, NonZeroA($d, pLexSgn, goto NotEqual ), goto Equal);";
     $nw = &GetNumWords($argv);
     $rargv =  $rargv."_".$nw;
     $res = join("_", $res, pMonComp, $ot, $nw);
     if ($nw eq "nwONE" || $nw eq "nwTWO")
     {
       $res = $res."($p1, $p2, $d, NonZeroA($d, pLexSgn, goto NotEqual ), goto Equal);" 
     }
     else
     {
       $res = $res."($p1, $p2, pVariables1W, $d, NonZeroA($d, pLexSgn, goto NotEqual ), goto Equal);";
     }
    return $res;
  }
  else
  {
    if ($ot eq "otSYZDPC")
    {
      $res = "if (($d = pGetOrder($p1) - pGetOrder($p2))) goto NotEqual;\n";
      $nw = &GetNumWords($argv);
      $hom = &GetHomog($argv);
      $rargv =  $rargv."_".$hom;
      $rargv =  $rargv."_".$nw;
      $res = join("_", $res, pMonComp, $ot, $nw);
      if ($nw eq "nwONE" || $nw eq "nwTWO")
      {
	$res = $res."($p1, $p2, $d, goto NotEqual, goto Equal);";
      }
      else
      {
	$res = $res."($p1, $p2, pVariables1W, $d, goto NotEqual, goto Equal);";
      }
      return $res;
    }
    # general ordering type
    return ("if (($d = pComp0($p1, $p2))) goto NotEqual; else goto Equal;");
  }
}


###########################################################################
##
## Generates a Proc from a given "$argv" input string,
## i.e. property string
## 
## Returns spSpolyLoop and sets global associative array
## $loops{$rargv} to generated loop.
##

sub Generate_CurrProc
{
  local($argv)= @_;
  local($loopbody);

  undef $rargv if (defined($rargv));
  
  $loopbody = &Expand($CurrProcTemplate, $argv);

  $rargv = $argv if (! defined($rargv));
    
  & Warning("Can not realize $argv: Changed to $rargv") if ($argv ne $rargv);

  if (grep(/$rargv/, keys(%loops)))
  {
    & Warning("Duplicate entry $rargv: Ignoring $argv");
    return;
  }
  else
  {
    $loops{$rargv} = "static ".$CurrProcReturnType." ".$CurrProc."_$rargv\n$loopbody\n";
    return ($loops{$rargv});
  }
}

#gnerates Procs from array of $argv strings
sub Generate_CurrProcs
{
  local(%loops);
  
  foreach $argv (@_)
  {
    & Generate_CurrProc($argv);
  }
  return (%loops);
}

###########################################################################
##
## Generates array of $argv strings from a given input string by
## "orthoganization", i.e. komma-separated entries in input string are
## replaced by two argv strings containing the respective entrie.
##

sub FlattenInputString
{
  local($str) = @_;
  
  if ($str =~ /_/)
  {
    local(@parts, $head, @subresults, @result);
    @parts = split(/_/, $str);
    $head = shift(@parts);
    @subresults = &FlattenInputString(join("_", @parts));
    foreach $part (split(/,/, $head))
    {
      foreach $subresult (@subresults)
      {
	@result = (@result, $part."_".$subresult);
      }
    }
    return (@result);
  }
  return (split(/,/, $str));
}

# generate array of $argv strings from array of input strings
sub FlattenInput
{
  local(@result);
  foreach $entry (@_)
  {
    $entry =~ s/\s//;
    @result = (@result, & FlattenInputString($entry));
  }
  return @result;
}


###########################################################################
##
## GetProc routines
## They all work on valid $argv strings, only.
##

## Given a $prefix and an array of $argv's, return all the "next"
## values, i.e. strings deliminted by $prefix and next '_'.
sub GetNextKeys
{
  local($prefix, @values, $value);
  
  $prefix = shift(@_);
  $prefix = $prefix."_" unless ($prefix eq "");
  foreach $element (@_)
  {
    if ($prefix eq "")
    {
      ($value = $element) =~ s/^([a-zA-Z]*)(\w*)/\1/;
      @values = ($value, @values) unless grep(/^$value$/, @values);
    }
    elsif ($element =~ /^$prefix/)
    {
      ($value = $element) =~ s/^($prefix)([a-zA-Z]*)(\w*)/\2/;
      @values = ($value, @values) unless grep(/^$value$/, @values);
    }
  }
  return @values;
}

# recursively generates "Get$CurrProc" by working through all $argv's
# which math $prefix
# Sets $checks{$prefix}, if $prefix eq $argv
sub GenerateBody_GetCurrProc
{
  local($prefix, $nextprefix, $source, $newsource, $prop, $curr_prop, $gen_key);
  
  $prefix = shift(@_);
  
  #check for exact match -- then we are done
  if (grep(/^$prefix$/, @_))
  {
    $checks{$prefix} = "return ".$CurrProc."_$prefix;\n";
    return ($checks{$prefix});
  }
  
  foreach $key (& GetNextKeys($prefix, @_))
  {
    if ($key =~ /\w+/)
    {
      # get prop,
      ($prop = $key) =~ s/([a-z]*)([A-Z]*)/\1/;
      #check prop against exiting ones
      if ($curr_prop)
      {
	if ($prop ne $curr_prop)
	{
	  & Warning("Different props at same level: $prop : $curr_prop");
	  next;
	}
      }
      else
      {
	$curr_prop = $prop;
      }
      # delay handling of "GEN" field
      if ($key =~ /GEN/)
      {
	$gen_key = $key;
	next;
      }
      # let's work recursively
      if ($prefix eq "")
      {
	$nextprefix = $key;
      }
      else
      {
	$nextprefix = $prefix."_".$key;
      }
      $newsource=&GenerateBody_GetCurrProc($nextprefix,grep(/$nextprefix/,@_));
      if ($newsource)
      {
	$source = $source."if ($prop == $key)\n{\n".$newsource."}\n";
      }
    }
  }
  
  # take care of general key, if it exists
  if ($gen_key)
  {
    $gen_key  = $prefix."_".$gen_key unless ($prefix eq "");
    $source = $source . &GenerateBody_GetCurrProc($gen_key,grep(/$gen_key/,@_));
  }
  return ($source);
}


sub Generate_GetCurrProc
{
  local($header);
  
  $header = "static ".$CurrProc."_Proc Get".$CurrProc."(";
  
  foreach $key (@Properties)
  {
    $etype = $EnumType{$key};
    if ($etype =~ /\w+/)
    {
      $header = $header."$etype $key,";
    }
  }
  chop($header);
  
  return ($header. 
	  ")\n{\n" . 
	  &GenerateBody_GetCurrProc("", @_). 
	  "return NULL;\n}\n");
}

###########################################################################
##
## Input Specification
##

##
## kb_n_Mult_p
##

$input{"kb_n_Mult_p"} = ("chMODP_otGEN_homGEN_nwGEN");

$returnType{"kb_n_Mult_p"} = "void";

$template{"kb_n_Mult_p"} = <<_EOT_
(number n, poly p)
{
  while (p != NULL)
  {
    number nc = pGetCoeff(p);
    pSetCoeff0(p, CALL_NMULT("n", "nc"));
    CALL_NDELETE("&nc");
    pIter(p);
  }
}
_EOT_
  ;

##
## kb_p_Add_q
##

$input{"kb_p_Add_q"}
  = (
 "chMODP".
 "_otEXP,otCOMPEXP,otEXPCOMP".
"_homGEN,homYES".
"_nwONE,nwTWO,nwEVEN,nwODD".
":".
      "chMODP".
      "_otSYZDPC".
      "_homGEN".
      "_nwONE,nwTWO,nwEVEN,nwODD"
    );

## Template of kb_p_Add_q
$returnType{"kb_p_Add_q"} = "void";

$template{"kb_p_Add_q"} = <<_EOT_
(poly *p, int *lp,
 poly *q, int *lq, 
 memHeap heap)
{
#ifdef KB_USE_HEAPS
  assume(heap != NULL);
#else
  assume(heap == NULL);
#endif
  assume(pLength(*p) == *lp && pLength(*q) == *lq);
  
  number t, n1, n2;
  unsigned int l = *lp + *lq;
  spolyrec rp;
  poly a = &rp, a1 = *p, a2 = *q;

  if (a2 == NULL) return;

  *q = NULL;
  *lq = 0;
  
  if (a1 == NULL) 
  {
    *p = a2;
    *lp = l;
    return;
  }

  Top:     // compare a1 and a2 w.r.t. monomial ordering
  register long d;
  CALL_COMPARE("a1", "a2", "d");
  
  Equal:
  assume(pComp0(a1, a2) == 0);

  n1 = pGetCoeff(a1);
  n2 = pGetCoeff(a2);
  t = CALL_NADD("n1", "n2");
  CALL_NDELETE("&n1");
  CALL_NDELETE("&n2");
  kb_pFree1AndAdvance(a2, heap);
  
  if (CALL_NISZERO("t"))
  {
    l -= 2;
    CALL_NDELETE("&t");
    kb_pFree1AndAdvance(a1, heap);
  }
  else
  {
    l--;
    pSetCoeff0(a1,t);
    a = pNext(a) = a1;
    pIter(a1);
  }
  if (a1==NULL)
  {
    pNext(a) = a2;
    goto Finish;
  }
  else if (a2==NULL)
  {
    pNext(a) = a1;
    goto Finish;
  }
  goto Top;
     
  NotEqual:
  if (d < 0)
  {
    assume(pComp0(a1, a2) == -1);
    a = pNext(a) = a2;
    pIter(a2);
    if (a2==NULL)
    {
      pNext(a) = a1;
      goto Finish;
    }
  }
  else 
  {
    assume(pComp0(a1, a2) == 1);
    a = pNext(a) = a1;
    pIter(a1);
    if (a1==NULL)
    {
      pNext(a) = a2;
      goto Finish;
    }
  }
  goto Top;
  

  Finish:  
  assume(pLength(pNext(&rp)) == (int) l);
  *lp = l;
  *p  = pNext(&rp);
}

_EOT_
  ;

##
## Third, kb_p_Mult_m
##

$input{"kb_p_Mult_m"}
  = ("chMODP".
     "_homGEN,homYES".
     "_nwONE,nwTWO,nwEVEN,nwODD"
    );
## Template of kb_p_Mult_m
$returnType{"kb_p_Mult_m"} = "poly";

$template{"kb_p_Mult_m"} = <<_EOT_
(poly p,
 poly m, 
 poly spNoether,
 memHeap heap)
{
#ifdef KB_USE_HEAPS
  assume(heap != NULL);
#else
  assume(heap == NULL);
#endif
  if (p == NULL) return NULL;
  spolyrec rp;
  poly q = &rp;
  number ln = pGetCoeff(m);
  int comp = pGetComp(m);
  CALL_ORDERINIT("p", "m");
  

  pSetComp(m, 0);

  while (p != NULL)
  {
    kb_pNew(pNext(q), heap);
    q = pNext(q);

    pSetCoeff0(q, CALL_NMULT("ln", "pGetCoeff(p)"));

    CALL_ORDERADD("q", "p", "m");

    CALL_EXPADD("q", "p", "m");
    
    p = pNext(p);
  }
  pNext(q) = NULL;
  pSetComp(m, comp);
  return pNext(&rp);
}
_EOT_
  ;

##
## Fourth, kb_p_Minus_m_Mult_q
##
$input{"kb_p_Minus_m_Mult_q"}
  = ("chMODP".
     "_otEXP,otCOMPEXP,otEXPCOMP".
     "_homGEN,homYES".
     "_nwONE,nwTWO,nwEVEN,nwODD".
":".
      "chMODP".
      "_otSYZDPC".
      "_homGEN".
      "_nwONE,nwTWO,nwEVEN,nwODD"
    );

## Template of kb_p_Minus_m_Mult_q
$returnType{"kb_p_Minus_m_Mult_q"} = "void";
$template{"kb_p_Minus_m_Mult_q"} = <<_EOT_
(poly *pp, int *lpp,
 poly m,
 poly q, int lq,
 poly spNoether,
 kb_p_Mult_m_Proc kb_p_Mult_m,
 memHeap heap)
{ 
#ifdef KB_USE_HEAPS
  assume(heap != NULL);
#else
  assume(heap == NULL);
#endif
  assume(pLength(q) == lq);
  assume(pLength(*pp) == *lpp);

  // we are done if q == NULL
  if (q == NULL || m == NULL) return;
  
  poly a = m,                         // collects the result
       qm = NULL,                     // stores q*m
       c,                             // used for temporary storage
       p = *pp;

  number tm   = pGetCoeff(m),       // coefficient of m
         tneg = CALL_NCOPYNEG("tm"),    // - (coefficient of m)
         tb,                        // used for tm*coeff(a1)
         tc;                        // used as intermediate number

  unsigned int lp = *lpp + lq;

  int comp = pGetComp(m);
  CALL_ORDERINIT("q", "m");

  pSetComp(m, 0);
  
  if (p == NULL) goto Finish; // we are done if p is 0

  kb_pNew(qm, heap);

  CALL_ORDERADD("qm", "q", "m");
  CALL_EXPADD("qm", "q", "m");
  
  // MAIN LOOP:
  Top:     // compare qm = m*q and p w.r.t. monomial ordering
    register long d;
    CALL_COMPARE("qm", "p", "d");

  Equal:   // qm equals p
    tb = CALL_NMULT("pGetCoeff(q)", "tm");
    tc = pGetCoeff(p);
    if (!CALL_NEQUAL("tc", "tb"))
    {
      lp--;
      tc = CALL_NSUB("tc", "tb");
      CALL_NDELETE("&pGetCoeff(p)");
      pSetCoeff0(p,tc); // adjust coeff of p
      a = pNext(a) = p; // append p to result and advance p
      pIter(p);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      lp -= 2;
      CALL_NDELETE("&tc");
      kb_pFree1AndAdvance(p, heap);
    }
    CALL_NDELETE("&tb");
    pIter(q);
    if (q == NULL || p == NULL) goto Finish; // are we done ?
    // no, so update qm
    CALL_ORDERADD("qm", "q", "m");
    CALL_EXPADD("qm", "q", "m");
    goto Top;

  NotEqual:     // qm != p 
    if (d < 0)  // qm < p: 
    {
      a = pNext(a) = p;// append p to result and advance p
      pIter(p);
      if (p == NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., qm > p
    {
      pSetCoeff0(qm,CALL_NMULT("pGetCoeff(q)", "tneg"));
      a = pNext(a) = qm;       // append qm to result and advance q
      pIter(q);
      if (q == NULL) // are we done?
      {
        qm = NULL;
        goto Finish; 
      }
      // construct new qm 
      kb_pNew(qm, heap);
      CALL_ORDERADD("qm", "q", "m");
      CALL_EXPADD("qm", "q", "m");
      goto Top;
    }
 
 Finish: // q or p is NULL: Clean-up time
   pSetComp(m, comp);
   if (q == NULL) // append rest of p to result
     pNext(a) = p;
   else  // append (- q*m) to result
   {
     pSetCoeff0(m, tneg);
     pNext(a) = kb_p_Mult_m(q, m, spNoether, heap);
     pSetCoeff0(m, tm);
   }
   
   CALL_NDELETE("&tneg");
   if (qm != NULL) kb_pFree1(qm, heap);

   *pp = pNext(m);
   *lpp = lp;
   pNext(m) = NULL;
   
   assume(pLength(*pp) == *lpp);
} 

_EOT_
  ;



###########################################################################
##
## Main program
##

print & Generate_EnumTypes(@Properties);

foreach $CurrProc (@ARGV)
{
  if ($template{$CurrProc} && $input{$CurrProc})
  {
    
    $CurrProcTemplate = $template{$CurrProc};
    $CurrProcReturnType = $returnType{$CurrProc};
    
    #flatten out input
    @finput = &FlattenInput(split(/:/, $input{$CurrProc}));
    
    #generate loops
    %loops = & Generate_CurrProcs(@finput);
    
    # Generate GetSpolyLoop based on generated loops
    $getspolyloop = & Generate_GetCurrProc(sort(keys(%loops)));
    
    # Output results

    foreach $key (sort(keys(%loops)))
    {
      print $loops{$key};
    }
    
    print $getspolyloop;
  }
  else
  {
    print (STDERR "No template for $CurrProc\n") if (! $template{$CurrProc});
    print (STDERR "No input for $CurrProc\n") if (! $input{$CurrProc});
  }
}



