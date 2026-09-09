#!/usr/bin/env bash
set -euo pipefail

singular="${1:?missing Singular executable}"
compat_dir=".windows-ssi-compat-$$-$RANDOM"
mkdir "$compat_dir"
trap 'rm -rf "$compat_dir"' EXIT
compat_lib="$compat_dir/windows_line_endings.lib"

printf '%s\n' \
  'version="version windows_line_endings.lib 1.0.0.0 Sep_2026 ";' \
  'category="Tests";' \
  'proc windowsLineEndingProc(int x)' \
  '{' \
  '  return(x+1);' \
  '}' > "$compat_lib"

run_compat_test()
{
  local target="$1"
  local input="$target.sing"
  {
    printf 'string compatLib="%s";\n' "$compat_lib"
    printf 'string compatSsi="%s";\n' "$target"
    cat .github/scripts/windows-ssi-compat.sing
  } > "$input"
  "$singular" "$input" -q 2>&1
}

lf_output="$(run_compat_test "$compat_dir/lf.ssi")"
test "$lf_output" = 'SSI_TYPES_OK'

awk '{ printf "%s\r\n", $0 }' "$compat_lib" > "$compat_lib.tmp"
mv "$compat_lib.tmp" "$compat_lib"
crlf_output="$(run_compat_test "$compat_dir/crlf.ssi")"
test "$crlf_output" = 'SSI_TYPES_OK'

cmp "$compat_dir/lf.ssi" "$compat_dir/crlf.ssi"
if LC_ALL=C grep -q $'\r' "$compat_dir/crlf.ssi"; then
  echo 'Native Windows SSI output contains CR bytes' >&2
  exit 1
fi

printf '%s\n' "$crlf_output"
