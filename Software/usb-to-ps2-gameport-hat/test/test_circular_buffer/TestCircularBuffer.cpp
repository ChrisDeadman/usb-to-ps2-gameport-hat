#include <unity.h>

#include "CircularBuffer.h"

CircularBuffer<uint8_t, 3> buffer;

void setUp(void) { buffer.clear(); }

void tearDown(void) {}

void test_simple(void) {
  TEST_ASSERT_EQUAL_UINT16(0, buffer.length());
  buffer.enq(42);
  TEST_ASSERT_EQUAL_UINT16(1, buffer.length());
  TEST_ASSERT_EQUAL_UINT8(42, buffer.get(0));
  TEST_ASSERT_EQUAL_UINT8(42, buffer.deq());
  TEST_ASSERT_EQUAL_UINT16(0, buffer.length());
}

void test_filled(void) {
  buffer.enq(1);
  buffer.enq(2);
  buffer.enq(3);
  TEST_ASSERT_EQUAL_UINT16(3, buffer.length());
  TEST_ASSERT_EQUAL_UINT8(1, buffer.get(0));
  TEST_ASSERT_EQUAL_UINT8(2, buffer.get(1));
  TEST_ASSERT_EQUAL_UINT8(3, buffer.get(2));
  TEST_ASSERT_EQUAL_UINT8(1, buffer.deq());
  TEST_ASSERT_EQUAL_UINT16(2, buffer.length());
  TEST_ASSERT_EQUAL_UINT8(2, buffer.deq());
  TEST_ASSERT_EQUAL_UINT16(1, buffer.length());
  TEST_ASSERT_EQUAL_UINT8(3, buffer.deq());
  TEST_ASSERT_EQUAL_UINT16(0, buffer.length());
}

void test_overfilled(void) {
  buffer.enq(1);
  buffer.enq(2);
  buffer.enq(3);
  buffer.enq(4);
  buffer.enq(5);
  TEST_ASSERT_EQUAL_UINT16(3, buffer.length());
  TEST_ASSERT_EQUAL_UINT8(3, buffer.get(0));
  TEST_ASSERT_EQUAL_UINT8(4, buffer.get(1));
  TEST_ASSERT_EQUAL_UINT8(5, buffer.get(2));
  TEST_ASSERT_EQUAL_UINT8(3, buffer.deq());
  TEST_ASSERT_EQUAL_UINT16(2, buffer.length());
  TEST_ASSERT_EQUAL_UINT8(4, buffer.deq());
  TEST_ASSERT_EQUAL_UINT16(1, buffer.length());
  TEST_ASSERT_EQUAL_UINT8(5, buffer.deq());
  TEST_ASSERT_EQUAL_UINT16(0, buffer.length());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_simple);
  RUN_TEST(test_filled);
  RUN_TEST(test_overfilled);
  return UNITY_END();
}
