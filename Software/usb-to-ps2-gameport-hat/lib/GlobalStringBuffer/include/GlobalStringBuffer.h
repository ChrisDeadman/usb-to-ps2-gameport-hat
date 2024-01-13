#ifndef _GLOBAL_STRING_BUFFER_H_
#define _GLOBAL_STRING_BUFFER_H_

#include <Arduino.h>

class GlobalStringBuffer {
 private:
  static constexpr char const* CR_LF = "\r\n";

  const uint16_t max_size;
  char* const buffer;
  uint16_t buffer_length;

  // forbid copying, stack allocation and destruction
  GlobalStringBuffer() = delete;
  GlobalStringBuffer(const GlobalStringBuffer&) = delete;
  void operator=(const GlobalStringBuffer&) = delete;
  ~GlobalStringBuffer() = delete;

  GlobalStringBuffer(uint16_t maxSize);

 public:
  /**
   * Allocates a global string with a given maximum size.
   */
  static GlobalStringBuffer* const alloc(uint16_t max_size);

  /**
   * Returns a pointer to the string buffer.
   */
  const char* const get();

  /**
   * Returns the length of the string.
   */
  uint16_t length();

  /**
   * Clears all data from the buffer.
   */
  GlobalStringBuffer* const clear();

  /**
   * Adds a string to the buffer.
   *
   * If the result would exceed maxLength, the string is truncated.
   */
  GlobalStringBuffer* const concat(char const* const str);

  /**
   * Adds a string to the buffer and terminates it with CR/LF.
   *
   * If the result would exceed maxLength, the string is truncated.
   */
  GlobalStringBuffer* const concatln(char const* const str);

  /**
   * Adds a formatted value to the buffer using 'snprintf'.
   *
   * If the result would exceed maxLength, the value is truncated.
   */
  template <typename T>
  GlobalStringBuffer* const concat(char const* const format, T value);

  /**
   * Adds a formatted value to the buffer and terminates it with CR/LF.
   *
   * If the result would exceed maxLength, the value is truncated.
   */
  template <typename T>
  GlobalStringBuffer* const concatln(char const* const format, T value);
};

template <typename T>
GlobalStringBuffer* const GlobalStringBuffer::concat(char const* const format,
                                                     T value) {
  uint16_t remaining = max_size + 1 - buffer_length;
  int written = snprintf(&buffer[buffer_length], remaining, format, value);
  if (written > 0 && written < remaining) {
    buffer_length += written;
  }
  return this;
}

template <typename T>
GlobalStringBuffer* const GlobalStringBuffer::concatln(char const* const format,
                                                       T value) {
  return this->concat(format, value)->concat(CR_LF);
}

#endif  //_GLOBAL_STRING_BUFFER_H_
