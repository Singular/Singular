#!/usr/local/bin/perl
###########################################################################
<<<<<<< spSpolyLoop.pl
# $Id: spSpolyLoop.pl,v 1.10 1999-09-27 15:05:31 obachman Exp $
=======
# $Id: spSpolyLoop.pl,v 1.10 1999-09-27 15:05:31 obachman Exp $
>>>>>>> 1.8.6.1

###########################################################################
##
## FILE: spSpolyLoops.pl 
## PURPOSE: Generates spSpolyLoop's and spGetSpolyLoop as specified by
## variable @input. Writes result to stdout, warning/errors to stderr.
## AUTHOR: obachman (3/98)
##

###########################################################################
##
## How to add/modify generation of spSpolyLoops
##

# 1.) Add property/characterisitc to property specification and make
# sure that property is checked for in spGetSpolyLoop(...) in
# spSpolyLoop.cc
# 
# 2.) Modify macros of spSpolyLoops so that approriate actions are taken
# for new properties
# 
# 3.) Add properties to check for in @input



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
## Specification of the properties which determine an spSpolyLoop
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
## Template of the spSpolyLoop
## Is modified by Expand
##

$spSpolyLoopBodyTemplate = <<_EOT_
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

###########################################################################
##
## "Macros" needed in expansion of spSpolyLoop
##
sub NCOPYNEG
{
  local($number, $argv) = @_;
  
  return "npNegM($number)" if (& GetCharacteristic($argv) eq "chMODP");
  return "nNeg(nCopy($number))";
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
  
  return "$order = pGetOrder($p)" if (&GetHomog($argv) eq "homYES");
  return "";
}

  
sub PCOPYADDFAST
{
  local($p1, $p2, $p3, $order, $argv) = @_;
  
  return "pCopyAddFastHomog($p1, $p2, $p3, $order)" 
    if (&GetHomog($argv) eq "homYES");
  return "pCopyAddFast0($p1, $p2, $p3)";
}

###########################################################################
##
## COMPARE "macro": Besides generating th source code which
## accomplishes monomial comparisons, it also generates the (global)
## string $rargv charcaterising the generated spSpolyLoop
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
## Generates an spSpolyLoop from a given "$argv" input string,
## i.e. property string
## 
## Returns spSpolyLoop and sets global associative array
## $loops{$rargv} to generated loop.
##

sub Generate_SpolyLoop
{
  local($argv)= @_;
  local($loopbody);

  $loopbody = &Expand($spSpolyLoopBodyTemplate, $argv);

  & Warning("Can not realize $argv: Changed to $rargv") if ($argv ne $rargv);

  if (grep(/$rargv/, keys(%loops)))
  {
    & Warning("Duplicate entry $rargv: Ignoring $argv");
    return;
  }
  else
  {
    $loops{$rargv} = "static void spSpolyLoop_$rargv\n$loopbody\n";
    return ($loops{$rargv});
  }
}

#gnerates SpolyLoops from array of $argv strings
sub Generate_SpolyLoops
{
  local(%loops);
  
  foreach $argv (@_)
  {
    & Generate_SpolyLoop($argv);
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
## GetSpolyLoop routines
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

# recursively generates "GetSpolyLoop" by working through all $argv's
# which math $prefix
# Sets $checks{$prefix}, if $prefix eq $argv
sub GenerateBody_GetSpolyLoop
{
  local($prefix, $nextprefix, $source, $newsource, $prop, $curr_prop, $gen_key);
  
  $prefix = shift(@_);
  
  #check for exact match -- then we are done
  if (grep(/^$prefix$/, @_))
  {
    $checks{$prefix} = "return spSpolyLoop_$prefix;\n";
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
      $newsource=&GenerateBody_GetSpolyLoop($nextprefix,grep(/$nextprefix/,@_));
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
    $source = $source . &GenerateBody_GetSpolyLoop($gen_key,grep(/$gen_key/,@_));
  }
  return ($source);
}


sub Generate_GetSpolyLoop
{
  local($header);
  
  $header = "static spSpolyLoopProc spGetSpolyLoop(";
  
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
	  &GenerateBody_GetSpolyLoop("", @_). 
	  "return NULL;\n}\n");
}

###########################################################################
##
## Input Specification
##

@input = ("chMODP".
          "_otEXP,otCOMPEXP,otEXPCOMP".
	  "_homGEN,homYES".
	  "_nwONE,nwTWO,nwEVEN,nwODD",
# this does not seem to pay off -- general loop seems to be better
#           "chGEN".
#           "_otEXP,otCOMPEXP,otEXPCOMP".
#  	  "_homGEN".
#           "_nwGEN"
	 );


###########################################################################
##
## Main program
##

#flatten out input
@finput = &FlattenInput(@input);

#generate loops
%loops = & Generate_SpolyLoops(@finput);

# Generate GetSpolyLoop based on generated loops
$getspolyloop = & Generate_GetSpolyLoop(sort(keys(%loops)));

# Output results
print & Generate_EnumTypes(@Properties);

foreach $key (sort(keys(%checks)))
{
  print $loops{$key};
}

print $getspolyloop;

