/*
 * Copyright (c) 2023 Light Source Software, LLC. All rights reserved.
 */

#include "defs.h"

static bool has_microtasks(uv_microtask_context_t* context);
static void run_microtasks(uv_microtask_context_t* context);

veil_t* veil_init() {
  veil_t* veil = calloc(1, sizeof(veil_t));
  CHECK_NOT_NULL(veil);

  veil_cfg_init(&veil->cfg);
  veil_uv_init(&veil->uv);
  veil_vm_init(&veil->vm);

  veil->uv.microtask_context = (uv_microtask_context_t*)&veil->vm;
  veil->uv.has_microtasks_cb = has_microtasks;
  veil->uv.run_microtasks_cb = run_microtasks;

  veil->cfg.writable = true;

  return veil;
}

void veil_drop(veil_t* veil) {
  veil_vm_drop(&veil->vm);
  veil_uv_drop(&veil->uv);
  veil_cfg_drop(&veil->cfg);
  free(veil);
}

veil_parse_args_result_t veil_parse_args(veil_t* veil, int argc, char** argv) {
  return veil_cfg_parse_args(veil, argc, argv);
}

int veil_run(veil_t* veil) {
  CHECK_NOT_NULL(veil);

  veil->cfg.writable = false;

  veil_uv_run(&veil->uv);

  veil_uv_drop(&veil->uv);
  veil_vm_drop(&veil->vm);

  return 0;
}

int veil_main(int argc, char** argv) {
  int exit_code;
  veil_t* veil = veil_init();
  veil_parse_args_result_t p = veil_parse_args(veil, argc, argv);

  if (p.ok) {
    exit_code = veil_run(veil);
  } else {
    exit_code = p.exit_code;
  }

  veil_drop(veil);

  return exit_code;
}

static bool has_microtasks(uv_microtask_context_t* context) {
  // TODO: check vm
  return false;
}

static void run_microtasks(uv_microtask_context_t* context) {
  // TODO: drain microtasks
}
