{
  description = "C snippet library";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-24.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachSystem flake-utils.lib.allSystems (system: let
      pkgs = import nixpkgs {
        inherit system;
      };
    in {
      packages = {
        default = pkgs.stdenv.mkDerivation {
          name = "csnip";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
          ];

          buildInputs = with pkgs; [ ];

          WORDLIST = "${pkgs.scowl}/share/dict/words.txt";
        };
      };
    });
}

# vim:sw=2:sts=2:et
