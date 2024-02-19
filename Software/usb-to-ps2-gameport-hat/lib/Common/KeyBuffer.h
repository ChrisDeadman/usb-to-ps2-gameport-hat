#ifndef _KEY_BUFFER_H_
#define _KEY_BUFFER_H_

#include "CircularBuffer.h"
#include "KeyboardAction.h"
#include "KeyboardCodes.h"

#define KEYBOARD_KRO 3

class KeyBuffer {
 private:
  CircularBuffer<KeyboardAction, KEYBOARD_KRO> buffer;

 public:
  /**
   * Returns the length of the buffer.
   *
   * Always returns 'SIZE' when filled, until cleared.
   */
  uint16_t length();

  /**
   * Add a keyboard action to the rear of the buffer.
   *
   * Overwrites the rear-most MAKE action if filled.
   */
  void enq(KeyboardAction kb_action);

  /**
   * Get the next keyboard action and remove it from the buffer.
   */
  KeyboardAction deq();

  /**
   * Remove all items from the buffer.
   */
  void clear();
};

#endif  //_KEY_BUFFER_H_
