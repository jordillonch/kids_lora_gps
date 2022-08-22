#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_check.h>
#include "lvgl.h"
#include "display.h"
#include "lora.h"
#include "gps_cbor_coders.h"

#define TAG "kids_lora_gps_rx"

static lv_obj_t *scr;
static lv_obj_t *label_gps_position;
static lv_obj_t *label_lora_receive;

static void init_screen();

static void init_lora();

static void task_lora_rx(void *p);

void app_main(void) {
  display_init(init_screen);
  init_lora();
  xTaskCreate(&task_lora_rx, "task_lora_rx", 2048, NULL, 5, NULL);

  while (true) {
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

static void task_lora_rx(void *p) {
  uint8_t lora_message[50];
  float latitude, longitude, speed;
  int x;
  for (;;) {
    lora_receive();    // put into receive mode

    while (lora_received()) {
      x = lora_receive_packet(lora_message, sizeof(lora_message));
      lora_message[x] = 0;

      if (ESP_OK == gps_cbor_decode(lora_message, &latitude, &longitude, &speed)) {
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
