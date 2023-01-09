/*
 * Copyright (c) 2023 Light Source Software, LLC. All rights reserved.
 */

#include "defs.h"

#include <stc/cstr.h>

const char* cstr_str_safe(const cstr* str) {
  const char* value = cstr_str(str);

  return value ? value : "";
}

void veil_abort(const char* filename, size_t line, const char* msg) {
  fprintf(stderr, "%s@%zu: %s", filename, line, msg);
  abort();
}
