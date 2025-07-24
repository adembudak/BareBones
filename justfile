default:
  @just --list --unsorted

builddir_default := 'build'

alias c := configure
configure builddir= builddir_default:
  cmake --preset default -S . -B {{builddir}} --toolchain ./i686-elf.toolchain.cmake

alias b := build
build builddir= builddir_default:
  cmake --build {{builddir}} --clean-first

run builddir = builddir_default:
  cmake --build {{builddir}} --target run_os
