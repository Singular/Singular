LIB "tst.lib";
tst_init();

proc check(int ok, string label)
{
  if (!ok)
  {
    ERROR(label);
  }
}

proc child_output(string code, int restricted)
{
  string executable=system("Singular");
  string marker="/.libs/Singular";
  int at=find(executable,marker);
  // Use the libtool wrapper when testing an uninstalled build tree.
  if (at>0)
  {
    executable=executable[1,at]+"Singular";
  }
  string options=" -q -t --no-rc -c '";
  if (restricted)
  {
    // Singular currently initializes restricted mode only without -q.
    options=" -t --no-rc --no-shell -c '";
  }
  string command="\""+executable+"\""+options+code+"' </dev/null";
  return(tst_system(command,1));
}

proc child_rejects(string code, string needle, string label, int restricted)
{
  string output=child_output(code,restricted);
  check(find(output,needle)>0,label);
  return(output);
}

check(system("sh",
  "rm -f ssi2_robust_*.ssi2") == 0,
  "remove stale robustness files");

link base="ssi2:w ssi2_robust_base.ssi2";
write(base,7);
close(base);
kill base;

link eof="ssi2:r ssi2_robust_base.ssi2";
def first=read(eof);
def at_eof=read(eof);
check(typeof(first)=="int" && first==7,"value before clean EOF");
check(typeof(at_eof)=="none","clean EOF value");
kill first;
kill at_eof;
kill eof;

check(system("sh",
  "size=$(wc -c < ssi2_robust_base.ssi2); test \"$size\" -gt 1 && dd if=ssi2_robust_base.ssi2 of=ssi2_robust_truncated.ssi2 bs=1 count=$((size-1)) 2>/dev/null") == 0,
  "prepare truncated stream");
string output=child_rejects(
  "link in=\"ssi2:r ssi2_robust_truncated.ssi2\"; def value=read(in); quit;",
  "unexpected end of input","truncated stream rejection",0);

output=child_rejects(
  "link in=\"ssi2:r ssi2_robust_missing.ssi2\"; def value=read(in); quit;",
  "cannot open input file","missing input rejection",0);

output=child_rejects(
  "link out=\"ssi2:w ssi2_robust_restricted.ssi2\"; write(out,1); quit;",
  "no links allowed","restricted mode rejection",1);
check(system("sh","test ! -e ssi2_robust_restricted.ssi2") == 0,
      "restricted mode created no file");

output=child_rejects(
  "ring R=real,x,dp; poly p=1.25*x+2.5; link out=\"ssi2:w ssi2_robust_unsupported.ssi2\"; write(out,p); quit;",
  "coeff type 3 not implemented","unsupported coefficient rejection",0);

check(system("sh",
  "cp ssi2_robust_base.ssi2 ssi2_robust_version.ssi2 && LC_ALL=C perl -e 'open(F,\"+<\",shift) or exit 1; binmode F; seek(F,1,0) or exit 2; print F chr(2); close(F) or exit 3' ssi2_robust_version.ssi2") == 0,
  "prepare incompatible stream version");
output=child_rejects(
  "link in=\"ssi2:r ssi2_robust_version.ssi2\"; def value=read(in); quit;",
  "incompatible stream version 2","stream version rejection",0);

string schema_mutator=
  "LC_ALL=C perl -e 'open(F,\"<\",shift) or exit 1; binmode F; local $/; $d=<F>; close F; $p=0; sub v {$x=0;$s=0;while(1){$b=ord(substr($d,$p++,1));$x|=($b&127)<<$s;last unless $b&128;$s+=7;}return $x;} ord(substr($d,$p++,1))==98 or exit 2; v() for 1..4; ord(substr($d,$p++,1))==26 or exit 3; v(); $n=v(); $at=-1; for(1..$n){$id=v();$start=$p;v();$at=$start if $id==7;} $at>=0 or exit 4; substr($d,$at,1)=chr(2); substr($d,$p)=pack(\"C*\",1,14,14,0); open(G,\">\",shift) or exit 5; binmode G; print G $d; close G or exit 6' ssi2_robust_base.ssi2 ssi2_robust_schema.ssi2";
check(system("sh",schema_mutator) == 0,
      "prepare incompatible list schema");
output=child_rejects(
  "link in=\"ssi2:r ssi2_robust_schema.ssi2\"; def first=read(in); if(first!=7){ERROR(\"first\"+\" value\");}; def value=read(in); quit;",
  "unsupported list schema version 2","per-type schema rejection",0);
check(find(output,"first value")==0,"unchanged schema remains readable");

check(system("sh",
  "size=$(wc -c < ssi2_robust_base.ssi2); test \"$size\" -gt 2 && dd if=ssi2_robust_base.ssi2 of=ssi2_robust_cache.ssi2 bs=1 count=$((size-2)) 2>/dev/null && printf '\\005\\011\\024' >> ssi2_robust_cache.ssi2") == 0,
  "prepare invalid ring cache reference");
output=child_rejects(
  "link in=\"ssi2:r ssi2_robust_cache.ssi2\"; def value=read(in); quit;",
  "ring cache index is too large","ring cache bounds",0);

check(system("sh",
  "size=$(wc -c < ssi2_robust_base.ssi2); test \"$size\" -gt 2 && dd if=ssi2_robust_base.ssi2 of=ssi2_robust_property.ssi2 bs=1 count=$((size-2)) 2>/dev/null && printf '\\027\\000\\001' >> ssi2_robust_property.ssi2") == 0,
  "prepare ring property without ring");
output=child_rejects(
  "link in=\"ssi2:r ssi2_robust_property.ssi2\"; def value=read(in); quit;",
  "ring property without a ring","ring property bounds",0);

check(system("sh",
  "size=$(wc -c < ssi2_robust_base.ssi2); test \"$size\" -gt 2 && dd if=ssi2_robust_base.ssi2 of=ssi2_robust_list.ssi2 bs=1 count=$((size-2)) 2>/dev/null && printf '\\016\\200\\200\\200\\200\\010' >> ssi2_robust_list.ssi2") == 0,
  "prepare oversized list");
output=child_rejects(
  "link in=\"ssi2:r ssi2_robust_list.ssi2\"; def value=read(in); quit;",
  "list length is too large","container bounds",0);

check(system("sh","rm -f ssi2_robust_*.ssi2") == 0,
      "remove robustness files");

tst_status(1);$
