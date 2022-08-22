#include <cbor.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include "gps_cbor_coders.h"

#define TAG "gps_cbor_coders"

static esp_err_t get_float(CborValue *it, float *value);

esp_err_t gps_cbor_encode(float latitude, float longitude, float speed, uint8_t *message, size_t *message_length) {
  CborEncoder root_encoder, array_encoder;
  cbor_encoder_init(&root_encoder, message, *message_length, 0);
  ESP_RETURN_ON_ERROR(cbor_encoder_create_array(&root_encoder, &array_encoder, 3), TAG, "unable to create array");
  ESP_RETURN_ON_ERROR(cbor_encode_float(&array_encoder, latitude), TAG, "unable to encode latitude");
  ESP_RETURN_ON_ERROR(cbor_encode_float(&array_encoder, longitude), TAG, "unable to encode longitude");
  ESP_RETURN_ON_ERROR(cbor_encode_float(&array_encoder, speed), TAG, "unable to encode speed");
  ESP_RETURN_ON_ERROR(cbor_encoder_close_container(&root_encoder, &array_encoder), TAG, "unable to close map");
  *message_length = cbor_encoder_get_buffer_size(&root_encoder, message);
  ESP_LOGD(TAG, "CBOR Output: %d bytes", *message_length);
  for (int i = 0; i < *message_length; i++) {
    ESP_LOGD(TAG, "0x%02x", message[i]);
  }
  return ESP_OK;
}

esp_err_t gps_cbor_decode(uint8_t *message, size_t message_length, float *latitude, float *longitude, float *speed) {
  CborError ret;
  CborValue it;
  CborParser root_parser;
  CborType type;
  CborValue recursed;

  cbor_parser_init(message, message_length, 0, &root_parser, &it);
  type = cbor_value_get_type(&it);
  if (type != CborArrayType) {
    ESP_LOGE(TAG, "Invalid type: 0x%04X", type);
    return ESP_ERR_INVALID_ARG;
  }

  assert(cbor_value_is_container(&it));
  ret = cbor_value_enter_container(&it, &recursed);
  if (ret != CborNoError) {
    ESP_LOGE(TAG, "%s(%d)", __FUNCTION__, __LINE__);
    return ESP_ERR_INVALID_ARG;
  }

  ESP_RETURN_ON_ERROR(get_float(&recursed, latitude), TAG, "unable to parse latitude");
  ESP_RETURN_ON_ERROR(get_float(&recursed, longitude), TAG, "unable to parse longitude");
  ESP_RETURN_ON_ERROR(get_float(&recursed, speed), TAG, "unable to parse speed");

  ret = cbor_value_leave_container(&it, &recursed);
  if (ret != CborNoError) {
    ESP_LOGE(TAG, "%s(%d)", __FUNCTION__, __LINE__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

static esp_err_t get_float(CborValue *it, float *value) {
  CborError ret;
  CborType type;
  type = cbor_value_get_type(it);
  ESP_LOGD(TAG, "Type: 0x%04X", type);
  if (type != CborFloatType) {
    ESP_LOGE(TAG, "Invalid type: 0x%04X, it should be CborFloatType", type);
    return ESP_ERR_INVALID_ARG;
  }
  ret = cbor_value_get_float(it, value);
  if (ret != CborNoError) {
    return ESP_ERR_INVALID_ARG;
  }
  ret = cbor_value_advance_fixed(it);
  if (ret != CborNoError) {
    ESP_LOGE(TAG, "Advance fixed failed");
    return ESP_ERR_INVALID_ARG;
  }
  return ESP_OK;
}
