#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 3 ]]; then
  echo "usage: $0 SOURCE_ROOT NOTICE_DIR PLATFORM" >&2
  exit 2
fi

source_root="$(cd "$1" && pwd)"
notice_dir="$2"
platform="$3"
mkdir -p \
  "$notice_dir/Singular" \
  "$notice_dir/Singular/embedded-bsd" \
  "$notice_dir/runtime"

install -m 0644 "$source_root/COPYING" \
  "$notice_dir/Singular/COPYING"
install -m 0644 "$source_root/GPL2" \
  "$notice_dir/Singular/GPL-2.0.txt"
install -m 0644 "$source_root/GPL3" \
  "$notice_dir/Singular/GPL-3.0.txt"
install -m 0644 "$source_root/Singular/COPYING" \
  "$notice_dir/Singular/Singular-COPYING"
install -m 0644 "$source_root/kernel/COPYING" \
  "$notice_dir/Singular/kernel-COPYING"
install -m 0644 "$source_root/libpolys/COPYING" \
  "$notice_dir/Singular/libpolys-COPYING"
install -m 0644 "$source_root/factory/COPYING" \
  "$notice_dir/Singular/Factory-COPYING"
install -m 0644 "$source_root/omalloc/COPYING" \
  "$notice_dir/Singular/omalloc-COPYING"
install -m 0644 "$source_root/Singular/LIB/COPYING" \
  "$notice_dir/Singular/LIB-COPYING"
install -m 0644 "$source_root/doc/COPYING.texi" \
  "$notice_dir/Singular/COPYING.texi"

# These notices must accompany binary distributions; the top-level Singular
# notice names the components but does not reproduce their complete terms.
install -m 0644 "$source_root/Singular/links/ndbm.h" \
  "$notice_dir/Singular/embedded-bsd/ndbm.h"
install -m 0644 "$source_root/Singular/svd/license" \
  "$notice_dir/Singular/embedded-bsd/svd-license-index.txt"
for file in \
  Singular/svd/bdsvd.h \
  Singular/svd/bidiagonal.h \
  Singular/svd/blas.h \
  Singular/svd/lq.h \
  Singular/svd/qr.h \
  Singular/svd/reflections.h \
  Singular/svd/rotations.h \
  Singular/svd/svd.h \
  Singular/svd_si.h
do
  install -m 0644 "$source_root/$file" \
    "$notice_dir/Singular/embedded-bsd/$(basename "$file")"
done

if [[ -n "${RUNTIME_PACKAGE_MANIFEST:-}" && -f "$RUNTIME_PACKAGE_MANIFEST" ]]; then
  install -m 0644 "$RUNTIME_PACKAGE_MANIFEST" \
    "$notice_dir/runtime/packages.txt"
fi

source_commit="$(git -C "$source_root" rev-parse HEAD)"
source_url="${GITHUB_SERVER_URL:-https://github.com}/${GITHUB_REPOSITORY:-Singular/Singular}/tree/$source_commit"
package_license="(GPL-2.0-only OR GPL-3.0-only) AND BSD-3-Clause"

cat > "$notice_dir/COMPONENTS.txt" <<EOF
Singular binary package component manifest

Platform: $platform
Singular source commit: $source_commit
Singular source: $source_url
Package license expression: $package_license

Runtime package versions and any bundled runtime licenses are recorded below runtime/.
EOF
