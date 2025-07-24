cmake_minimum_required(VERSION 4.0.0)

project(BareBones
  DESCRIPTION "Bare bones kernel based on https://wiki.osdev.org/Bare_Bones"
  LANGUAGES ASM C CXX)

add_library(boot OBJECT)
target_sources(boot PRIVATE boot.s)

add_library(kernel OBJECT)
target_sources(kernel PRIVATE kernel.c)

set_target_properties(kernel PROPERTIES LINK_DEPENDS ${PROJECT_SOURCE_DIR}/linker.ld)

add_executable(myos)
target_sources(myos PRIVATE $<TARGET_OBJECTS:boot> $<TARGET_OBJECTS:kernel>)
target_link_options(myos PRIVATE -nostdlib -lgcc -T ${PROJECT_SOURCE_DIR}/linker.ld)

find_program(GRUB_FILE_COMMAND grub-file REQUIRED)
add_custom_command(TARGET myos POST_BUILD COMMAND ${GRUB_FILE_COMMAND} --is-x86-multiboot $<TARGET_FILE:myos> VERBATIM)

find_program(QEMU_COMMAND qemu-system-i386 REQUIRED)
add_custom_target(run_os COMMAND ${QEMU_COMMAND} -kernel $<TARGET_FILE:myos> VERBATIM)
