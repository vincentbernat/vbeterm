{
  inputs = {
    nixpkgs.url = "nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, flake-utils, ... }@inputs:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = inputs.nixpkgs.legacyPackages."${system}";
      in
      {
        packages.default = pkgs.stdenv.mkDerivation rec {
          name = "vbeterm";
          src = ./.;
          nativeBuildInputs = with pkgs; [ autoreconfHook pkgconfig ];
          buildInputs = [ pkgs.vte ];
          postInstall = ''
            mv $out/bin/term $out/bin/${name}
          '';
        };
      });
}
