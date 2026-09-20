#!/usr/bin/env bash

set -euo pipefail

repo_root="${GITHUB_WORKSPACE:-$PWD}"
doc_dir="$repo_root/doc"

documentation_exclusions() {
  sed -n \
    's/^DOC2TEX_EXAMPLE_EXCLUSIONS = //p' \
    "$doc_dir/Makefile-docbuild" | xargs
}

case "${1:-}" in
  --print-exclusions)
    documentation_exclusions
    ;;
  ""|--build)
    exclusions="$(documentation_exclusions)"
    echo "::warning::Old documentation excludes examples: $exclusions"
    make -C "$doc_dir" -j1 -f Makefile-docbuild \
      DOC2TEX_EXAMPLE_EXCLUSIONS="$exclusions" \
      singular.info html singular.idx
    chmod -R a+rX "$doc_dir/singular.info" "$doc_dir/html"
    cp "$doc_dir/singular.info" "$doc_dir/singular.idx" "$doc_dir/html"
    (
      cd "$doc_dir/html"
      tar jcf ../doc.tbz2 *
    )
    test -s "$doc_dir/doc.tbz2"
    ls -lh "$doc_dir/doc.tbz2"
    ;;
  *)
    echo "usage: $0 [--build|--print-exclusions]" >&2
    exit 2
    ;;
esac
