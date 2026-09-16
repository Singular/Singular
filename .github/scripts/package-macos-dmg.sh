#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 5 ]]; then
  echo "usage: $0 SOURCE_ROOT DISPLAY_VERSION BUNDLE_VERSION ARCH_LABEL OUTPUT_DMG" >&2
  exit 2
fi

source_root="$(cd "$1" && pwd)"
display_version="$2"
bundle_version="$3"
arch_label="$4"
output_dmg="$5"
short_version="${display_version%%p*}"
short_version="${short_version%%-*}"
app="/Applications/Singular.app"
contents="$app/Contents"

if [[ "${CI:-false}" != true ]]; then
  echo "Refusing to stage a package below /Applications outside CI" >&2
  exit 1
fi
if [[ -e "$app" ]]; then
  echo "$app already exists on this runner" >&2
  exit 1
fi

sudo mkdir -p "$contents"
sudo chown -R "$(id -u):$(id -g)" "$app"

configure_flags=(
  "--prefix=$contents"
  --enable-gfanlib
  --with-ntl=yes
  --with-flint=yes
  --without-python
  --disable-python
)

build_dir="$RUNNER_TEMP/singular-build-macos"
mkdir -p "$build_dir"
(
  cd "$source_root"
  ./autogen.sh
)
(
  cd "$build_dir"
  "$source_root/configure" "${configure_flags[@]}"
  make -j"${BUILD_JOBS:-3}"
  make install
)

mkdir -p "$contents/MacOS" "$contents/Resources"
cat > "$contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleDisplayName</key><string>Singular</string>
  <key>CFBundleExecutable</key><string>Singular</string>
  <key>CFBundleIdentifier</key><string>org.singular.singular</string>
  <key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
  <key>CFBundleName</key><string>Singular</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>CFBundleShortVersionString</key><string>$short_version</string>
  <key>CFBundleVersion</key><string>$bundle_version</string>
  <key>LSMinimumSystemVersion</key><string>12.0</string>
</dict>
</plist>
EOF
printf 'APPL????' > "$contents/PkgInfo"
cat > "$contents/MacOS/Singular" <<'EOF'
#!/bin/sh
contents="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
singular="$contents/bin/Singular"
if [ -t 0 ] || [ -t 1 ]; then
  exec "$singular" "$@"
fi
exec osascript - "$singular" <<'APPLESCRIPT'
on run argv
  tell application "Terminal"
    activate
    do script quoted form of item 1 of argv
  end tell
end run
APPLESCRIPT
EOF
chmod 0755 "$contents/MacOS/Singular"

notice_dir="$contents/Resources/THIRD_PARTY_LICENSES"
"$source_root/.github/scripts/bundle-macos-runtime.py" \
  "$app" "$notice_dir/runtime/homebrew"
export RUNTIME_PACKAGE_MANIFEST="$notice_dir/runtime/homebrew/homebrew-runtime-packages.txt"
"$source_root/.github/scripts/collect-binary-notices.sh" \
  "$source_root" "$notice_dir" "macOS $arch_label"

codesign --force --sign - --timestamp=none "$app"
codesign --verify --deep --strict "$app"

singular="$contents/bin/Singular"
actual_arch="$(lipo -archs "$singular")"
if [[ "$arch_label" == x86_64 && "$actual_arch" != *x86_64* ]]; then
  echo "Intel package contains $actual_arch Singular" >&2
  exit 1
fi
if [[ "$arch_label" == arm64 && "$actual_arch" != *arm64* ]]; then
  echo "Apple Silicon package contains $actual_arch Singular" >&2
  exit 1
fi

smoke_output="$(printf '%s\n' \
  'ring r=0,(x,y),dp;' \
  'ideal i=x^2,y^2;' \
  'size(std(i));' \
  'print("BINARY_PACKAGE_SMOKE_OK");' \
  'quit;' | "$singular" -q 2>&1)"
printf '%s\n' "$smoke_output"
grep -q BINARY_PACKAGE_SMOKE_OK <<< "$smoke_output"

dmg_root="$RUNNER_TEMP/singular-dmg-root"
mkdir -p "$dmg_root"
ditto "$app" "$dmg_root/Singular.app"
ln -s /Applications "$dmg_root/Applications"
cat > "$dmg_root/README.txt" <<EOF
Drag Singular.app to Applications.

Command-line executable:
  /Applications/Singular.app/Contents/bin/Singular

Third-party notices:
  /Applications/Singular.app/Contents/Resources/THIRD_PARTY_LICENSES
EOF

mkdir -p "$(dirname "$output_dmg")"
hdiutil create \
  -volname "Singular $display_version" \
  -srcfolder "$dmg_root" \
  -format UDZO \
  -ov \
  "$output_dmg"
hdiutil verify "$output_dmg"
