#!/usr/bin/env python3

import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile


SYSTEM_PREFIXES = ("/System/Library/", "/usr/lib/")
LICENSE_NAMES = re.compile(r"^(copying|licen[cs]e|notice|copyright)", re.I)


def run(*args: str, capture: bool = False) -> str:
    result = subprocess.run(
        args,
        check=True,
        text=True,
        stdout=subprocess.PIPE if capture else None,
    )
    return result.stdout.strip() if capture else ""


def is_macho(path: Path) -> bool:
    if not path.is_file() or path.is_symlink():
        return False
    description = run("file", "-b", str(path), capture=True)
    return "Mach-O" in description


def install_name(path: Path) -> str | None:
    result = subprocess.run(
        ("otool", "-D", str(path)),
        check=False,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
    )
    lines = result.stdout.splitlines()
    return lines[1].strip() if result.returncode == 0 and len(lines) > 1 else None


def dependencies(path: Path) -> list[str]:
    lines = run("otool", "-L", str(path), capture=True).splitlines()[1:]
    result = [line.strip().split(" (", 1)[0] for line in lines if " (" in line]
    own_name = install_name(path)
    return [dependency for dependency in result if dependency != own_name]


def rpaths(path: Path) -> list[str]:
    lines = run("otool", "-l", str(path), capture=True).splitlines()
    result = []
    for index, line in enumerate(lines):
        if line.strip() != "cmd LC_RPATH":
            continue
        for detail in lines[index + 1 : index + 5]:
            match = re.match(r"\s*path (.+) \(offset \d+\)$", detail)
            if match:
                result.append(match.group(1))
                break
    return result


def anchored_paths(value: str, binary: Path, app: Path) -> list[Path]:
    if value == "@loader_path":
        return [binary.parent]
    if value.startswith("@loader_path/"):
        return [binary.parent / value.removeprefix("@loader_path/")]
    if value == "@executable_path":
        suffix = ""
    elif value.startswith("@executable_path/"):
        suffix = value.removeprefix("@executable_path/")
    else:
        return [Path(value)] if not value.startswith("@") else []

    return [
        binary.parent / suffix,
        app / "Contents" / "bin" / suffix,
        app / "Contents" / "MacOS" / suffix,
    ]


def dependency_candidates(dependency: str, binary: Path, app: Path) -> list[Path]:
    if dependency.startswith("@rpath/"):
        suffix = dependency.removeprefix("@rpath/")
        candidates = []
        for search_path in rpaths(binary):
            for expanded in anchored_paths(search_path, binary, app):
                candidates.append(expanded / suffix)
        return candidates
    return anchored_paths(dependency, binary, app)


def first_existing(paths: list[Path]) -> Path | None:
    return next((path for path in paths if path.exists()), None)


def loader_reference(binary: Path, target: Path) -> str:
    relative = os.path.relpath(target, binary.parent)
    return "@loader_path" if relative == "." else f"@loader_path/{relative}"


def formula_for(path: Path, cellar: Path) -> tuple[str, str] | None:
    try:
        relative = path.resolve().relative_to(cellar.resolve())
    except ValueError:
        return None
    if len(relative.parts) < 3:
        return None
    return relative.parts[0], relative.parts[1]


