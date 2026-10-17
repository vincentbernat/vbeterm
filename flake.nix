{
  inputs = {
    nixpkgs.url = "nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, flake-utils, ... }@inputs:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import inputs.nixpkgs {
          inherit system;
        };
        vte = pkgs.vte.overrideAttrs (old: {
          patches = (old.patches or [ ]) ++ [ ./nix/vte-hyperlink-underline.patch ];
        });
      in
      {
        packages.default = pkgs.stdenv.mkDerivation rec {
          name = "vbeterm";
          src = ./.;
          nativeBuildInputs = with pkgs; [ autoreconfHook pkg-config ];
          buildInputs = [ vte ];
          postInstall = ''
            mv $out/bin/term $out/bin/${name}
          '';
        };
        devShells.default = pkgs.mkShell {
          name = "vbeterm-dev";
          buildInputs =
            self.packages."${system}".default.nativeBuildInputs ++
            self.packages."${system}".default.buildInputs;
        };
      });
}
