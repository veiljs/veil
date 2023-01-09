/*
 * Copyright (c) 2023 Light Source Software, LLC. All rights reserved.
 */

#include "defs.h"

void veil_vm_init(veil_vm_t* vm) {
  vm->runtime = JS_NewRuntime();
  CHECK_NOT_NULL(vm->runtime);
  JS_SetRuntimeOpaque(vm->runtime, vm);

  vm->context = JS_NewContext(vm->runtime);
  CHECK_NOT_NULL(vm->context);
  JS_SetContextOpaque(vm->context, vm);

  vm->enabled = true;
}

void veil_vm_drop(veil_vm_t* vm) {
  if (!vm->enabled) {
    return;
  }

  JS_FreeContext(vm->context);
  JS_FreeRuntime(vm->runtime);
  vm->enabled = false;
}

void veil_vm_drain_microtasks(veil_vm_t* vm) {
  int err;
  JSContext* last = NULL;

  for(;;) {
    err = JS_ExecutePendingJob(vm->runtime, &last);
    if (err <= 0) {
      if (err < 0) {
        // TODO: handle error
      }
      break;
    }
  }
}
