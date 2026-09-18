# Publishing the Spack package

The `spack-repo-index.yaml` file makes this Git repository directly usable as
a Spack package repository. The **Spack packages** workflow builds the selected
Linux architectures. It uses Spack's public cache first, builds cache misses
from source, and exports Singular with its runtime/link dependency closure.

## Signing key

Create a key once, off GitHub, using the same Spack release as the workflow:

```sh
spack gpg create \
  --export singular-spack-public-key.gpg \
  --export-secret singular-spack-private-key.gpg \
  "Singular build cache" singular@mathematik.uni-kl.de
base64 < singular-spack-private-key.gpg | tr -d '\n'
```

Store the final base64 line in the repository Actions secret
`SPACK_SIGNING_KEY`. Keep the private key offline. Runs without this secret are
marked `signed=false` and are for testing only.

## Publish the cache

1. Download and extract all `singular-spack-*` workflow artifacts.
2. Merge their `spack-cache` directories into one directory.
3. With the public key trusted locally, regenerate the merged key and package
   indexes:

   ```sh
   spack gpg trust -y singular-spack-public-key.gpg
   spack gpg publish -d /path/to/spack-cache --update-index
   spack buildcache update-index /path/to/spack-cache
   ```

4. Upload the complete cache directory unchanged to a stable HTTPS location.

Users can then configure the recipe and signed cache:

```sh
spack repo add --name singular https://github.com/Singular/Singular.git
spack mirror add --signed singular https://www.singular.uni-kl.de/spack-cache
spack buildcache keys --install --trust -y singular
spack install --use-buildcache only singular@4.4.1p5
```

Before proposing the recipe to `spack/spack-packages`, add maintainers, update
the release versions and checksums, run `spack style`, and build each supported
architecture. Once it is upstream, this in-tree repository can remain as the
place to validate unreleased Singular commits.
