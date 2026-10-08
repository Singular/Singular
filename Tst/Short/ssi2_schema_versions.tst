LIB "tst.lib";
tst_init();

proc check(int ok, string label)
{
  if (!ok)
  {
    ERROR(label);
  }
}

proc decode_fixture(string source, string target)
{
  string command=
    "LC_ALL=C perl -0777 -ne 's/[[:space:]]+//g; print pack(\"H*\",$_)' "
    +source+" > "+target;
  check(system("sh",command)==0,"decode "+source);
}

proc child_rejects(string code, string needle, string label)
{
  string executable=system("Singular");
  string marker="/.libs/Singular";
  int at=find(executable,marker);
  if (at>0)
  {
    executable=executable[1,at]+"Singular";
  }
  string command="\""+executable+"\" -q -t --no-rc -c '"+code+"' </dev/null";
  string output=tst_system(command,1);
  check(find(output,needle)>0,label);
  return(output);
}

proc check_v1_fixture(string link_name, string expected_ideal, string label)
{
  link input=link_name;
  def payload=read(input);
  def at_eof=read(input);
  close(input);
  kill input;

  check(typeof(payload)=="list",label+" list type");
  check(size(payload)==3,label+" list size");
  check(typeof(payload[1])=="int" && payload[1]==7,label+" integer");
  check(typeof(payload[2])=="ideal",label+" ideal type");
  check(string(payload[2])==expected_ideal,label+" ideal value");
  check(typeof(payload[3])=="string" && payload[3]=="schema-wrapper",
        label+" string");
  check(typeof(at_eof)=="none",label+" clean EOF");
  kill payload;
  kill at_eof;
}

check(system("sh",
  "rm -f ssi2_schema_versions_* ssi2_schema_versions.key ssi2_schema_current.ssi2") == 0,
      "remove stale schema-version files");

decode_fixture("ssi2_schema_v1.hex","ssi2_schema_versions_v1.ssi2");
decode_fixture("ssi2_schema_v1_xchacha20poly1305.hex",
               "ssi2_schema_versions_v1.ssi2e");
decode_fixture("ssi2_schema_v1_aes256gcm_fips.hex",
               "ssi2_schema_versions_v1.ssi2f");

ring R=0,(x,y,z),dp;
ideal expected=x^2+2*y+1/3,x*y-z^3,5*x^3*y+7/11;
string expected_ideal=string(expected);

// This is a fixed stream with schema versions 1 and no explicit MPZ entry.
// Do not regenerate it when a schema version is bumped: the test then requires
// the reader to retain the version-1 decoder.
check_v1_fixture("ssi2:r ssi2_schema_versions_v1.ssi2",expected_ideal,
                 "plain v1");

check(system("sh",
  "cp ssi2_schema_versions_v1.ssi2 ssi2_schema_versions_maxtok.ssi2 && LC_ALL=C perl -e 'open(F,\"+<\",shift) or exit 1; binmode F; seek(F,2,0) or exit 2; read(F,$b,1)==1 or exit 3; ord($b)==0xa1 or exit 4; seek(F,2,0) or exit 5; print F chr(0xa2); close(F) or exit 6' ssi2_schema_versions_maxtok.ssi2") == 0,
  "change recorded MAX_TOK");
check_v1_fixture("ssi2:r ssi2_schema_versions_maxtok.ssi2",expected_ideal,
                 "different MAX_TOK");

proc check_optional_v1_wrappers(string expected_ideal)
{
if (size(system("executable","gzip"))>0)
{
  check(system("sh",
    "gzip -c ssi2_schema_versions_v1.ssi2 > ssi2_schema_versions_v1.ssi2.gz") == 0,
    "compress gzip fixture");
  check_v1_fixture("ssi2:r ssi2_schema_versions_v1.ssi2.gz",expected_ideal,
                   "gzip v1");
}

if (size(system("executable","zstd"))>0)
{
  check(system("sh",
    "zstd -q -f ssi2_schema_versions_v1.ssi2 -o ssi2_schema_versions_v1.ssi2.zst") == 0,
    "compress zstd fixture");
  check_v1_fixture("ssi2:r ssi2_schema_versions_v1.ssi2.zst",expected_ideal,
                   "zstd v1");
  check_v1_fixture("ssi2c:r ssi2_schema_versions_v1.ssi2.zst",expected_ideal,
                   "ssi2c v1");
}

if (size(system("executable","lz4"))>0)
{
  check(system("sh",
    "lz4 -q -f ssi2_schema_versions_v1.ssi2 ssi2_schema_versions_v1.ssi2.lz4") == 0,
    "compress lz4 fixture");
  check_v1_fixture("ssi2:r ssi2_schema_versions_v1.ssi2.lz4",expected_ideal,
                   "lz4 v1");
}

string fixture_key=
  "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f";
link key_output="ASCII:w ssi2_schema_versions.key";
write(key_output,fixture_key);
close(key_output);
kill key_output;

link sodium_feature="ssi2e: ";
string sodium_encryption=status(sodium_feature,"encryption");
kill sodium_feature;
if (sodium_encryption=="xchacha20poly1305")
{
  check_v1_fixture(
    "ssi2e:r,keyfile=ssi2_schema_versions.key ssi2_schema_versions_v1.ssi2e",
    expected_ideal,"encrypted v1");
}
else
{
  check(sodium_encryption=="unavailable","disabled libsodium status");
}

link fips_feature="ssi2f: ";
string fips_encryption=status(fips_feature,"encryption");
kill fips_feature;
if (fips_encryption=="aes-256-gcm-fips")
{
  check_v1_fixture(
    "ssi2f:r,keyfile=ssi2_schema_versions.key ssi2_schema_versions_v1.ssi2f",
    expected_ideal,"FIPS encrypted v1");
}
else
{
  check(fips_encryption=="unavailable","disabled FIPS status");
}
}
check_optional_v1_wrappers(expected_ideal);

