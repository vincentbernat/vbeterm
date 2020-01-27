{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  name = "vbeterm";
  src = ./.;
  nativeBuildInputs = [ pkgs.pkgconfig ];
  buildInputs = [ pkgs.vte ];
}
