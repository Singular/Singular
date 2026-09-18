#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 7 ]]; then
  echo "usage: $0 SOURCE_ROOT VERSION SOURCE_URL SOURCE_SHA256 ROOT_URL OUTPUT_DIR ARCHITECTURE" >&2
  exit 2
fi

source_root="$(cd "$1" && pwd)"
version="$2"
source_url="$3"
source_sha256="$4"
root_url="$5"
output_dir="$6"
architecture="$7"
template="$source_root/.github/packaging/homebrew/singular.rb.in"
tap=singular/package-build

escape_sed_replacement() {
  printf '%s' "$1" | sed 's/[\\&|]/\\&/g'
}

rm -rf "$output_dir"
mkdir -p "$output_dir"

formula="$output_dir/singular.rb"
sed \
  -e "s|@SOURCE_URL@|$(escape_sed_replacement "$source_url")|g" \
  -e "s|@VERSION@|$(escape_sed_replacement "$version")|g" \
  -e "s|@SOURCE_SHA256@|$(escape_sed_replacement "$source_sha256")|g" \
  "$template" > "$formula"

git config --global user.name "Singular package workflow"
git config --global user.email "singular@mathematik.uni-kl.de"
brew tap-new "$tap"
tap_root="$(brew --repository "$tap")"
mkdir -p "$tap_root/Formula"
cp "$formula" "$tap_root/Formula/singular.rb"

brew install --build-bottle "$tap/singular"
brew test "$tap/singular"

(
  cd "$output_dir"
  brew bottle --json --root-url="$root_url" "$tap/singular"
)

bottle="$(find "$output_dir" -maxdepth 1 -type f -name '*.bottle*.tar.gz' -print -quit)"
bottle_json="$(find "$output_dir" -maxdepth 1 -type f -name '*.bottle.json' -print -quit)"
test -n "$bottle"
test -n "$bottle_json"
tar -tzf "$bottle" > "$output_dir/bottle-contents.txt"
grep -Eq '/bin/Singular$' "$output_dir/bottle-contents.txt"

cat > "$output_dir/package-source.txt" <<EOF
repository=$GITHUB_REPOSITORY
commit=$GITHUB_SHA
version=$version
architecture=$architecture
source_url=$source_url
source_sha256=$source_sha256
bottle_root_url=$root_url
EOF
