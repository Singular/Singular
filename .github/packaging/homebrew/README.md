# Publishing the Homebrew bottles

Run the **Homebrew bottles** workflow on the release tag. It builds Singular on
each selected architecture and uploads the bottle, its JSON metadata, the
rendered formula, and an installation-source report.

Homebrew bottles do not contain their dependencies. If a runtime dependency
has no usable Homebrew bottle, the workflow rebuilds it with `--build-bottle`
and places its bottle, JSON metadata, and formula snapshot under
`dependency-bottles/`. This is based on the actual installation receipts on
the runner, not a hard-coded dependency list.

Upload all bottle archives to the `bottle_root_url` used for the workflow. In
the tap checkout, merge the Singular JSON files into `Formula/singular.rb`:

```sh
brew bottle --merge --write --no-commit /path/to/singular--*.bottle.json
```

The generated dependency bottles can be installed directly before Singular:

```sh
brew install /path/to/dependency--*.bottle.tar.gz
brew install Singular/singular/singular
```

For automatic dependency selection, maintain the corresponding formula copy
in the tap, merge its JSON files into that copy, and make the Singular formula
depend on the fully qualified tap formula. That turns the dependency into tap
maintenance; do it only for dependencies that remain unbottled upstream.
