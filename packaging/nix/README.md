# Publishing the Nix package

Run the **Nix packages** workflow on the release tag and select the desired
systems. The workflow builds and tests the flake, then exports one binary-cache
archive per system.

## Signing key

Create the cache key once, off GitHub:

```sh
nix-store --generate-binary-cache-key \
  cache.singular.uni-kl.de-1 singular-nix-private-key singular-nix-public-key
```

Store the contents of `singular-nix-private-key` in the repository Actions
secret `NIX_CACHE_PRIVATE_KEY`. Keep the private-key file offline. The workflow
includes only the public key in its artifacts. Runs without the secret are
marked `signed=false`; use those for testing, not as a public cache.

## Publish

1. Download all `singular-nix-*` workflow artifacts and extract their
   `singular-nix-cache-*.tar.gz` files.
2. Merge the extracted `nix-cache` directories. Their content-addressed file
   names can coexist; identical files must be byte-for-byte identical.
3. Upload the merged directory unchanged to a stable HTTPS location. Its root
   must contain `nix-cache-info`, the `.narinfo` files, and the `nar/`
   directory.
4. Publish `singular-nix-public-key.txt` next to the cache and retain it for
   future releases that use the same signing key.

Users can install the tagged source while taking binaries from that cache:

```sh
nix profile install github:Singular/Singular/RELEASE_TAG#singular \
  --extra-substituters https://cache.singular.uni-kl.de/nix \
  --extra-trusted-public-keys "$(cat singular-nix-public-key.txt)"
```

Test the hosted cache from a machine that has no local Singular result before
announcing it. Do not use `--no-check-sigs` for the published instructions.
