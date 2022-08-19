#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <cbor.h>
#include <time.h>
#include "gps.h"
#include "lvgl.h"
#include "display.h"
#include "lora.h"

#define TAG "kids_lora_gps"
#define LORA_SEND_GPS_COORDINATES_EVERY_SECONDS 60

static lv_obj_t *scr;
static lv_obj_t *label_gps_position;
static lv_obj_t *label_lora_transmission;
static int lora_packet_count;
static time_t lora_sent_last_time;

static nmea_parser_handle_t init_gps();

static void on_gps_update_screen(void *event_handler_arg,
                                 esp_event_base_t event_base,
                                 int32_t event_id,
                                 void *event_data);

static void on_gps_send_position_through_lora(void *event_handler_arg,
                                              esp_event_base_t event_base,
                                              int32_t event_id,
                                              void *event_data);

static void init_screen();

static void init_lora();

void app_main(void) {
  nmea_parser_handle_t gps = init_gps();
  display_init(init_screen);
  init_lora();

  gps_add_handler(gps, on_gps_update_screen, NULL);
  gps_add_handler(gps, on_gps_send_position_through_lora, NULL);

  while (true) {
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }

  gps_deinit(gps);
}

static void on_gps_update_screen(void *event_handler_arg,
                                 esp_event_base_t event_base, int32_t
                                 event_id,
                                 void *event_data) {
  gps_t *gps = NULL;
  switch (event_id) {
    case GPS_UPDATE:
      gps = (gps_t *) event_data;
      char buffer[100];
      sprintf(buffer, "Ariadna & Julia position: "
                      "lat=%.05f°N | "
                      "lng=%.05f°E",
                      gps->latitude, gps->longitude);
      ESP_LOGD(TAG, "%s", buffer);

      sprintf(buffer, "Ariadna & Julia position:\n"
                      "lat=%.05f°N\n"
                      "lng=%.05f°E",
                      gps->latitude, gps->longitude);
      lv_label_set_text(label_gps_position, buffer);
      lv_obj_align(label_gps_position, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 3, -3);
      break;
    case GPS_UNKNOWN:
      /* print unknown statements */
      ESP_LOGW(TAG, "Unknown statement:%s", (char *) event_data);
      break;
    default:
      break;
  }
}

static void on_gps_send_position_through_lora(void *event_handler_arg,
                                              esp_event_base_t event_base,
                                              int32_t event_id,
                                              void *event_data) {
  time_t now;
  gps_t *gps = NULL;
  switch (event_id) {
    case GPS_UPDATE:
      time(&now);
      if (difftime(now, lora_sent_last_time) >= LORA_SEND_GPS_COORDINATES_EVERY_SECONDS) {
        gps = (gps_t *) event_data;
        //  Max output size is 50 bytes (which fits in a LoRa packet)
        uint8_t output[50];
        CborEncoder root_encoder, map_encoder;
        cbor_encoder_init(&root_encoder, output, sizeof(output), 0);
        CborError res = cbor_encoder_create_map(&root_encoder, &map_encoder, 3);
        ESP_ERROR_CHECK(res);
        res = cbor_encode_text_stringz(&map_encoder, "lat");
        ESP_ERROR_CHECK(res);
        res = cbor_encode_float(&map_encoder, gps->latitude);
        ESP_ERROR_CHECK(res);
        res = cbor_encode_text_stringz(&map_encoder, "lng");
        ESP_ERROR_CHECK(res);
        res = cbor_encode_float(&map_encoder, gps->longitude);
        ESP_ERROR_CHECK(res);
        res = cbor_encode_text_stringz(&map_encoder, "s");
        ESP_ERROR_CHECK(res);
        res = cbor_encode_float(&map_encoder, gps->speed);
        ESP_ERROR_CHECK(res);
        res = cbor_encoder_close_container(&root_encoder, &map_encoder);
        ESP_ERROR_CHECK(res);
        size_t output_len = cbor_encoder_get_buffer_size(&root_encoder, output);
        ESP_LOGD(TAG, "CBOR Output: %d bytes", output_len);
        for (int i = 0; i < output_len; i++) {
          ESP_LOGD(TAG, "0x%02x", output[i]);
        }

        lora_send_packet(output, (int) output_len);
        lora_packet_count++;

        char buffer[30];
        sprintf(buffer, "LORA pkt: %i", lora_packet_count);
        lv_label_set_text(label_lora_transmission, buffer);
        lv_obj_align(label_lora_transmission, NULL, LV_ALIGN_IN_TOP_LEFT, 3, 3);

        time(&lora_sent_last_time);
      }
      break;
    case GPS_UNKNOWN:
      /* print unknown statements */
      ESP_LOGW(TAG, "Unknown statement:%s", (char *) event_data);
      break;
    default:
      break;
  }
}

static nmea_parser_handle_t init_gps() {
  esp_err_t result;
  nmea_parser_handle_t gps;
  gps = gps_init(&result);
  ESP_ERROR_CHECK(result);
  return gps;
}

static void init_screen() {
  scr = lv_disp_get_scr_act(NULL);
  label_gps_position = lv_label_create(scr, NULL);
  lv_label_set_text(label_gps_position, "");

  label_lora_transmission = lv_label_create(scr, NULL);
  lv_label_set_text(label_lora_transmission, "LORA pkt: 0");
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_font(&style, LV_STATE_DEFAULT, &lv_font_montserrat_14);
  lv_obj_add_style(label_lora_transmission, LV_OBJ_PART_MAIN, &style);
}

static void init_lora() {
  lora_init();
  lora_set_frequency(868e6);
  lora_enable_crc();
}
