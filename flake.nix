{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in
    {
      devShells.x86_64-linux.default = pkgs.mkShell {
        packages = with pkgs; [
          clang
          gnumake
        ];

        buildInputs = with pkgs; [
          libGL
          extra-cmake-modules
          wayland
          wayland-protocols
          libxkbcommon
          libffi
          xorg.libX11
          xorg.libXinerama
          xorg.libXi
          xorg.libXcursor
          xorg.libXrandr
        ];
      };
    };
}