bigint current_value=123456789012345678901234567890;
link current_output="ssi2:w ssi2_schema_current.ssi2";
write(current_output,current_value);
close(current_output);
kill current_output;
link current_append="ssi2:a ssi2_schema_current.ssi2";
write(current_append,current_value);
close(current_append);
kill current_append;
link current_input="ssi2:r ssi2_schema_current.ssi2";
def current_recovered=read(current_input);
def current_recovered_again=read(current_input);
close(current_input);
kill current_input;
check(typeof(current_recovered)=="bigint" && current_recovered==current_value,
      "current MPZ roundtrip");
check(typeof(current_recovered_again)=="bigint"
      && current_recovered_again==current_value,"appended MPZ roundtrip");
kill current_recovered;
kill current_recovered_again;

string mpz_mutator=
  "LC_ALL=C perl -e 'open(F,\"<\",shift) or exit 1; binmode F; local $/; $d=<F>; close F; $p=0; sub v {$x=0;$s=0;while(1){$b=ord(substr($d,$p++,1));$x|=($b&127)<<$s;last unless $b&128;$s+=7;}return $x;} ord(substr($d,$p++,1))==98 or exit 2; v() for 1..4; ord(substr($d,$p++,1))==26 or exit 3; v(); $n=v(); $at=-1; for(1..$n){$id=v();$start=$p;v();$at=$start if $id==16;} $at>=0 or exit 4; substr($d,$at,1)=chr(2); open(G,\">\",shift) or exit 5; binmode G; print G $d; close G or exit 6' ssi2_schema_current.ssi2 ssi2_schema_versions_mpz2.ssi2";
check(system("sh",mpz_mutator) == 0,"prepare incompatible MPZ schema");
string rejection=child_rejects(
  "link in=\"ssi2:r ssi2_schema_versions_mpz2.ssi2\"; def value=read(in); quit;",
  "unsupported mpz schema version 2","MPZ schema rejection");

string second_mpz_mutator=
  "LC_ALL=C perl -e 'open(F,\"<\",shift) or exit 1; binmode F; local $/; $d=<F>; close F; length($d)%2==0 or exit 2; $half=length($d)/2; substr($d,0,$half) eq substr($d,$half) or exit 3; $p=$half; sub v {$x=0;$s=0;while(1){$b=ord(substr($d,$p++,1));$x|=($b&127)<<$s;last unless $b&128;$s+=7;}return $x;} ord(substr($d,$p++,1))==98 or exit 4; v() for 1..4; ord(substr($d,$p++,1))==26 or exit 5; v(); $n=v(); $at=-1; for(1..$n){$id=v();$start=$p;v();$at=$start if $id==16;} $at>=0 or exit 6; substr($d,$at,1)=chr(2); open(G,\">\",shift) or exit 7; binmode G; print G $d; close G or exit 8' ssi2_schema_current.ssi2 ssi2_schema_versions_second_mpz2.ssi2";
check(system("sh",second_mpz_mutator) == 0,
      "prepare incompatible MPZ schema in appended segment");
rejection=child_rejects(
  "link in=\"ssi2:r ssi2_schema_versions_second_mpz2.ssi2\"; def first=read(in); if(typeof(first)!=\"bigint\"){ERROR(\"first value\");}; def second=read(in); quit;",
  "unsupported mpz schema version 2","appended MPZ schema rejection");

check(system("sh",
  "rm -f ssi2_schema_versions_* ssi2_schema_versions.key ssi2_schema_current.ssi2") == 0,
  "remove schema-version files");

tst_status(1);$
