#!/bin/bash

if [ `id -u` -ne "0" ]; then
	echo "[!] Run as root"
	exit 1
fi

apt install clang graphviz
