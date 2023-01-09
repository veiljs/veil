/*
 * Copyright (c) 2023 Light Source Software, LLC. All rights reserved.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VEIL_VERSION_MAJOR 0
#define VEIL_VERSION_MINOR 1
#define VEIL_VERSION_PATCH 0
#define VEIL_VERSION_STR "0.1.0"
#define VEIL_VERSION_HEX  ((VEIL_VERSION_MAJOR << 16) | (VEIL_VERSION_MINOR <<  8) | (VEIL_VERSION_PATCH))

struct veil_s;
typedef struct veil_s veil_t;

typedef enum {
  VEIL_ESM_SPECIFIER_RESOLUTION_NODE,
  VEIL_ESM_SPECIFIER_RESOLUTION_EXPLICIT,
} veil_esm_specifier_resolution_t;

typedef enum {
  VEIL_INPUT_TYPE_MODULE,
  VEIL_INPUT_TYPE_COMMONJS
} veil_input_type_t;

typedef enum {
  VEIL_SCRIPT_OP_SPECIFIER,
  VEIL_SCRIPT_OP_EVAL,
  VEIL_SCRIPT_OP_PRINT,
} veil_script_op_t;

typedef struct veil_parse_args_result_s {
  bool ok;
  int exit_code;
} veil_parse_args_result_t;

int veil_main(int argc, char** argv);

veil_t* veil_init();

void veil_drop(veil_t* veil);

veil_parse_args_result_t veil_parse_args(veil_t* veil, int argc, char** argv);

int veil_run(veil_t* veil);

bool veil_cfg_get_no_deprecation(veil_t* veil);
void veil_cfg_set_no_deprecation(veil_t* veil, bool no_deprecation);

bool veil_cfg_get_throw_deprecation(veil_t* veil);
void veil_cfg_set_throw_deprecation(veil_t* veil, bool throw_deprecation);

bool veil_cfg_get_expose_gc(veil_t* veil);
void veil_cfg_set_expose_gc(veil_t* veil, bool expose_gc);

bool veil_cfg_get_expose_internals(veil_t* veil);
void veil_cfg_set_expose_internals(veil_t* veil, bool expose_internals);

bool veil_cfg_get_preserve_symlinks(veil_t* veil);
void veil_cfg_set_preserve_symlinks(veil_t* veil, bool preserve_symlinks);

bool veil_cfg_get_preserve_symlinks_main(veil_t* veil);
void veil_cfg_set_preserve_symlinks_main(veil_t* veil, bool preserve_symlinks_main);

const char* veil_cfg_get_loader(veil_t* veil);
void veil_cfg_set_loader(veil_t* veil, const char* loader);

const char* veil_cfg_get_script(veil_t* veil);
veil_script_op_t veil_cfg_get_script_op(veil_t* veil);
void veil_cfg_set_script(veil_t* veil, const char* script, veil_script_op_t op);

veil_input_type_t veil_cfg_get_input_type(veil_t* veil);
bool veil_cfg_set_input_type(veil_t* veil, veil_input_type_t input_type);
bool veil_cfg_set_input_type_str(veil_t* veil, const char* input_type);

veil_esm_specifier_resolution_t veil_cfg_get_esm_specifier_resolution(veil_t* veil);
bool veil_cfg_set_esm_specifier_resolution(veil_t* veil, veil_esm_specifier_resolution_t esm_specifier_resolution);
bool veil_cfg_set_esm_specifier_resolution_str(veil_t* veil, const char* esm_specifier_resolution);

size_t veil_cfg_get_conditions_count(veil_t* veil);
const char* veil_cfg_get_condition(veil_t* veil, size_t index);
void veil_cfg_add_condition(veil_t* veil, const char* condition);

size_t veil_cfg_get_require_count(veil_t* veil);
const char* veil_cfg_get_require(veil_t* veil, size_t index);
void veil_cfg_add_require(veil_t* veil, const char* require);

size_t veil_cfg_get_import_count(veil_t* veil);
const char* veil_cfg_get_import(veil_t* veil, size_t index);
void veil_cfg_add_import(veil_t* veil, const char* import);

const char* veil_cfg_get_argv0(veil_t* veil);
void veil_cfg_set_argv0(veil_t* veil, const char* argv0);

size_t veil_cfg_get_argv_count(veil_t* veil);
const char* veil_cfg_get_argv(veil_t* veil, size_t index);
void veil_cfg_add_argv(veil_t* veil, const char* argv);

size_t veil_cfg_get_exec_argv_count(veil_t* veil);
const char* veil_cfg_get_exec_argv(veil_t* veil, size_t index);
void veil_cfg_add_exec_argv(veil_t* veil, const char* exec_argv);

#ifdef __cplusplus
} /* extern "C" { */
#endif
