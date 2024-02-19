#include "KeyBuffer.h"

uint16_t KeyBuffer::length() { return buffer.length(); }

void KeyBuffer::enq(KeyboardAction kb_action) {
  uint16_t make_count = 0;
  uint16_t overwrite_idx = buffer.length() - 1;

  // buffer not full -> we can enqueue
  if (!buffer.is_filled()) {
    buffer.enq(kb_action);
    return;
  }

  // discard MAKE actions if no space
  if (kb_action.type == KbActionMake) {
    return;
  }

  // search for MAKE actions we can replace
  for (uint16_t idx = 0; idx < buffer.length(); idx++) {
    if (buffer.get(idx).type == KbActionMake) {
      overwrite_idx = idx;
      make_count++;
    }
  }

  // replace a MAKE action with the BREAK action
  if (make_count > 0) {
    buffer.set(overwrite_idx, kb_action);
  }
}

KeyboardAction KeyBuffer::deq() { return buffer.deq(); }

void KeyBuffer::clear() { buffer.clear(); }
