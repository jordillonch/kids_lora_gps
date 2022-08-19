#include <stdio.h>
#include "types.h"

void call_with_payload_if_defined(CallbackWithPayload callback, Payload payload) {
  if (callback != NULL) callback(payload);
}
