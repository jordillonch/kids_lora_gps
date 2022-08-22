#include "nmea_parser.h"
#include "gps_uart_nmea.h"

nmea_parser_handle_t gps_init(esp_err_t *result) {
  nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
  nmea_parser_handle_t handler = nmea_parser_init(&config);
  *result = ESP_OK;
  return handler;
}

esp_err_t gps_deinit(nmea_parser_handle_t handler) {
  nmea_parser_deinit(handler);
  return ESP_OK;
}

esp_err_t gps_add_handler(nmea_parser_handle_t nmea_hdl, esp_event_handler_t event_handler, void *handler_args) {
  return nmea_parser_add_handler(nmea_hdl, event_handler, handler_args);
}
