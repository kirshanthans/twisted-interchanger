# Twisted Interchanger

## Description
The main purpose of this project is, exploring the automated transformation of nested recursive traversals. 
This project contains a source to source transformation tool written using clang 3.8.0 libtooling and a set of dual-tree nested recursive traversal benchmark programs
The conceptual details of this transformation is well explained in the paper.
Our transformation could be viewed as an equivalent to loop interchange in nested recursive iteration spaces. The main focus of this transformation is locality enhancement.

## Exploring Code
This repository has two main directories, clang-tool and benchmark

### clang-tool
This is a source to source transformation tool written in clang 3.8.0 One needs clang/llvm to build this tool.

### How to build the tool
* cd clang-tool
* make

This will create a binary named tool

### How to use the tool
* ./tool <filename>

tool takes the file where the recursive nested traversals are written. There are plenty of examples for this in the benchmark folder.
This tool requires compilation-database.json which contains the compilation commands for the files of the program that you are going to apply this transformation.
This file can be created by hand if your program is simple. But if you have a make file project it can be create using the utility [Build EAR](https://github.com/rizsotto/Bear). 
This will be clearly explained in the benchmark section

### benchmark
Each benchmark program contains a folder named dual-tree. This folder contains the dual-tree version of the program.
For instance, point-correlation folder contains point-correlation/dual-tree directory. 

* cd ./benchmark/point-correlation/dual-tree
* make

This will create a binary named run.

Now let us see how to create a transformed version of the same code
* cd ./bencmark/point-correlation
* cp dual-tree twisted-tree
* cd ./twisted-tree
* bear make (This is not needed if you have the compilation-database.json)
* ../../../tool/tool ./src/traversal.cpp
* Change the usage of the recursive function in the main.cpp file (This will be automated soon) e.g. recurse1(root1, root1); => recurse1(root1, root1, 0);
* make 

This sequence of steps will produce the binary for the transformed code
