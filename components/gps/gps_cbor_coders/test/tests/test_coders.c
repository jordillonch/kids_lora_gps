#include <stdio.h>
#include <esp_err.h>
#include "unity.h"
#include <gps_cbor_coders.h>

void test_It_should_encode_gps_coordinates() {
  uint8_t message[50];
  size_t message_length = 50;
  float latitude = 42.35456;
  float longitude = 2.12345;
  float speed = 0.1;
  esp_err_t result = gps_cbor_encode(latitude, longitude, speed, message, &message_length);
  TEST_ASSERT_EQUAL(ESP_OK, result);
  TEST_ASSERT_EQUAL(16, message_length);
}

void test_It_should_encode_and_decode_gps_coordinates() {
  uint8_t message[50];
  size_t message_length = 50;
  float latitude = 42.35456;
  float longitude = 2.12345;
  float speed = 0.1;
  esp_err_t result = gps_cbor_encode(latitude, longitude, speed, message, &message_length);
  TEST_ASSERT_EQUAL(ESP_OK, result);
  float decoded_latitude, decoded_longitude, decoded_speed;
  result = gps_cbor_decode(message, message_length, &decoded_latitude, &decoded_longitude, &decoded_speed);
  TEST_ASSERT_EQUAL(ESP_OK, result);
  TEST_ASSERT_EQUAL(latitude, decoded_latitude);
  TEST_ASSERT_EQUAL(longitude, decoded_longitude);
  TEST_ASSERT_EQUAL(speed, decoded_speed);
}
