#!/usr/local/bin/perl
###########################################################################
# $Id: generate.pl,v 1.1 1998-08-05 12:32:33 obachman Exp $

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
  
@OrderingTypes = ("otGEN", "otEXP", "otCOMPEXP", "otEXPCOMP");
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
	  & Warning("Different propes at same level: $prop : $curr_prop");
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
  
  $header = "static ".$CurrProc."Proc Get".$CurrProc."(";
  
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
## First, spSpolyLoop
##
$input{"spSpolyLoop"}
  = ("chMODP".
     "_otEXP,otCOMPEXP,otEXPCOMP".
     "_homGEN,homYES".
     "_nwONE,nwTWO,nwEVEN,nwODD"
# this does not seem to pay off -- general loop seems to be better
#      .":chGEN".
#      "_otEXP,otCOMPEXP,otEXPCOMP".
#      "_homGEN".
#      "_nwGEN"
    );

## Template of spSpolyLoop
$returnType{"spSpolyLoop"} = "void";
$template{"spSpolyLoop"} = <<_EOT_
(poly a1, poly a2, poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL,                          // stores a1*monom
       c;                                 // used for temporary storage
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = CALL_NCOPYNEG(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff

  Order_t order;                          // used for homog case

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  b = pNew();   

  CALL_INITORDER(order, a2);              // inits order for homog case
  

  CALL_PCOPYADDFAST(b, a1, monom, order);  // now a2 != NULL -- set up b

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    CALL_COMPARE(b, a2, d);

  Equal:   // b equals a2
    assume(pComp0(b, a2) == 0);
    tb = CALL_NMULT("pGetCoeff(a1)",tm);
    tc = pGetCoeff(a2);
    if (!CALL_NEQUAL(tc,tb))
    {
      tc=CALL_NSUB(tc, tb);
      CALL_NDELETE("&pGetCoeff(a2)");
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      c = a2;  // do not append anything to result: Delete a2 and advance
      pIter(a2);
      CALL_NDELETE("&tc");
      pFree1(c);
    }
    CALL_NDELETE("&tb");
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    CALL_PCOPYADDFAST(b, a1, monom, order); // No! So, get new b = a1*monom
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      assume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      assume(pComp0(b, a2) == 1);
      pSetCoeff0(b,CALL_NMULT("pGetCoeff(a1)",tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = pNew();
        goto Finish; 
      }
      b = pNew();
      CALL_PCOPYADDFAST(b, a1, monom, order); // No! So, update b = a1*monom
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   assume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else  // append (- a1*monom) to result 
     CALL_MULTCOPYX(a1, monom, a, tneg, spNoether);
   CALL_NDELETE("&tneg");
   if (b != NULL) pFree1(b);
} 

_EOT_
  ;

##
## Second, kbAdd
##

$input{"kbAdd"}
  = ("chMODP".
     "_otEXP,otCOMPEXP,otEXPCOMP".
     "_homGEN,homYES".
     "_nwONE,nwTWO,nwEVEN,nwODD");
## Template of kbAdd
$returnType{"kbAdd"} = "poly";

$template{"kbAdd"} = <<_EOT_
(poly a1, const unsigned int l1, 
 poly a2, const unsigned int l2, 
 unsigned int *length)
{
  int c;
  number t, n1, n2;
  unsigned int l = l1 + l2;
  spolyrec rp;
  poly a = &rp;
  pNext(a) = NULL;

  kbassume((pLength(a1) == (int) l1) && (pLength(a2) == (int) l2));
  
  if (a1 == NULL) 
  {
    *length = l;
    return a2;
  }
  
  if (a2==NULL) 
  {
    *length = l;
    return a1;
  }

  Top:     // compare a1 and a2 w.r.t. monomial ordering
  register long d;
  CALL_COMPARE(a1, a2, d);

  Equal:
  kbassume(pComp0(a1, a2) == 0);
  n1 = pGetCoeff(a1);
  n2 = pGetCoeff(a2);
  t=CALL_NADD("pGetCoeff(a1)","pGetCoeff(a2)");
  CALL_NDELETE(n1);
  CALL_NDLETE(n2);

  kb_pFree1AndAdvance(a2);

  if (CALL_NISZERO(t))
  {
    l -= 2;
    CALL_NDELETE("&t");
    kb_pFree1AndAdvance(a1);
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
    kbassume(pComp0(a1, a2) == -1);
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
    kbassume(pComp0(a1, a2) == 1);
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
  kbassume(pLength(pNext(&rp)) == (int) l);
  *length = l;
  
  return pNext(&rp);
}

_EOT_
  ;

##
## Third, kbMult
##

$input{"kbMult"}
  = ("chMODP".
     "_homGEN,homYES".
     "_nwONE,nwTWO,nwEVEN,nwODD"
    );
## Template of kbMult
$returnType{"kbMult"} = "poly";

$template{"kbMult"} = <<_EOT_
(poly a1, poly lm)
{
  spolyrec rp;
  poly p = &rp;
  number ln = pGetCoeff(lm);
  CALL_ORDERINIT(a1, lm);

  ln = CALL_NNEG(ln);
  _pSetComp(lm, 0);
  
  while (a1 != NULL)
  {
    kb_pNew(pNext(p));
    p = pNext(p);
    
    pSetCoeff0(p, CALL_NMULT("ln", "pGetCoeff(a1)"));

    CALL_ORDERADD(p, a1, lm);

    CALL_EXPADD(p, a1, lm);
    
    a1 = pNext(a1);
  }
  pNext(p) = NULL;
  CALL_NDELETE("&ln");
  pTest(rp.next);
  return rp.next;
}

_EOT_
  ;

##
## Fourth, kbSpolyLoop
##
$input{"kbSpolyLoop"}
  = ("chMODP".
     "_otEXP,otCOMPEXP,otEXPCOMP".
     "_homGEN,homYES".
     "_nwONE,nwTWO,nwEVEN,nwODD"
# this does not seem to pay off -- general loop seems to be better
#      .":chGEN".
#      "_otEXP,otCOMPEXP,otEXPCOMP".
#      "_homGEN".
#      "_nwGEN"
    );

## Template of kbSpolyLoop
$returnType{"kbSpolyLoop"} = "unsigned int";
$template{"kbSpolyLoop"} = <<_EOT_
(poly a1, const unsigned int l1, 
 poly a2, const unsigned int l2, 
 poly monom, poly spNoether)
{ 
  poly a = monom,                         // collects the result
       b = NULL;                          // stores a1*monom
  number tm   = pGetCoeff(monom),         // coefficient of monom
         tneg = CALL_NCOPYNEG(tm),        // - (coefficient of monom)
         tb,                              // used for tm*coeff(a1)
         tc;                              // used for intermediate coeff
  CALL_ORDERINIT(a1, monom);              // inits order for homog case

  unsigned int pl = l1 + l2;

  pTest(a1); kbassume(pLength(a1) == (int) l1);
  pTest(a2); kbassume(pLength(a2) == (int) l2);

  pSetComp(monom, 0);

  if (a2==NULL) goto Finish;              // we are done if a2 is 0
  kb_pNew(b);

  CALL_ORDERADD(b, a1, monom);               // now a2 != NULL -- set up b
  CALL_EXPADD(b, a1, monom);  
  pSetCoeff0(b, pGetCoeff(a1));

  // MAIN LOOP:
  Top:     // compare b = monom*a1 and a2 w.r.t. monomial ordering
    register long d;
    CALL_COMPARE(b, a2, d);

  Equal:   // b equals a2
    kbassume(pComp0(b, a2) == 0);
    tb = CALL_NMULT("pGetCoeff(a1)",tm);
    tc = pGetCoeff(a2);
    if (!CALL_NEQUAL(tc,tb))
    {
      pl--;
      tc=CALL_NSUB(tc, tb);
      CALL_NDELETE("&pGetCoeff(a2)");
      pSetCoeff0(a2,tc); // adjust coeff of a2
      a = pNext(a) = a2; // append a2 to result and advance a2
      pIter(a2);
    }
    else
    { // coeffs are equal, so their difference is 0: 
      CALL_NDELETE("&tc");
      kb_pFree1AndAdvance(a2);
      pl -= 2;
    }
    CALL_NDELETE("&tb");
    pIter(a1);
    if (a2 == NULL || a1 == NULL) goto Finish; // are we done ?
    CALL_ORDERADD(b, a1, monom);               
    CALL_EXPADD(b, a1, monom);  
    pSetCoeff0(b, pGetCoeff(a1));
    goto Top;

  NotEqual:     // b != a2 
    if (d < 0)  // b < a2: 
    {
      kbassume(pComp0(b, a2) == -1);
      a = pNext(a) = a2;// append a2 to result and advance a2
      pIter(a2);
      if (a2==NULL) goto Finish;;
      goto Top;
    }
    else // now d >= 0, i.e., b > a2
    {
      kbassume(pComp0(b, a2) == 1);
      pSetCoeff0(b,CALL_NMULT("pGetCoeff(a1)",tneg));
      a = pNext(a) = b;       // append b to result and advance a1
      pIter(a1);
      if (a1 == NULL)         // are we done?
      {
        b = NULL;
        goto Finish; 
      }
      kb_pNew(b); 
      CALL_ORDERADD(b, a1, monom);               
      CALL_EXPADD(b, a1, monom);  
      pSetCoeff0(b, pGetCoeff(a1));
      goto Top;
    }
 
 Finish: // a1 or a2 is NULL: Clean-up time
   kbassume(a1 == NULL || a2 == NULL);
   if (a1 == NULL) // append rest of a2 to result
     pNext(a) = a2;
   else 
      pNext(a) = kbmult(a1, monom);

   CALL_NDELETE("&tneg");
   if (b != NULL) kb_pFree1(b);
   pTest(pNext(monom));
   kbassume((unsigned int) pLength(pNext(monom)) == pl);
   return pl;
} 

_EOT_
  ;



###########################################################################
##
## Main program
##

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
    print & Generate_EnumTypes(@Properties);
    
    foreach $key (sort(keys(%checks)))
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



