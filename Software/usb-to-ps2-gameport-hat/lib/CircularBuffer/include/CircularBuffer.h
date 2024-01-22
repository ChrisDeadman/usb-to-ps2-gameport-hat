#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#ifdef PIO_UNIT_TESTING
#include <stdint.h>
#else
#include <Arduino.h>
#endif
#include <assert.h>

/**
 * Circular buffer that overflows to null.
 * `SIZE` must be less than UINT16_MAX.
 */
template <typename T, uint16_t SIZE>
class CircularBuffer {
 private:
  T buffer[SIZE];

  uint16_t rear_idx;
  uint16_t front_idx;

 public:
  CircularBuffer();

  /**
   * Returns the length of the buffer.
   *
   * Always returns 'SIZE' when filled, until cleared.
   */
  uint16_t length();

  /**
   * Whether the buffer is completely filled.
   */
  bool is_filled();

  /**
   * Add an item to the rear of the buffer.
   *
   * Overwrites the rear if filled.
   */
  void enq(T value);

  /**
   * Get the front item and remove it from the buffer.
   */
  T deq();

  /**
   * Get the item at the given index, starting at the front.
   */
  T get(uint16_t idx);

  /**
   * Remove all items from the buffer.
   */
  void clear();
};

template <class T, uint16_t SIZE>
CircularBuffer<T, SIZE>::CircularBuffer() {
  clear();
}

template <typename T, uint16_t SIZE>
uint16_t CircularBuffer<T, SIZE>::length() {
  if (rear_idx == UINT16_MAX) return 0;
  uint16_t len = (rear_idx - front_idx) + 1;
  return len > 0 ? len : SIZE;
}

template <typename T, uint16_t SIZE>
bool CircularBuffer<T, SIZE>::is_filled() {
  return length() >= SIZE;
}

template <typename T, uint16_t SIZE>
void CircularBuffer<T, SIZE>::enq(T value) {
  rear_idx = (uint16_t)(rear_idx + 1) % SIZE;
  buffer[rear_idx] = value;
  if (front_idx == UINT16_MAX) {
    front_idx = 0;
  } else if (rear_idx <= front_idx) {
    front_idx = (uint16_t)(front_idx + 1) % SIZE;
  }
}

template <typename T, uint16_t SIZE>
T CircularBuffer<T, SIZE>::deq() {
  uint16_t len = length();
  assert(len > 0);
  T value = buffer[front_idx];
  if (len > 1) {
    front_idx = (uint16_t)(front_idx + 1) % SIZE;
  } else {
    clear();
  }
  return value;
}

template <typename T, uint16_t SIZE>
T CircularBuffer<T, SIZE>::get(uint16_t idx) {
  return buffer[(uint16_t)(front_idx + idx) % SIZE];
}

template <typename T, uint16_t SIZE>
void CircularBuffer<T, SIZE>::clear() {
  front_idx = UINT16_MAX;
  rear_idx = UINT16_MAX;
}

#endif  //_CIRCULAR_BUFFER_H_
