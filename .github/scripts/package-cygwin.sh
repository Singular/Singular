#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 5 ]]; then
  echo "usage: $0 SOURCE_ROOT ARTIFACT_VERSION PACKAGE_VERSION PACKAGE_RELEASE OUTPUT_DIR" >&2
  exit 2
fi

source_root="$(cd "$1" && pwd)"
artifact_version="$2"
package_version="$3"
package_release="$4"
output_dir="$5"
bundle_old_docs="${BUNDLE_OLD_DOCS:-false}"
artifact_suffix="${ARTIFACT_SUFFIX:-}"
package_name=singular-upstream
install_prefix="/opt/$package_name/$artifact_version"
runner_temp="${RUNNER_TEMP:-/tmp}"
if [[ "$runner_temp" =~ ^[A-Za-z]: ]]; then
  runner_temp="$(cygpath -u "$runner_temp")"
fi
build_dir="$runner_temp/singular-build-cygwin"
stage="$runner_temp/singular-stage-cygwin"
repository="$output_dir/cygwin-overlay"
package_dir="$repository/x86_64/release/$package_name"

case "$bundle_old_docs" in
  true|false) ;;
  *)
    echo "BUNDLE_OLD_DOCS must be true or false" >&2
    exit 2
    ;;
esac
if [[ "$bundle_old_docs" == true && ! -s "$source_root/doc/doc.tbz2" ]]; then
  echo "BUNDLE_OLD_DOCS=true, but doc/doc.tbz2 is missing" >&2
  exit 1
fi

rm -rf "$build_dir" "$stage" "$repository"
mkdir -p "$build_dir" "$stage" "$package_dir"

(
  cd "$source_root"
  mkdir -p /usr/local/bin
  ln -sf /usr/bin/autoconf-2.73 /usr/local/bin/autoconf
  ln -sf /usr/bin/autoheader-2.73 /usr/local/bin/autoheader
  ln -sf /usr/bin/autom4te-2.73 /usr/local/bin/autom4te
  ln -sf /usr/bin/autoreconf-2.73 /usr/local/bin/autoreconf
  ln -sf /usr/bin/aclocal-1.18 /usr/local/bin/aclocal
  ln -sf /usr/bin/automake-1.18 /usr/local/bin/automake
  ./autogen.sh
)

(
  cd "$build_dir"
  "$source_root/configure" \
    --prefix="$install_prefix" \
    --disable-shared \
    --enable-gfanlib \
    --disable-p-procs-dynamic \
    --enable-p-procs-static \
    --with-builtinmodules=gfanlib,gitfan,interval,loctriv,partialgb,syzextra,customstd,cohomo,subsets,freealgebra,systhreads \
    --without-python \
    --without-pythonmodule \
    --disable-cf-inline
  make -j"${BUILD_JOBS:-2}"
  make -C omalloc check
  make install DESTDIR="$stage"
)

if [[ "$bundle_old_docs" == true ]]; then
  test -s "$stage$install_prefix/share/info/singular.info"
  test -s "$stage$install_prefix/share/singular/singular.idx"
  html_manual="$stage$install_prefix/share/doc/singular"
  if [[ ! -d "$html_manual" || \
        -z "$(find "$html_manual" -type f -print -quit)" ]]; then
    echo "The legacy HTML manual was not installed" >&2
    exit 1
  fi
else
  # Standard binary packages use the online manual. Do not retain legacy
  # documentation if the source tree happened to contain doc.tbz2.
  rm -rf "$stage$install_prefix/share/doc/singular"
  rm -f \
    "$stage$install_prefix/share/info/singular.info" \
    "$stage$install_prefix/share/singular/singular.idx"
  test ! -e "$stage$install_prefix/share/info/singular.info"
  test ! -e "$stage$install_prefix/share/singular/singular.idx"
fi

singular="$stage$install_prefix/bin/Singular.exe"
if [[ ! -x "$singular" ]]; then
  echo "The staged Cygwin Singular executable is missing: $singular" >&2
  exit 1
fi

export SINGULARPATH="$stage$install_prefix/share/singular/LIB"
smoke_output="$(printf '%s\n' \
  'LIB "gfanlib.so";' \
  'intmat gfanPoints[2][2]=1,0,0,1;' \
  'cone c=coneViaPointsInternal(gfanPoints);' \
  'print("GFANLIB_DIM="+string(dimensionInternal(c)));' \
  'ring r=0,(x,y),dp;' \
  'ideal i=x^2,y^2;' \
  'std(i);' \
  'factorize(x^6-y^6);' \
  'print("CYGWIN_PACKAGE_SMOKE_OK");' \
  'quit;' \
  | "$singular" -q 2>&1)"
printf '%s\n' "$smoke_output"
grep -q GFANLIB_DIM=2 <<< "$smoke_output"
grep -q CYGWIN_PACKAGE_SMOKE_OK <<< "$smoke_output"

mkdir -p \
  "$stage/usr/bin" \
  "$stage/usr/share/doc/Cygwin" \
  "$stage/usr/share/doc/$package_name"

cat > "$stage/usr/bin/Singular-upstream" <<EOF
#!/usr/bin/env bash
exec "$install_prefix/bin/Singular" "\$@"
EOF
chmod 0755 "$stage/usr/bin/Singular-upstream"
ln -s Singular-upstream "$stage/usr/bin/singular-upstream"

