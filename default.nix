{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  name = "vbeterm";
  src = pkgs.nix-gitignore.gitignoreSource [] ./.;
  nativeBuildInputs = [ pkgs.autoreconfHook pkgs.pkgconfig ];
  buildInputs = [ pkgs.vte ];
}
