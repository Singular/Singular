LIB "tst.lib";
tst_init();

proc check(int ok, string label)
{
  if (!ok)
  {
    ERROR(label);
  }
}

proc child_rejects(string code, string needle, string label)
{
  string executable=system("Singular");
  string marker="/.libs/Singular";
  int at=find(executable,marker);
  // Use the libtool wrapper when testing an uninstalled build tree.
  if (at>0)
  {
    executable=executable[1,at]+"Singular";
  }
  string command="\""+executable+"\" -q -t --no-rc -c '"+code+"' </dev/null";
  string output=tst_system(command,1);
  check(find(output,needle)>0,label);
  return(output);
}

link plain_feature="ssi2: ";
check(status(plain_feature,"encryption")=="none",
      "plain SSI2 is not reported as encrypted");
kill plain_feature;

link feature="ssi2e: ";
string encryption=status(feature,"encryption");
kill feature;

if (encryption=="xchacha20poly1305")
{
  string good_key=
    "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f";
  string wrong_key=
    "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";

  link key_out="ASCII:w ssi2_encryption.key";
  write(key_out,good_key);
  close(key_out);
  kill key_out;
  link wrong_key_out="ASCII:w ssi2_encryption_wrong.key";
  write(wrong_key_out,wrong_key);
  close(wrong_key_out);
  kill wrong_key_out;

  ring R=0,(x,y,z),dp;
  ideal I=x^2+2*y+1/3,x*y-z^3,5*x^3*y+7/11;
  string secret="SSI2E-CONFIDENTIAL-MARKER";
  string expected=string(I);
  string large="0123456789abcdef";
  int chunk_power;
  for (chunk_power=1;chunk_power<=17;chunk_power++)
  {
    large=large+large;
  }

  // Reuse the same link after close; keyfile= must survive reopening.
  link encrypted=
    "ssi2e:w,keyfile=ssi2_encryption.key ssi2_encryption.ssi2e";
  write(encrypted,secret);
  write(encrypted,I);
  write(encrypted,large);
  close(encrypted);
  def recovered_secret=read(encrypted);
  def recovered_ideal=read(encrypted);
  def recovered_large=read(encrypted);
  close(encrypted);
  check(typeof(recovered_secret)=="string","encrypted string type");
  check(recovered_secret==secret,"encrypted string value");
  check(typeof(recovered_ideal)=="ideal","encrypted ideal type");
  check(string(recovered_ideal)==expected,"encrypted ideal value");
  check(typeof(recovered_large)=="string","chunked string type");
  check(recovered_large==large,"chunked string value");
  kill encrypted;
  kill recovered_secret;
  kill recovered_ideal;
  kill recovered_large;

  // The nonce-bearing stream header makes repeated encryption distinct.
  link second=
    "ssi2e:w,keyfile=ssi2_encryption.key ssi2_encryption_second.ssi2e";
  write(second,secret,I,large);
  close(second);
  kill second;
  check(system("sh","cmp -s ssi2_encryption.ssi2e ssi2_encryption_second.ssi2e")!=0,
        "fresh encrypted stream header");
  check(system("sh","LC_ALL=C grep -a -q SSI2E-CONFIDENTIAL-MARKER ssi2_encryption.ssi2e")!=0,
        "plaintext marker is hidden");

  string output=child_rejects(
    "link in=\"ssi2e:r,keyfile=ssi2_encryption_wrong.key ssi2_encryption.ssi2e\"; def value=read(in); close(in); quit;",
    "authentication failed","wrong key rejection");
  check(find(output,wrong_key)==0,"key material is not logged");

  check(system("sh",
    // 40 = 12-byte envelope header + 24-byte secretstream header
    //      + 4-byte first-frame length.
    "cp ssi2_encryption.ssi2e ssi2_encryption_tampered.ssi2e && LC_ALL=C LANG=C perl -e 'open(F,\"+<\",shift) or exit 1; binmode F; seek(F,40,0) or exit 2; read(F,$b,1)==1 or exit 3; seek(F,40,0) or exit 4; print F chr(ord($b)^1); close(F) or exit 5' ssi2_encryption_tampered.ssi2e")==0,
    "prepare tampered stream");
  output=child_rejects(
    "link in=\"ssi2e:r,keyfile=ssi2_encryption.key ssi2_encryption_tampered.ssi2e\"; def value=read(in); close(in); quit;",
    "authentication failed","ciphertext tampering rejection");

  check(system("sh",
    "size=$(wc -c < ssi2_encryption.ssi2e); test \"$size\" -gt 1 && dd if=ssi2_encryption.ssi2e of=ssi2_encryption_truncated.ssi2e bs=1 count=$((size-1)) 2>/dev/null")==0,
    "prepare truncated stream");
  output=child_rejects(
    "link in=\"ssi2e:r,keyfile=ssi2_encryption.key ssi2_encryption_truncated.ssi2e\"; def value=read(in); close(in); quit;",
    "truncated encrypted frame","truncation rejection");

  check(system("sh",
    "cp ssi2_encryption.ssi2e ssi2_encryption_trailing.ssi2e && printf X >> ssi2_encryption_trailing.ssi2e")==0,
    "prepare stream with trailing data");
  output=child_rejects(
    "link in=\"ssi2e:r,keyfile=ssi2_encryption.key ssi2_encryption_trailing.ssi2e\"; def value=read(in); close(in); quit;",
    "trailing data after final frame","trailing data rejection");

  output=child_rejects(
    "link out=\"ssi2e:a,keyfile=ssi2_encryption.key ssi2_encryption.ssi2e\"; write(out,1); close(out); quit;",
    "append mode is not supported","append rejection");

  check(system("sh",
    "rm -f ssi2_encryption.key ssi2_encryption_wrong.key ssi2_encryption.ssi2e ssi2_encryption_second.ssi2e ssi2_encryption_tampered.ssi2e ssi2_encryption_truncated.ssi2e ssi2_encryption_trailing.ssi2e")==0,
    "remove encryption test files");
}

tst_status(1);$