cat > "$stage/usr/share/doc/Cygwin/$package_name.README" <<EOF
$package_name is the binary distribution produced by the Singular project.

It deliberately uses a distinct package name, command name, and installation
prefix so that it does not replace or modify any separately maintained Cygwin
package. Run it as Singular-upstream or singular-upstream.

Installation prefix: $install_prefix
Singular source commit: $(git -C "$source_root" rev-parse HEAD)
EOF

declare -A installed_versions=()
while read -r package version; do
  [[ -n "$package" && -n "$version" ]] || continue
  installed_versions["$package"]="$version"
done < <(cygcheck -cd | awk 'NR > 2 { print $1, $2 }')

package_from_record()
{
  local record="$1"
  local candidate
  local best=""

  for candidate in "${!installed_versions[@]}"; do
    if [[ "$record" == "$candidate-"* && ${#candidate} -gt ${#best} ]]; then
      best="$candidate"
    fi
  done
  printf '%s\n' "$best"
}

declare -A runtime_packages=([bash]=1 [cygwin]=1)
while IFS= read -r -d '' binary; do
  if ! file -b "$binary" | grep -Eq 'PE32|MS Windows'; then
    continue
  fi

  while IFS= read -r dependency; do
    dependency="$(sed 's/^[[:space:]]*//;s/[[:space:]]*$//' <<< "$dependency")"
    [[ -n "$dependency" ]] || continue
    unix_dependency="$(cygpath -u "$dependency" 2>/dev/null || true)"
    [[ -f "$unix_dependency" ]] || continue
    owner_record="$(cygcheck -f "$unix_dependency" 2>/dev/null | sed -n '1p')"
    [[ -n "$owner_record" ]] || continue
    owner_package="$(package_from_record "$owner_record")"
    [[ -n "$owner_package" ]] || continue
    runtime_packages["$owner_package"]=1
  done < <(cygcheck "$binary" 2>/dev/null | sed -n '/\.dll$/Ip')
done < <(find "$stage$install_prefix" -type f -print0)

runtime_manifest="$runner_temp/cygwin-runtime-packages.txt"
: > "$runtime_manifest"
while IFS= read -r package; do
  if [[ -z "${installed_versions[$package]:-}" ]]; then
    echo "Could not determine the installed version of Cygwin package $package" >&2
    exit 1
  fi
  printf '%s %s\n' "$package" "${installed_versions[$package]}" \
    >> "$runtime_manifest"
done < <(printf '%s\n' "${!runtime_packages[@]}" | LC_ALL=C sort)

RUNTIME_PACKAGE_MANIFEST="$runtime_manifest" \
  "$source_root/.github/scripts/collect-binary-notices.sh" \
    "$source_root" \
    "$stage/usr/share/doc/$package_name/THIRD_PARTY_LICENSES" \
    "Cygwin x86_64"

cat > "$stage/usr/share/doc/$package_name/SOURCE.txt" <<EOF
Singular source commit: $(git -C "$source_root" rev-parse HEAD)
Source: ${GITHUB_SERVER_URL:-https://github.com}/${GITHUB_REPOSITORY:-Singular/Singular}/tree/$(git -C "$source_root" rev-parse HEAD)
EOF

requires="$(printf '%s\n' "${!runtime_packages[@]}" | LC_ALL=C sort | paste -sd ' ' -)"
package_base="$package_name-$package_version-$package_release"
binary_package="$package_dir/$package_base.tar.xz"
source_package="$package_dir/$package_base-src.tar.xz"

tar -C "$stage" -cJf "$binary_package" opt usr
git -C "$source_root" archive --format=tar \
  --prefix="$package_base/" HEAD \
  | xz -9 -c > "$source_package"

cat > "$package_dir/$package_base.hint" <<EOF
sdesc: "Singular upstream computer algebra system"
ldesc: "Binary distribution produced by the Singular project, installed separately from other Cygwin Singular packages"
category: Math
requires: $requires
EOF

(
  cd "$repository"
  mksetupini \
    --arch x86_64 \
    --inifile=x86_64/setup.ini \
    --releasearea=. \
    --disable-check=missing-required-package,missing-depended-package
  bzip2 -9 -c x86_64/setup.ini > x86_64/setup.bz2
  xz -6e -c x86_64/setup.ini > x86_64/setup.xz
)

cat > "$repository/README.txt" <<EOF
Singular project Cygwin overlay repository

Serve this directory as a separate Cygwin package-server URL and select it
together with a standard Cygwin mirror. The package is named $package_name and
does not replace a separately maintained package named singular.

Before public deployment, sign the generated setup metadata with the key that
users will pass to Cygwin Setup via its -K option.
EOF

(
  cd "$repository"
  find . -type f ! -name SHA512SUMS -print0 \
    | LC_ALL=C sort -z \
    | xargs -0 sha512sum > SHA512SUMS
)

archive="$output_dir/Singular-$artifact_version-cygwin-x86_64${artifact_suffix}-overlay.tar.xz"
tar -C "$output_dir" -cJf "$archive" "$(basename "$repository")"

printf 'Cygwin package: %s\n' "$binary_package"
printf 'Cygwin overlay archive: %s\n' "$archive"