def collect_formula_licenses(
    formulas: dict[str, str], license_dir: Path, manifest: Path
) -> None:
    lines = []
    for formula in sorted(formulas):
        prefix = Path(run("brew", "--prefix", formula, capture=True))
        info = json.loads(run("brew", "info", "--json=v2", formula, capture=True))
        metadata = info["formulae"][0]
        version = formulas[formula]
        spdx = metadata.get("license") or "unknown"
        destination = license_dir / formula
        copied = 0

        def copy_licenses(root: Path) -> int:
            found = 0
            for candidate in root.rglob("*"):
                if not candidate.is_file() or len(candidate.relative_to(root).parts) > 5:
                    continue
                if not LICENSE_NAMES.match(candidate.name):
                    continue
                relative = candidate.relative_to(root)
                target = destination / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(candidate, target)
                found += 1
            return found

        copied += copy_licenses(prefix)
        if copied == 0:
            # Homebrew bottles commonly omit upstream license files. Fetch the
            # exact formula source archive so the bundled dylib keeps its full
            # upstream notice rather than only Homebrew's SPDX metadata.
            with tempfile.TemporaryDirectory(prefix=f"singular-{formula}-") as temp:
                unpack_dir = Path(temp)
                run(
                    "brew",
                    "unpack",
                    "--destdir",
                    str(unpack_dir),
                    "--force",
                    formula,
                )
                for source_root in unpack_dir.iterdir():
                    if source_root.is_dir():
                        copied += copy_licenses(source_root)

        if copied == 0:
            raise RuntimeError(f"no license file found for Homebrew formula {formula}")

        stable = metadata.get("urls", {}).get("stable", {})
        source_url = stable.get("url", "unknown")
        checksum = stable.get("checksum", "unknown")
        lines.append(
            f"{formula} {version} (license metadata: {spdx}; "
            f"source: {source_url}; sha256: {checksum})"
        )

    manifest.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main() -> int:
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} APP_DIR RUNTIME_LICENSE_DIR", file=sys.stderr)
        return 2

    app = Path(sys.argv[1]).resolve()
    license_dir = Path(sys.argv[2]).resolve()
    contents = app / "Contents"
    bundle_lib = contents / "lib"
    bundle_lib.mkdir(parents=True, exist_ok=True)
    license_dir.mkdir(parents=True, exist_ok=True)
    cellar = Path(run("brew", "--cellar", capture=True))

    queue = [path for path in contents.rglob("*") if is_macho(path)]
    inspected: set[Path] = set()
    copied_sources: dict[Path, Path] = {}
    formulas: dict[str, str] = {}

    while queue:
        binary = queue.pop(0)
        if binary in inspected:
            continue
        inspected.add(binary)

        for dependency in dependencies(binary):
            if dependency.startswith(SYSTEM_PREFIXES):
                continue
            source = first_existing(dependency_candidates(dependency, binary, app))
            if source is None:
                raise RuntimeError(f"unresolved dependency {dependency} required by {binary}")
            source = source.resolve()
            try:
                source.relative_to(app)
                queue.append(source)
                continue
            except ValueError:
                pass

            formula = formula_for(source, cellar)
            if formula is None:
                raise RuntimeError(
                    f"external dependency is not owned by Homebrew: {dependency}"
                )
            formulas[formula[0]] = formula[1]

            destination = bundle_lib / Path(dependency).name
            if destination.exists():
                if destination.resolve() != source and destination.read_bytes() != source.read_bytes():
                    raise RuntimeError(f"conflicting bundled libraries named {destination.name}")
            else:
                shutil.copy2(source, destination)
            copied_sources[destination] = source
            queue.append(destination)

    copied_targets = {
        source.resolve(): destination
        for destination, source in copied_sources.items()
    }
    macho_files = [path for path in contents.rglob("*") if is_macho(path)]

    for binary in macho_files:
        for dependency in dependencies(binary):
            if dependency.startswith(SYSTEM_PREFIXES):
                continue
            source = first_existing(dependency_candidates(dependency, binary, app))
            if source is None:
                raise RuntimeError(
                    f"unresolved dependency {dependency} required by {binary}"
                )
            source = source.resolve()
            target = copied_targets.get(source, source).resolve()
            try:
                target.relative_to(app)
            except ValueError as error:
                raise RuntimeError(
                    f"external dependency was not bundled: {dependency}"
                ) from error

            replacement = loader_reference(binary, target)
            if dependency != replacement:
                run("install_name_tool", "-change", dependency, replacement, str(binary))

        if install_name(binary) is not None:
            run(
                "install_name_tool",
                "-id",
                f"@loader_path/{binary.name}",
                str(binary),
            )

        # All non-system dependencies now use @loader_path, so inherited
        # Homebrew and build-tree search paths are neither needed nor safe.
        for search_path in dict.fromkeys(rpaths(binary)):
            run("install_name_tool", "-delete_rpath", search_path, str(binary))

    # Re-scan after rewriting and reject any dependency on the CI Homebrew tree.
    unresolved = []
    macho_files = [path for path in contents.rglob("*") if is_macho(path)]
    for binary in macho_files:
        for search_path in rpaths(binary):
            unresolved.append(f"{binary}: retained rpath {search_path}")
        for dependency in dependencies(binary):
            if dependency.startswith(SYSTEM_PREFIXES):
                continue
            if dependency.startswith(("@loader_path/", "@executable_path/", "@rpath/")):
                candidates = dependency_candidates(dependency, binary, app)
                target = first_existing(candidates)
                if target is None:
                    unresolved.append(f"{binary}: unresolved {dependency}")
                    continue
                try:
                    target.resolve().relative_to(app)
                except ValueError:
                    unresolved.append(f"{binary}: external {dependency} -> {target}")
                continue
            unresolved.append(f"{binary}: external {dependency}")

    if unresolved:
        raise RuntimeError("non-portable Mach-O dependencies:\n" + "\n".join(unresolved))

    # install_name_tool invalidates existing bottle signatures. Ad-hoc signing
    # keeps arm64 binaries loadable; the enclosing app is signed after any FIPS
    # module integrity data has been regenerated.
    for binary in sorted(macho_files, key=lambda path: len(path.parts), reverse=True):
        run("codesign", "--force", "--sign", "-", "--timestamp=none", str(binary))

    manifest = license_dir / "homebrew-runtime-packages.txt"
    collect_formula_licenses(formulas, license_dir, manifest)
    print(f"Bundled {len(copied_sources)} Homebrew dylibs from {len(formulas)} formulae")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (RuntimeError, subprocess.CalledProcessError) as error:
        print(error, file=sys.stderr)
        raise SystemExit(1)
