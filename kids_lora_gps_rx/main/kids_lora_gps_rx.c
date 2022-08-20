#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <cbor.h>
#include <esp_check.h>
#include "lvgl.h"
#include "display.h"
#include "lora.h"

#define TAG "kids_lora_gps_rx"

static lv_obj_t *scr;
static lv_obj_t *label_gps_position;
static lv_obj_t *label_lora_receive;

static void init_screen();

static void init_lora();

static void task_lora_rx(void *p);

static esp_err_t parse_incoming_lora_message(uint8_t *buf, float *latitude, float *longitude, float *speed);

void app_main(void) {
  display_init(init_screen);
  init_lora();
  xTaskCreate(&task_lora_rx, "task_lora_rx", 2048, NULL, 5, NULL);

  while (true) {
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

static void task_lora_rx(void *p) {
  uint8_t buf[50];
  float latitude, longitude, speed;
  int x;
  for (;;) {
    lora_receive();    // put into receive mode

    while (lora_received()) {
      x = lora_receive_packet(buf, sizeof(buf));
      buf[x] = 0;

      if (ESP_OK == parse_incoming_lora_message(buf, &latitude, &longitude, &speed)) {
        char buffer[100];
        sprintf(buffer, "Ariadna & Julia position:\n"
                        "lat=%.05f°N\n"
                        "lng=%.05f°E\n"
                        "spd=%.02f m/s",
                latitude, longitude, speed);
        ESP_LOGD(TAG, "%s", buffer);
        lv_label_set_text(label_gps_position, buffer);
        lv_obj_align(label_gps_position, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 3, -3);
      }

      lora_receive();
    }
    vTaskDelay(1);
  }
}

static void init_screen() {
  scr = lv_disp_get_scr_act(NULL);
  label_gps_position = lv_label_create(scr, NULL);
  lv_label_set_text(label_gps_position, "");

  label_lora_receive = lv_label_create(scr, NULL);
  lv_label_set_text(label_lora_receive, "LORA pkt: 0");
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_font(&style, LV_STATE_DEFAULT, &lv_font_montserrat_14);
  lv_obj_add_style(label_lora_receive, LV_OBJ_PART_MAIN, &style);
}

static void init_lora() {
  lora_init();
  lora_set_frequency(868e6);
  lora_enable_crc();
}

static esp_err_t lora_message_get_float(CborValue *it, float *value);

static esp_err_t parse_incoming_lora_message(uint8_t *buf, float *latitude, float *longitude, float *speed) {
  CborError ret;
  CborValue it;
  CborParser root_parser;
  CborType type;
  CborValue recursed;

  cbor_parser_init(buf, sizeof(buf), 0, &root_parser, &it);
  type = cbor_value_get_type(&it);
  if (type != CborMapType) {
    ESP_LOGE(TAG, "Invalid type: 0x%04X", type);
    return ESP_ERR_INVALID_ARG;
  }

  ret = cbor_value_enter_container(&it, &recursed);
  if (ret != CborNoError) {
    ESP_LOGE(TAG, "%s(%d)", __FUNCTION__, __LINE__);
    return ESP_ERR_INVALID_ARG;
  }

  ESP_RETURN_ON_ERROR(lora_message_get_float(&it, latitude), TAG, "unable to parse latitude");
  ESP_RETURN_ON_ERROR(lora_message_get_float(&it, longitude), TAG, "unable to parse longitude");
  ESP_RETURN_ON_ERROR(lora_message_get_float(&it, speed), TAG, "unable to parse speed");

  ret = cbor_value_leave_container(&it, &recursed);
  if (ret != CborNoError) {
    ESP_LOGE(TAG, "%s(%d)", __FUNCTION__, __LINE__);
    return ESP_ERR_INVALID_ARG;
  }

  return ESP_OK;
}

static esp_err_t lora_message_get_float(CborValue *it, float *value) {
  CborError ret;
  CborType type;
  type = cbor_value_get_type(it);
  if (type != CborFloatType) {
    ESP_LOGE(TAG, "Invalid type: 0x%04X", type);
    return ESP_ERR_INVALID_ARG;
  }
  ret = cbor_value_get_float(it, value);
  if (ret != CborNoError) {
    return ESP_ERR_INVALID_ARG;
  }
  return ESP_OK;
}
