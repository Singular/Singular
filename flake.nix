{
  description = "Singular computer algebra system";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";

  outputs = { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
      forAllSystems = nixpkgs.lib.genAttrs systems;
      configureLine = builtins.head (
        nixpkgs.lib.splitString "\n" (builtins.readFile ./configure.ac)
      );
      versionMatch = builtins.match
        "AC_INIT[(][[]singular[]], [[]([0-9A-Za-z.p]+)[]],.*"
        configureLine;
      sourceVersion = builtins.elemAt versionMatch 0;
      sourceRevision = if self ? shortRev then self.shortRev else "dirty";
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
          spasm = pkgs.stdenv.mkDerivation {
            pname = "spasm";
            version = "1.2";

            src = pkgs.fetchurl {
              url = "https://github.com/cbouilla/spasm/archive/refs/tags/v1.2.tar.gz";
              sha256 = "e01947316c177ac2084a4fe587e06f33377a196f711613e9d4a7b1b1c7cec000";
            };

            nativeBuildInputs = [
              pkgs.autoconf
              pkgs.automake
              pkgs.libtool
            ];

            configurePhase = ''
              runHook preConfigure
              autoreconf -fi
              ./configure --prefix=$out --disable-openmp
              runHook postConfigure
            '';

            buildPhase = ''
              runHook preBuild
              make -C src
              runHook postBuild
            '';

            installPhase = ''
              runHook preInstall
              make -C src install
              runHook postInstall
            '';

            doCheck = false;
          };
          singular = (pkgs.singular.override { enableDocs = false; }).overrideAttrs (_: {
            version = "${sourceVersion}-git.${sourceRevision}";
            src = self;
            patches = [ ];
            configureFlags = [
              "--enable-gfanlib"
              "--with-ntl=${pkgs.ntl}"
              "--with-flint=${pkgs.flint}"
              "--without-python"
              "--disable-python"
            ];
          });
          singular-sispasm = singular.overrideAttrs (old: {
            buildInputs = (old.buildInputs or [ ]) ++ [ spasm ];
            configureFlags = (old.configureFlags or [ ]) ++ [
              "--enable-sispasm-module"
            ];
          });
        in
        {
          inherit singular singular-sispasm spasm;
          default = singular;
        });

      apps = forAllSystems (system: {
        singular = {
          type = "app";
          program = "${self.packages.${system}.singular}/bin/Singular";
        };
        default = self.apps.${system}.singular;
      });
    };
}
