#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 6 ]]; then
  echo "usage: $0 SOURCE_ROOT VERSION RELEASE FORMAT OUTPUT_DIR ARTIFACT_BASENAME" >&2
  exit 2
fi

source_root="$(cd "$1" && pwd)"
version="$2"
package_release="$3"
format="$4"
output_dir="$5"
artifact_basename="$6"
build_dir="$RUNNER_TEMP/singular-build-linux"
stage="$RUNNER_TEMP/singular-stage"

case "$format" in
  deb)
    architecture="$(dpkg --print-architecture)"
    libdir="/usr/lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH)"
    notice_dir="$stage/usr/share/doc/singular/THIRD_PARTY_LICENSES"
    ;;
  rpm)
    architecture="$(rpm --eval '%{_arch}')"
    libdir="$(rpm --eval '%{_libdir}')"
    notice_dir="$stage/usr/share/licenses/singular"
    ;;
  *)
    echo "Unsupported package format: $format" >&2
    exit 2
    ;;
esac

rm -rf "$build_dir" "$stage"
mkdir -p "$build_dir" "$stage" "$output_dir"

configure_flags=(
  --prefix=/usr
  "--libdir=$libdir"
  --enable-gfanlib
  --with-ntl=yes
  --with-flint=yes
  --without-python
  --disable-python
)

runtime_manifest="$RUNNER_TEMP/runtime-packages.txt"
: > "$runtime_manifest"
package_license="(GPL-2.0-only OR GPL-3.0-only) AND BSD-3-Clause"

if [[ "$format" == deb ]]; then
  dpkg-query -W -f='${binary:Package} ${Version}\n' \
    libc6 libgcc-s1 libstdc++6 libgmp-dev libmpfr-dev libreadline-dev \
    libntl-dev libflint-dev libcdd-dev >> "$runtime_manifest"
else
  rpm -q --qf '%{NAME} %{VERSION}-%{RELEASE}\n' \
    glibc libgcc libstdc++ gmp-devel mpfr-devel readline-devel \
    ntl-devel flint-devel cddlib-devel >> "$runtime_manifest"
fi

(
  cd "$source_root"
  ./autogen.sh
)
(
  cd "$build_dir"
  "$source_root/configure" "${configure_flags[@]}"
  make -j"${BUILD_JOBS:-2}"
  make install DESTDIR="$stage"
)

export RUNTIME_PACKAGE_MANIFEST="$runtime_manifest"
"$source_root/.github/scripts/collect-binary-notices.sh" \
  "$source_root" "$notice_dir" "Linux $architecture ($format)"

mkdir -p "$stage/usr/share/doc/singular"
cat > "$stage/usr/share/doc/singular/SOURCE.txt" <<EOF
Singular source commit: $(git -C "$source_root" rev-parse HEAD)
Source: ${GITHUB_SERVER_URL:-https://github.com}/${GITHUB_REPOSITORY:-Singular/Singular}/tree/$(git -C "$source_root" rev-parse HEAD)
EOF

if [[ "$format" == deb ]]; then
  cat > "$stage/usr/share/doc/singular/copyright" <<EOF
This package was built from Singular at the source revision recorded in
SOURCE.txt. Its package license expression is:

  $package_license

The complete Singular, embedded BSD, and bundled runtime notices are installed
in THIRD_PARTY_LICENSES/.
EOF
fi

singular="$build_dir/Singular/Singular"
smoke_output="$(printf '%s\n' \
  'ring r=0,(x,y),dp;' \
  'ideal i=x^2,y^2;' \
  'size(std(i));' \
  'print("BINARY_PACKAGE_SMOKE_OK");' \
  'quit;' | "$singular" -q 2>&1)"
printf '%s\n' "$smoke_output"
grep -q BINARY_PACKAGE_SMOKE_OK <<< "$smoke_output"

if [[ "$format" == deb ]]; then
  mkdir -p "$stage/DEBIAN"
  mapfile -d '' elf_files < <(
    find "$stage" -type f -print0 \
      | while IFS= read -r -d '' file; do
          if file -b "$file" | grep -q '^ELF '; then
            printf '%s\0' "$file"
          fi
        done
  )

  shlib_args=()
  for file in "${elf_files[@]}"; do
    shlib_args+=("-e$file")
  done
  shlib_args+=("-l$stage$libdir" --ignore-missing-info -O)
  mkdir -p "$RUNNER_TEMP/debian"
  printf 'Source: singular\nPackage: singular\nArchitecture: any\n' \
    > "$RUNNER_TEMP/debian/control"
  dependencies="$(
    cd "$RUNNER_TEMP"
    dpkg-shlibdeps "${shlib_args[@]}" \
      | sed -n 's/^shlibs:Depends=//p'
  )"
  if [[ -z "$dependencies" ]]; then
    echo "dpkg-shlibdeps did not determine runtime dependencies" >&2
    exit 1
  fi

  installed_size="$(du -sk "$stage" | awk '{print $1}')"
  cat > "$stage/DEBIAN/control" <<EOF
Package: singular
Version: $version-$package_release
Section: math
Priority: optional
Architecture: $architecture
Maintainer: Singular Team <singular@mathematik.uni-kl.de>
Installed-Size: $installed_size
Depends: $dependencies
Homepage: https://www.singular.uni-kl.de
Description: computer algebra system for polynomial computations
 Singular focuses on commutative and non-commutative algebra, algebraic
 geometry, and singularity theory.
EOF

  dpkg-deb --build --root-owner-group "$stage" \
    "$output_dir/$artifact_basename.deb"
else
  rpm_top="$RUNNER_TEMP/rpmbuild"
  mkdir -p "$rpm_top"/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
  find "$stage" -type f -name '*.la' -delete
  tar -C "$stage" -czf "$rpm_top/SOURCES/singular-payload.tar.gz" .
  (
    cd "$stage"
    find . \( -type f -o -type l \) -print \
      | LC_ALL=C sort \
      | sed 's|^\.||' \
      | awk '
          /^\/usr\/share\/licenses\/singular\// {
            print "%license " $0
            next
          }
          /^\/usr\/share\/man\/man[^/]+\// {
            directory = $0
            sub("/[^/]+$", "/*", directory)
            if (!seen[directory]++) print directory
            next
          }
          { print }
        ' \
      > "$rpm_top/SOURCES/files.list"
  )

  cat > "$rpm_top/SPECS/singular-binary.spec" <<EOF
Name: singular
Version: $version
Release: $package_release%{?dist}
Summary: Computer algebra system for polynomial computations
License: $package_license
URL: https://www.singular.uni-kl.de
Source0: singular-payload.tar.gz
Source1: files.list

%description
Singular is a computer algebra system with special emphasis on commutative
and non-commutative algebra, algebraic geometry, and singularity theory.

%prep

%build

%install
mkdir -p %{buildroot}
tar -C %{buildroot} -xzf %{SOURCE0}

%files -f %{SOURCE1}

%changelog
* Mon Jan 01 2024 Singular Team <singular@mathematik.uni-kl.de> - $version-$package_release
- Automated binary package
EOF

  rpmbuild -bb --define "_topdir $rpm_top" \
    "$rpm_top/SPECS/singular-binary.spec"
  built_rpm="$(find "$rpm_top/RPMS" -type f -name '*.rpm' -print -quit)"
  if [[ -z "$built_rpm" ]]; then
    echo "rpmbuild did not create a binary package" >&2
    exit 1
  fi
  cp "$built_rpm" "$output_dir/$artifact_basename.rpm"
fi
