#include <unity.h>

#include "HIDKeyboardParser.h"

// Constants for scancodes
#define SC_NO_KEY 0x00
#define SC_ERROR_ROLLOVER 0x01
#define SC_A 0x04
#define SC_B 0x05
#define SC_C 0x06
#define SC_D 0x07
#define SC_E 0x08
#define SC_F 0x09
#define SC_PAUSE 0x48

// HID buffer indices
#define HID_REPORT_LEN 8
#define MODIFIER_BYTE_INDEX 0
#define RESERVED_BYTE_INDEX 1
#define KEY_START_INDEX 2
#define NUM_KEY_BYTES (HID_REPORT_LEN - KEY_START_INDEX)

// Helper function to initialize the buffer
static void clear_buffer(uint8_t *buf) {
  for (int i = 0; i < HID_REPORT_LEN; ++i) {
    buf[i] = SC_NO_KEY;
  }
}

void test_reset_state(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;

  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL_UINT8(KbActionNone, kb_action.type);
}

void test_press_release_a(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN] = {0};

  // Press 'a' key
  buf[KEY_START_INDEX] = SC_A;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(aA, kb_action.code);

  // Release 'a' key
  buf[KEY_START_INDEX] = SC_NO_KEY;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(aA, kb_action.code);
}

void test_press_release_LeftShift(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN] = {0};

  // Press Left Shift
  buf[MODIFIER_BYTE_INDEX] = ModLeftShift;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(LeftShift, kb_action.code);

  // Release Left Shift
  buf[MODIFIER_BYTE_INDEX] = SC_NO_KEY;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(LeftShift, kb_action.code);
}

void test_press_Shift_a(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN] = {0};

  // Press Left Shift and 'a'
  buf[MODIFIER_BYTE_INDEX] = ModLeftShift;
  buf[KEY_START_INDEX] = SC_A;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue Left Shift make action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(LeftShift, kb_action.code);

  // Dequeue 'a' make action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(aA, kb_action.code);

  // Release both keys
  buf[MODIFIER_BYTE_INDEX] = SC_NO_KEY;
  buf[KEY_START_INDEX] = SC_NO_KEY;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue Left Shift break action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(LeftShift, kb_action.code);

  // Dequeue 'a' break action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(aA, kb_action.code);
}

void test_press_release_a_b(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN] = {0};

  // Press 'a' and 'b' keys
  buf[KEY_START_INDEX] = SC_A;
  buf[KEY_START_INDEX + 1] = SC_B;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue 'a' make action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(aA, kb_action.code);

  // Dequeue 'b' make action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(bB, kb_action.code);

  // Release both keys
  buf[KEY_START_INDEX] = SC_NO_KEY;
  buf[KEY_START_INDEX + 1] = SC_NO_KEY;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue 'a' break action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(aA, kb_action.code);

  // Dequeue 'b' break action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(bB, kb_action.code);
}

void test_press_a_then_b(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN] = {0};

  // Press 'a' key
  buf[KEY_START_INDEX] = SC_A;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue 'a' make action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(aA, kb_action.code);

  // Press 'b' key while 'a' is still pressed
  buf[KEY_START_INDEX + 1] = SC_B;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue 'b' make action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(bB, kb_action.code);

  // Release 'a' key
  buf[KEY_START_INDEX] = SC_NO_KEY;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue 'a' break action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(aA, kb_action.code);

  // Release 'b' key
  buf[KEY_START_INDEX + 1] = SC_NO_KEY;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue 'b' break action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(bB, kb_action.code);
}

void test_pause_with_ctrl(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN] = {0};

  // Press Left Ctrl and Pause key
  buf[MODIFIER_BYTE_INDEX] = ModLeftCtrl;
  buf[KEY_START_INDEX] = SC_PAUSE;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue Left Ctrl make action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(LeftControl, kb_action.code);

  // Dequeue Pause make action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(Pause, kb_action.code);

  // Release both keys
  buf[MODIFIER_BYTE_INDEX] = SC_NO_KEY;
  buf[KEY_START_INDEX] = SC_NO_KEY;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue Left Ctrl break action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(LeftControl, kb_action.code);

  // Dequeue Pause break action
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
  TEST_ASSERT_EQUAL(Pause, kb_action.code);
}

void test_invalid_scancode(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN] = {0};

  // Press key with scancode 0 (NoKey)
  buf[KEY_START_INDEX] = SC_NO_KEY;
  kb_parser.update(HID_REPORT_LEN, buf);

  // No action should be dequeued
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionNone, kb_action.type);
}

