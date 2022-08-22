esp_err_t gps_cbor_encode(float latitude, float longitude, float speed, uint8_t *message, size_t *message_length);

esp_err_t gps_cbor_decode(uint8_t *message, size_t message_length, float *latitude, float *longitude, float *speed);
