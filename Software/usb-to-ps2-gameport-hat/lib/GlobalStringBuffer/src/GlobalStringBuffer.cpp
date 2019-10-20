#include "GlobalStringBuffer.h"

inline uint16_t safeSize(uint16_t maxSize, uint16_t offset, uint16_t size) {
  return (size <= (maxSize - offset)) ? size : (maxSize - offset);
}

GlobalStringBuffer::GlobalStringBuffer(uint16_t maxSize)
    : maxSize(maxSize), buffer(new char[maxSize + 1]) {
  this->buffer[0] = 0;
}

GlobalStringBuffer* const GlobalStringBuffer::alloc(uint16_t maxSize) {
  return new GlobalStringBuffer(maxSize);
}

const char* const GlobalStringBuffer::get() { return buffer; }

uint16_t GlobalStringBuffer::length() { return strlen(buffer); }

bool GlobalStringBuffer::isEmpty() { return length() <= 0; }

GlobalStringBuffer* const GlobalStringBuffer::clear() {
  buffer[0] = 0;
  return this;
}

GlobalStringBuffer* const GlobalStringBuffer::concat(char const* const str) {
  strncat(buffer, str, safeSize(maxSize, length(), strlen(str)));
  return this;
}

GlobalStringBuffer* const GlobalStringBuffer::concatln(char const* const str) {
  return this->concat(str)->concat(CR_LF);
}
