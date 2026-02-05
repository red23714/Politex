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

    mkdir -p "$XDG_CONFIG_HOME" "$XDG_CACHE_HOME" "$XDG_DATA_HOME" projects
    cd projects

    if [ -n "$WAYLAND_DISPLAY" ]; then
        echo "🟢 Wayland session detected ($WAYLAND_DISPLAY)"
    elif [ -n "$DISPLAY" ]; then
        echo "🟡 X11 session detected ($DISPLAY)"
    else
        echo "🔴 No graphical session detected"
    fi

    echo "🚀 my-ide nix shell loaded"
    exec fish
  '';
}
