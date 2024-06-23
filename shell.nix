{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs.buildPackages; [
      wget
      gcc
      gdb
      gnumake
      cmake
      gnutar
      unzip
      zip
      curl
      jq
      python3
      zstd
      libpulseaudio
      pkgconf
    ];

}
