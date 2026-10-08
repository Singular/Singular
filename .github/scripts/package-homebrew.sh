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
tap=singular/package-build
build_dependency_bottles="${BUILD_MISSING_DEPENDENCY_BOTTLES:-true}"
enable_sispasm="${ENABLE_SISPASM:-false}"
formula_name=singular
template="$source_root/.github/packaging/homebrew/singular.rb.in"
handled_source_dependencies=

case "$build_dependency_bottles" in
  true|false) ;;
  *)
    echo "BUILD_MISSING_DEPENDENCY_BOTTLES must be true or false" >&2
    exit 2
    ;;
esac
case "$enable_sispasm" in
  true)
    formula_name=singular-sispasm
    template="$source_root/.github/packaging/homebrew/singular-sispasm.rb.in"
    handled_source_dependencies=spasm
    ;;
  false) ;;
  *)
    echo "ENABLE_SISPASM must be true or false" >&2
    exit 2
    ;;
esac

escape_sed_replacement() {
  printf '%s' "$1" | sed 's/[\\&|]/\\&/g'
}

rm -rf "$output_dir"
mkdir -p "$output_dir"

formula="$output_dir/$formula_name.rb"
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
cp "$formula" "$tap_root/Formula/$formula_name.rb"

if [[ "$enable_sispasm" == true ]]; then
  cp "$source_root/.github/packaging/homebrew/spasm.rb.in" "$output_dir/spasm.rb"
  cp "$output_dir/spasm.rb" "$tap_root/Formula/spasm.rb"
  brew install --only-dependencies "$tap/spasm"
  brew install --build-bottle "$tap/spasm"
  brew test "$tap/spasm"
  (
    cd "$output_dir"
    brew bottle --json --root-url="$root_url" "$tap/spasm"
  )
fi

# Homebrew refuses --build-bottle if a dependency still needs to be built from
# source. Install dependencies first so Intel runners can use source-only
# formulae such as current Automake and Readline.
brew install --only-dependencies "$tap/$formula_name"
brew install --build-bottle "$tap/$formula_name"
brew test "$tap/$formula_name"

dependencies="$(brew deps --union "$tap/$formula_name")"
# Formula names contain no whitespace. Passing the dependency list as separate
# arguments lets Homebrew emit one installation record for the whole build.
# shellcheck disable=SC2086
brew info --json=v2 "$tap/$formula_name" $dependencies \
  > "$output_dir/homebrew-installations.json"
FORMULA_NAME="$formula_name" \
HANDLED_SOURCE_DEPENDENCIES="$handled_source_dependencies" \
ruby -rjson -e '
  formula_name = ENV.fetch("FORMULA_NAME")
  handled = ENV.fetch("HANDLED_SOURCE_DEPENDENCIES", "").split(":")
  data = JSON.parse(File.read(ARGV.fetch(0)))
  source_dependencies = []
  data.fetch("formulae").sort_by { |formula| formula.fetch("name") }.each do |formula|
    installation = formula.fetch("installed").last
    source = installation.fetch("poured_from_bottle", false) ? "bottle" : "source"
    puts "#{formula.fetch("name")}\t#{installation.fetch("version")}\t#{source}"
    if formula.fetch("name") != formula_name && !handled.include?(formula.fetch("name")) && source == "source"
      source_dependencies << formula.fetch("full_name")
    end
  end
  File.write(ARGV.fetch(1), source_dependencies.join("\n") + (source_dependencies.empty? ? "" : "\n"))
' "$output_dir/homebrew-installations.json" \
  "$output_dir/source-built-dependencies.txt" \
  > "$output_dir/homebrew-installations.txt"

if [[ -n "${GITHUB_STEP_SUMMARY:-}" ]]; then
  {
    echo "### Homebrew installation sources ($formula_name, $architecture)"
    echo
    echo '```text'
    cat "$output_dir/homebrew-installations.txt"
    echo '```'
  } >> "$GITHUB_STEP_SUMMARY"
fi

(
  cd "$output_dir"
  brew bottle --json --root-url="$root_url" "$tap/$formula_name"
)

if [[ "$build_dependency_bottles" == true && \
      -s "$output_dir/source-built-dependencies.txt" ]]; then
  dependency_dir="$output_dir/dependency-bottles"
  mkdir -p "$dependency_dir/formulae"
  while IFS= read -r dependency; do
    test -n "$dependency"
    formula_path="$(brew formula "$dependency")"
    cp "$formula_path" "$dependency_dir/formulae/$(basename "$formula_path")"

    brew uninstall --ignore-dependencies "$dependency"
    brew install --build-bottle --as-dependency "$dependency"
    (
      cd "$dependency_dir"
      brew bottle --force-core-tap --json \
        --root-url="$root_url" "$dependency"
    )
  done < "$output_dir/source-built-dependencies.txt"
  brew test "$tap/$formula_name"
fi

bottle="$(find "$output_dir" -maxdepth 1 -type f -name "$formula_name--*.bottle*.tar.gz" -print -quit)"
bottle_json="$(find "$output_dir" -maxdepth 1 -type f -name "$formula_name--*.bottle.json" -print -quit)"
test -n "$bottle"
test -n "$bottle_json"
tar -tzf "$bottle" > "$output_dir/bottle-contents.txt"
grep -Eq '/bin/Singular$' "$output_dir/bottle-contents.txt"

cat > "$output_dir/package-source.txt" <<EOF
repository=$GITHUB_REPOSITORY
commit=$GITHUB_SHA
version=$version
architecture=$architecture
formula=$formula_name
sispasm=$enable_sispasm
source_url=$source_url
source_sha256=$source_sha256
bottle_root_url=$root_url
build_missing_dependency_bottles=$build_dependency_bottles
EOF
