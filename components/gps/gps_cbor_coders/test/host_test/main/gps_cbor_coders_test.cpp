#include "unity.h"

extern "C" {
#include <test_coders.h>
}

using namespace std;

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_It_should_encode_gps_coordinates);
  RUN_TEST(test_It_should_encode_and_decode_gps_coordinates);
  int failures = UNITY_END();
  return failures;
}
