{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  name = "my-ide";

  packages = with pkgs; [
    fish
    neovim
    zellij
    zoxide

    zig
    llvmPackages_latest.clang
    llvmPackages_latest.clang-tools
    gcc

    python312
    nodejs_20
    typst

    git
    openssh
    ripgrep
    fd
    btop
    htop
    wl-clipboard

    curl
    wget
    unzip
    gnutar

    qemu
    firefox

    zbar
    qrencode
    time
  ];

  shellHook = ''
    export IDE_ROOT="$PWD"

    export XDG_CONFIG_HOME="$IDE_ROOT/.config"
    export XDG_CACHE_HOME="$IDE_ROOT/.cache"
    export XDG_DATA_HOME="$IDE_ROOT/.share"

    export _ZO_DATA_DIR="$XDG_DATA_HOME/zoxide"

    export NPM_CONFIG_CACHE="$XDG_CACHE_HOME/npm"
    export PIP_CACHE_DIR="$XDG_CACHE_HOME/pip"

    export BROWSER=firefox

    mkdir -p "$XDG_CONFIG_HOME" "$XDG_CACHE_HOME" "$XDG_DATA_HOME" projects
    cd projects

    
    if [ "$XDG_SESSION_DESKTOP" = "Hyprland" ]; then
        export XDG_RUNTIME_DIR=/run/user/1000
        export WAYLAND_DISPLAY=wayland-1
        echo "🟢 Hyprland detected — using hardcoded Wayland values"
    else
        echo "🟢 Non-Hyprland session — inheriting env"
    fi

    echo "🚀 my-ide nix shell loaded"
    exec fish
  '';
}
