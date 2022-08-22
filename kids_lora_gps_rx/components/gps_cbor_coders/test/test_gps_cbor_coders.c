#include <esp_log.h>
#include "unity.h"
#include "gps_cbor_coders.h"

#define TAG "test_gps_cbor_coders"

TEST_CASE("It should encode a gps coordinates", "[encoder]") {
  uint8_t message[50];
  size_t message_length = 50;
  float latitude = 42.35456;
  float longitude = 2.12345;
  float speed = 0.1;
  esp_err_t result = gps_cbor_encode(latitude, longitude, speed, message, &message_length);
  TEST_ASSERT_EQUAL(ESP_OK, result);
  TEST_ASSERT_EQUAL(16, message_length);
}

TEST_CASE("It should decode a gps coordinates", "[encoder]") {
  uint8_t message[50];
  size_t message_length = 50;
  float latitude = 42.35456;
  float longitude = 2.12345;
  float speed = 0.1;
  esp_err_t result = gps_cbor_encode(latitude, longitude, speed, message, &message_length);
  TEST_ASSERT_EQUAL(ESP_OK, result);

  ESP_LOGD(TAG, "length: %i", message_length);
  for (int i = 0; i < message_length; i++) {
    ESP_LOGD(TAG, "0x%02x", message[i]);
  }

  float decoded_latitude, decoded_longitude, decoded_speed;
  result = gps_cbor_decode(message, message_length, &decoded_latitude, &decoded_longitude, &decoded_speed);
  TEST_ASSERT_EQUAL(ESP_OK, result);
  TEST_ASSERT_EQUAL(latitude, decoded_latitude);
  TEST_ASSERT_EQUAL(longitude, decoded_longitude);
  TEST_ASSERT_EQUAL(speed, decoded_speed);
}
