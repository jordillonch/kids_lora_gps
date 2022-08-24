#include <esp_log.h>
#include <test_coders.h>
#include "unity.h"

// used for target test

#define TAG "test_gps_cbor_coders"

TEST_CASE("It should encode a gps coordinates", "[encoder]") {
  test_It_should_encode_gps_coordinates();
}

TEST_CASE("It should encode and decode a gps coordinates", "[decoder]") {
  test_It_should_encode_and_decode_gps_coordinates();
}
