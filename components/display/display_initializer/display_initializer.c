#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "display_initializer.h"
#include "lvgl.h"
#include "lvgl_helpers.h"
#include "types.h"

#define LV_TICK_PERIOD_MS 1

static void guiTask(void *pvParameter);

static void lv_tick_task(void *arg);

void display_init(CallbackWithPayload on_display_initiated) {
  /* If you want to use a task to create the graphic, you NEED to create a Pinned task
* Otherwise there can be problem such as memory corruption and so on.
* NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
  xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, on_display_initiated, 0, NULL, 1);
}

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;

static void guiTask(void *pvParameter) {
//  (void) pvParameter;
  xGuiSemaphore = xSemaphoreCreateMutex();

  lv_init();

  /* Initialize SPI or I2C bus used by the drivers */
  lvgl_driver_init();

  lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  assert(buf1 != NULL);

  static lv_color_t *buf2 = NULL;
  static lv_disp_buf_t disp_buf;

  uint32_t size_in_px = DISP_BUF_SIZE;

  /* Actual size in pixels, not bytes. */
  size_in_px *= 8;

  /* Initialize the working buffer depending on the selected display.
   * NOTE: buf2 == NULL when using monochrome displays. */
  lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.flush_cb = disp_driver_flush;

  /* When using a monochrome display we need to register the callbacks:
   * - rounder_cb
   * - set_px_cb */
  disp_drv.rounder_cb = disp_driver_rounder;
  disp_drv.set_px_cb = disp_driver_set_px;

  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /* Create and start a periodic timer interrupt to call lv_tick_inc */
  const esp_timer_create_args_t periodic_timer_args = {
     .callback = &lv_tick_task,
     .name = "periodic_gui"
  };
  esp_timer_handle_t periodic_timer;
  ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

  CallbackWithPayload on_display_initiated = (CallbackWithPayload) pvParameter;
  call_with_payload_if_defined(on_display_initiated, NULL);

  while (1) {
    /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
    vTaskDelay(pdMS_TO_TICKS(10));

    /* Try to take the semaphore, call lvgl related function on success */
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
      lv_task_handler();
      xSemaphoreGive(xGuiSemaphore);
    }
  }

  /* A task should NEVER return */
  free(buf1);
  vTaskDelete(NULL);
}

static void lv_tick_task(void *arg) {
  (void) arg;

  lv_tick_inc(LV_TICK_PERIOD_MS);
}
