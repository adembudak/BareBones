
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
  #error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
  #error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

enum vga_color {
  black = 0,
  blue,
  green,
  cyan,
  red,
  magenta,
  brown,
  light_grey,
  dark_grey,
  light_blue,
  light_green,
  light_cyan,
  light_red,
  light_magenta,
  light_brown,
  white,
};
typedef enum vga_color vga_color;

uint8_t vga_entry_color(vga_color fg, vga_color bg) {
  return fg | bg << 4;
}

uint16_t vga_entry(unsigned char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer = (uint16_t *)VGA_MEMORY;

void terminal_initialize(void) {
  terminal_row = terminal_column = 0;
  terminal_color = vga_entry_color(light_grey, black);

  for(size_t y = 0; y < VGA_HEIGHT; y++) {
    for(size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

void terminal_setColor(uint8_t color) {
  terminal_color = color;
}

void terminal_putEntryAt(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
  if(c == '\n') {
    ++terminal_row;
    terminal_column = 0;
  }

  terminal_putEntryAt(c, terminal_color, terminal_column, terminal_row);
  if(++terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    if(++terminal_row == VGA_HEIGHT) terminal_row = 0;
  }
}

void terminal_write(const char *data, size_t size) {
  for(size_t i = 0; i < size; i++)
    terminal_putchar(data[i]);
}

size_t strlen(const char *str) {
  size_t len = 0;
  while(str[len])
    ++len;

  return len;
}

void terminal_writeString(const char *data) {
  terminal_write(data, strlen(data));
}

void kernel_main(void) {
  terminal_initialize();
  terminal_writeString("Hello, kernel World!\n");
}
