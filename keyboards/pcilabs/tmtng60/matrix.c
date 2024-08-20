#include <stdbool.h>

void matrix_init_custom(void) {
  // TODO: initialize hardware here
}

bool matrix_scan_custom(void* dump) {
  bool matrix_has_changed = false;

  // TODO: add matrix scanning routine here

  return matrix_has_changed;
}
