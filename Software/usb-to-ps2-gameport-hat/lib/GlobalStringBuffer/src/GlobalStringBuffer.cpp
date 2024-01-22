#include "GlobalStringBuffer.h"

GlobalStringBuffer::GlobalStringBuffer(uint16_t max_size)
    : max_size(max_size), buffer(new char[max_size + 1]) {
  clear();
}

GlobalStringBuffer* const GlobalStringBuffer::alloc(uint16_t max_size) {
  return new GlobalStringBuffer(max_size);
}

const char* const GlobalStringBuffer::get() { return buffer; }

uint16_t GlobalStringBuffer::length() { return buffer_length; }

GlobalStringBuffer* const GlobalStringBuffer::clear() {
  buffer[0] = 0;
  buffer_length = 0;
  return this;
}

GlobalStringBuffer* const GlobalStringBuffer::concat(char const* const str) {
  size_t remaining = max_size - buffer_length;
  if (remaining > 0) {
    size_t str_len = min(strlen(str), remaining);
    memcpy(&buffer[buffer_length], str, str_len);
    buffer_length += str_len;
    buffer[buffer_length] = 0;
  }
  return this;
}

GlobalStringBuffer* const GlobalStringBuffer::concatln(char const* const str) {
  return this->concat(str)->concat(CR_LF);
}
