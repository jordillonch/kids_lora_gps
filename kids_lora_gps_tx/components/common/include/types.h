#ifndef LIB_COMMON_TYPES_H
#define LIB_COMMON_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef void *Payload;

typedef void (*CallbackWithPayload)(Payload);

typedef void *Payload;

void call_with_payload_if_defined(CallbackWithPayload callback, Payload payload);

#endif // LIB_COMMON_TYPES_H