void test_scancode_NoKey(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN] = {0};

  // Press key with scancode that maps to NoKey
  buf[KEY_START_INDEX] = 0x66;
  kb_parser.update(HID_REPORT_LEN, buf);

  // No action should be dequeued
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionNone, kb_action.type);
}

void test_overrun_error(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN];

  // Simulate overrun error by setting all keycodes to ErrorRollOver
  buf[MODIFIER_BYTE_INDEX] = SC_NO_KEY;
  buf[RESERVED_BYTE_INDEX] = SC_NO_KEY;
  for (int i = KEY_START_INDEX; i < HID_REPORT_LEN; ++i) {
    buf[i] = SC_ERROR_ROLLOVER;
  }
  kb_parser.update(HID_REPORT_LEN, buf);

  // No action should be dequeued
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionNone, kb_action.type);

  // Clear overrun error
  for (int i = KEY_START_INDEX; i < HID_REPORT_LEN; ++i) {
    buf[i] = SC_NO_KEY;
  }
  kb_parser.update(HID_REPORT_LEN, buf);

  // No action should be dequeued
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionNone, kb_action.type);
}

void test_key_state_overflow(void) {
  HIDKeyboardParser kb_parser;
  KeyboardAction kb_action;
  uint8_t buf[HID_REPORT_LEN] = {0};

  // Press keys up to KEYBOARD_KRO limit
  uint8_t scancodes[KEYBOARD_KRO];
  KeyboardCodes expected_keys[KEYBOARD_KRO];

  // Initialize scancodes and expected key codes
  for (int i = 0; i < KEYBOARD_KRO; ++i) {
    scancodes[i] = SC_A + i;                                // SC_A, SC_B, SC_C, etc.
    expected_keys[i] = static_cast<KeyboardCodes>(aA + i);  // aA, bB, cC, etc.
  }

  // Fill the buffer with scancodes
  for (int i = 0; (i < KEYBOARD_KRO) && (i < NUM_KEY_BYTES); ++i) {
    buf[KEY_START_INDEX + i] = scancodes[i];
  }

  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue make actions for all keys
  for (int i = 0; (i < KEYBOARD_KRO) && (i < NUM_KEY_BYTES); ++i) {
    kb_action = kb_parser.deq();
    TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
    TEST_ASSERT_EQUAL(expected_keys[i], kb_action.code);
  }

  // Simulate overrun error by setting all keycodes to ErrorRollOver
  buf[MODIFIER_BYTE_INDEX] = SC_NO_KEY;
  buf[RESERVED_BYTE_INDEX] = SC_NO_KEY;
  for (int i = KEY_START_INDEX; i < HID_REPORT_LEN; ++i) {
    buf[i] = SC_ERROR_ROLLOVER;
  }
  kb_parser.update(HID_REPORT_LEN, buf);

  // Expect break actions for previously pressed keys
  for (int i = 0; (i < KEYBOARD_KRO) && (i < NUM_KEY_BYTES); ++i) {
    kb_action = kb_parser.deq();
    TEST_ASSERT_EQUAL(KbActionBreak, kb_action.type);
    TEST_ASSERT_EQUAL(expected_keys[i], kb_action.code);
  }

  // No further actions
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionNone, kb_action.type);

  // Clear overrun error
  for (int i = KEY_START_INDEX; i < HID_REPORT_LEN; ++i) {
    buf[i] = SC_NO_KEY;
  }
  kb_parser.update(HID_REPORT_LEN, buf);

  // No actions should be dequeued
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionNone, kb_action.type);

  // Now press some keys again
  buf[KEY_START_INDEX + 0] = SC_A;
  buf[KEY_START_INDEX + 1] = SC_B;
  kb_parser.update(HID_REPORT_LEN, buf);

  // Dequeue make actions
  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(aA, kb_action.code);

  kb_action = kb_parser.deq();
  TEST_ASSERT_EQUAL(KbActionMake, kb_action.type);
  TEST_ASSERT_EQUAL(bB, kb_action.code);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_reset_state);
  RUN_TEST(test_press_release_a);
  RUN_TEST(test_press_release_LeftShift);
  RUN_TEST(test_press_Shift_a);
  RUN_TEST(test_press_release_a_b);
  RUN_TEST(test_press_a_then_b);
  RUN_TEST(test_pause_with_ctrl);
  RUN_TEST(test_invalid_scancode);
  RUN_TEST(test_scancode_NoKey);
  RUN_TEST(test_overrun_error);
  RUN_TEST(test_key_state_overflow);
  return UNITY_END();
}
