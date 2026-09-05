#!/bin/bash

clang++ -S -emit-llvm MyString.cpp
opt -passes=dot-callgraph -disable-output MyString.ll
