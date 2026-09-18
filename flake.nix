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
        "AC_INIT\\(\\[singular\\], \\[([^]]+)\\],.*"
        configureLine;
      sourceVersion = builtins.elemAt versionMatch 0;
      sourceRevision = if self ? shortRev then self.shortRev else "dirty";
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
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
        in
        {
          inherit singular;
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
