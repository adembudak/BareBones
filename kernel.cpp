#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
  #error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
  #error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

struct VGA_t {
  static constexpr size_t width = 80;
  static constexpr size_t height = 25;
  static constexpr size_t memory = 0xB8000;

  enum vga_color : uint8_t;

  static vga_color entry_color(vga_color fg, vga_color bg);
  static uint16_t entry(unsigned char uc, vga_color color);
};

enum VGA_t::vga_color : uint8_t {
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

VGA_t::vga_color VGA_t::entry_color(vga_color fg, vga_color bg) {
  return vga_color(fg | bg << 4);
}

uint16_t VGA_t::entry(unsigned char uc, vga_color color) {
  return static_cast<uint16_t>(uc) | static_cast<uint16_t>(color) << 8;
}

struct terminal_t {
  size_t m_row = 0;
  size_t m_column = 0;

  VGA_t::vga_color m_color;
  uint16_t *m_buffer;

  terminal_t();

  void setColor(VGA_t::vga_color color);

  void put(size_t x, size_t y, char c, VGA_t::vga_color color);
  void put(char c);
  void put(const char *str, size_t size);
  void put(const char *str);

private:
  [[nodiscard]] size_t strlen(const char *str);
};

terminal_t::terminal_t() {
  m_buffer = reinterpret_cast<uint16_t *>(VGA_t::memory);
  m_color = VGA_t::entry_color(VGA_t::vga_color::light_grey, VGA_t::vga_color::black);

  for(size_t y = 0; y < VGA_t::height; y++) {
    for(size_t x = 0; x < VGA_t::width; x++) {
      const size_t index = y * VGA_t::width + x;

      m_buffer[index] = VGA_t::entry(' ', m_color);
    }
  }
}

void terminal_t::setColor(VGA_t::vga_color color) {
  m_color = color;
}

void terminal_t::put(size_t x, size_t y, char c, VGA_t::vga_color color) {
  const size_t index = y * VGA_t::width + x;
  m_buffer[index] = VGA_t::entry(c, color);
}

void terminal_t::put(char c) {
  if(c == '\n') {
    ++m_row;
    m_column = 0;
  }

  put(m_column, m_row, c, m_color);

  if(++m_column == VGA_t::width) {
    m_column = 0;
    if(++m_row == VGA_t::height) m_row = 0;
  }
}

void terminal_t::put(const char *data, size_t size) {
  for(size_t i = 0; i < size; i++)
    put(data[i]);
}

size_t terminal_t::strlen(const char *str) {
  size_t len = 0;
  while(str[len])
    ++len;

  return len;
}

void terminal_t::put(const char *str) {
  put(str, strlen(str));
}

extern "C"
{
  void kernel_main() {
    terminal_t terminal;
    terminal.setColor(VGA_t::vga_color::light_blue);

    terminal.put("Hello, kernel World!\n");
  }
}
