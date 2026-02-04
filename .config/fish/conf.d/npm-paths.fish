#!/usr/bin/env fish

# Add npm global bin to PATH if it exists
if test -d $HOME/.npm-global/bin
    fish_add_path $HOME/.npm-global/bin
end

# Add other common paths that might contain npm-installed binaries
if test -d $HOME/.yarn/bin
    fish_add_path $HOME/.yarn/bin
end

if test -d $HOME/.local/share/pnpm
    fish_add_path $HOME/.local/share/pnpm
end

if test -d $HOME/.bun/bin
    fish_add_path $HOME/.bun/bin
end