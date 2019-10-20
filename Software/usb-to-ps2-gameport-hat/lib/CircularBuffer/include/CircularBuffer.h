#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <Arduino.h>

template <typename T, uint16_t SIZE>
class CircularBuffer {
 private:
  T buffer[SIZE];

  /**
   * index of the newest value.
   */
  uint16_t headIdx;

  /**
   * index of the oldest value.
   */
  uint16_t tailIdx;

 public:
  CircularBuffer();

  /**
   * Returns the length of the buffer.
   *
   * After it is filled it will always return 'SIZE' until it's cleared.
   */
  uint16_t length();

  /**
   * Whether all slots of the buffer are filled.
   */
  bool isFilled();

  /**
   * Puts a new value into the buffer after the newest value.
   *
   * Overwrites the oldest value if it's full.
   */
  void put(T value);

  /**
   * Returns the value at the given index starting with the oldest value.
   */
  T get(uint16_t idx);

  /**
   * Removes all values from the buffer.
   */
  void clear();
};

template <class T, uint16_t SIZE>
CircularBuffer<T, SIZE>::CircularBuffer() {
  clear();
}

template <typename T, uint16_t SIZE>
uint16_t CircularBuffer<T, SIZE>::length() {
  return (tailIdx > 0) ? SIZE : (headIdx + 1);
}

template <typename T, uint16_t SIZE>
bool CircularBuffer<T, SIZE>::isFilled() {
  return length() >= SIZE;
}

template <typename T, uint16_t SIZE>
void CircularBuffer<T, SIZE>::put(T value) {
  if (++headIdx >= SIZE) {
    headIdx = 0;
    tailIdx = 1;
  } else if (tailIdx > 0) {
    tailIdx = (tailIdx + 1) % SIZE;
  }
  buffer[headIdx] = value;
}

template <typename T, uint16_t SIZE>
T CircularBuffer<T, SIZE>::get(uint16_t idx) {
  return buffer[(tailIdx + idx) % SIZE];
}

template <typename T, uint16_t SIZE>
void CircularBuffer<T, SIZE>::clear() {
  headIdx = -1;
  tailIdx = 0;
}

#endif  //_CIRCULAR_BUFFER_H_
