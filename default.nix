{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation {
  name = "TinyWebsockets";

  src = ./. ;

  buildInputs = with pkgs; [
    catch2
    cmake
    openssl
  ];

}

