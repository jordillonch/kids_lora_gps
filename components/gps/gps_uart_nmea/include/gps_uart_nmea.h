#include <esp_err.h>
#include "types.h"
#include "nmea_parser.h"

nmea_parser_handle_t gps_init(esp_err_t *result);

esp_err_t gps_deinit(nmea_parser_handle_t handler);

esp_err_t gps_add_handler(nmea_parser_handle_t nmea_hdl, esp_event_handler_t event_handler, void *handler_args);
