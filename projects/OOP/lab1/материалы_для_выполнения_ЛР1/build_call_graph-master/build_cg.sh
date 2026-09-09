#!/bin/bash

clang++ -S -emit-llvm ../../src/my_string/my_string.cpp
opt -passes=dot-callgraph -disable-output my_string.ll
