/*
 * Copyright (c) 2023 Light Source Software, LLC. All rights reserved.
 */

#pragma once

#include <veil.h>
#include <quickjs.h>
#include <uv.h>
#include <stc/cstr.h>
#include <stc/forward.h>
#include <stdlib.h>
#include <stdbool.h>
#include "util.h"

forward_cvec(cvec_str, cstr);

typedef struct veil_vm_s {
  bool enabled;
  JSRuntime* rt;
  JSContext* ctx;
} veil_vm_t;

typedef struct uv_microtask_context_s uv_microtask_context_t;
typedef bool (*uv_has_mircotasks_cb)(uv_microtask_context_t* context);
typedef void (*uv_run_mircotasks_cb)(uv_microtask_context_t* context);

typedef struct veil_uv_s {
  bool enabled;
  uv_loop_t loop;

  uv_prepare_t prepare_job;
  uv_idle_t idle_job;
  uv_check_t check_job;
  uv_async_t stop;

  uv_microtask_context_t* microtask_context;
  uv_has_mircotasks_cb has_microtasks_cb;
  uv_run_mircotasks_cb run_microtasks_cb;
} veil_uv_t;

typedef struct veil_cfg_s {
  bool writable;
  bool no_deprecation;
  bool throw_deprecation;
  bool expose_gc;
  bool expose_internals;
  bool preserve_symlinks;
  bool preserve_symlinks_main;
  cstr loader;
  cstr script;
  veil_input_type_t input_type;
  veil_script_op_t script_op;
  veil_esm_specifier_resolution_t esm_specifier_resolution;
  cvec_str conditions;
  cvec_str require;
  cvec_str import;

  cstr argv0;
  cvec_str argv;
  cvec_str exec_argv;
} veil_cfg_t;

struct veil_s {
  veil_cfg_t cfg;
  veil_uv_t uv;
  veil_vm_t vm;
};

void veil_cfg_init(veil_cfg_t* cfg);
void veil_cfg_drop(veil_cfg_t* cfg);

veil_parse_args_result_t veil_cfg_parse_args(veil_t* veil, int argc, char** argv);

void veil_uv_init(veil_uv_t* uv);
void veil_uv_drop(veil_uv_t* uv);
void veil_uv_run(veil_uv_t* uv);

void veil_vm_init(veil_vm_t* vm);
void veil_vm_drop(veil_vm_t* vm);

#define CHECK(EXPR) do { if (!(EXPR)) { veil_abort(__FILE__, __LINE__, #EXPR); } } while (0)
#define CHECK_OK(X) CHECK((X) == 0)
#define CHECK_EQ(X, Y) CHECK((X) == (Y))
#define CHECK_NULL(X) CHECK((X) == NULL)
#define CHECK_NOT_NULL(X) CHECK((X) != NULL)
