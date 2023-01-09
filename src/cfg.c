/*
 * Copyright (c) 2023 Light Source Software, LLC. All rights reserved.
 */

#include "defs.h"
#include <stc/coption.h>

#define i_val_str
#define i_opt (c_no_cmp | c_is_fwd)
#include <stc/cvec.h>

#define PARSE_RESULT_OK() (veil_parse_args_result_t) { .ok = true, .exit_code = 0 }
#define PARSE_RESULT_EXIT() (veil_parse_args_result_t) { .ok = false, .exit_code = 0 }
#define PARSE_RESULT_ERR(EXIT_CODE) (veil_parse_args_result_t) { .ok = false, .exit_code = EXIT_CODE }

static void print_help();
static void print_version();
static const char* cvec_str_get_or_empty(const cvec_str* vec, size_t index);

typedef enum {
  // stc/coption status
  OPT_STATUS_UNKNOWN = (int)'?',
  OPT_STATUS_MISSING = (int)':',
  OPT_STATUS_END = -1,

  // cli support
  OPT_HELP = (int)'h',
  OPT_VERSION = (int)'v',

  // veil options
  OPT_CONDITIONS = (int)'C',
  OPT_REQUIRE = (int)'r',
  OPT_EVAL = (int)'e',
  OPT_PRINT = (int)'p',
  OPT_EXPOSE_GC = 0x103,
  OPT_EXPOSE_INTERNALS = 0x105,
  OPT_PRESERVE_SYMLINKS = 0x106,
  OPT_PRESERVE_SYMLINKS_MAIN = 0x107,
  OPT_LOADER = 0x108,
  OPT_ESM_SPECIFIER_RESOLUTION = 0x109,
  OPT_INPUT_TYPE = 0x10A,
  OPT_NO_DEPRECATION = 0x10B,
  OPT_THROW_DEPRECATION = 0x10C,
  OPT_IMPORT = 0x10D,
} cli_option_id_t;

static const char* OPTS_SHORT = "+hvr:e:p:C:dw:";

static coption_long OPTS_LONG[] = {
    { "help", coption_no_argument, OPT_HELP },
    { "version", coption_no_argument, OPT_VERSION },
    { "expose-gc", coption_no_argument, OPT_EXPOSE_GC },
    { "no-deprecation", coption_no_argument, OPT_NO_DEPRECATION },
    { "throw-deprecation", coption_no_argument, OPT_THROW_DEPRECATION },
    { "expose-internals", coption_no_argument, OPT_EXPOSE_INTERNALS },
    { "preserve-symlinks", coption_no_argument, OPT_PRESERVE_SYMLINKS },
    { "preserve-symlinks-main", coption_no_argument, OPT_PRESERVE_SYMLINKS_MAIN },
    { "loader", coption_required_argument, OPT_LOADER },
    { "conditions", coption_required_argument, OPT_CONDITIONS },
    { "require", coption_required_argument, OPT_REQUIRE },
    { "import", coption_required_argument, OPT_IMPORT },
    { "eval", coption_required_argument, OPT_EVAL },
    { "print", coption_required_argument, OPT_PRINT },
    { "input-type", coption_required_argument, OPT_INPUT_TYPE },
    { "es-module-specifier-resolution", coption_required_argument, OPT_ESM_SPECIFIER_RESOLUTION },
    {0}
};

void veil_cfg_init(veil_cfg_t* cfg) {
  cfg->expose_gc = false;
  cfg->no_deprecation = false;
  cfg->throw_deprecation = false;
  cfg->expose_gc = false;
  cfg->expose_internals = false;
  cfg->preserve_symlinks = false;
  cfg->preserve_symlinks_main = false;
  cfg->loader = cstr_init();
  cfg->script = cstr_init();
  cfg->input_type = VEIL_INPUT_TYPE_COMMONJS;
  cfg->script_op = VEIL_SCRIPT_OP_SPECIFIER;
  cfg->conditions = cvec_str_init();
  cfg->require = cvec_str_init();
  cfg->import = cvec_str_init();
  cfg->esm_specifier_resolution = VEIL_ESM_SPECIFIER_RESOLUTION_NODE;
  cfg->argv0 = cstr_init();
  cfg->argv = cvec_str_init();
  cfg->exec_argv = cvec_str_init();
}

void veil_cfg_drop(veil_cfg_t* cfg) {
  cstr_drop(&cfg->loader);
  cstr_drop(&cfg->script);
  cvec_str_drop(&cfg->conditions);
  cvec_str_drop(&cfg->require);
  cvec_str_drop(&cfg->import);
  cstr_drop(&cfg->argv0);
  cvec_str_drop(&cfg->argv);
  cvec_str_drop(&cfg->exec_argv);

  memset(cfg, 0, sizeof(veil_cfg_t));
}

veil_parse_args_result_t veil_cfg_parse_args(veil_t* veil, int argc, char** argv) {
  int32_t value;
  coption opt = coption_init();

  while ((value = coption_get(&opt, argc, argv, OPTS_SHORT, OPTS_LONG)) != OPT_STATUS_END) {
    switch (value) {
      case OPT_EXPOSE_GC:
        veil_cfg_set_expose_gc(veil, true);
        break;
      case OPT_NO_DEPRECATION:
        veil_cfg_set_no_deprecation(veil, true);
        break;
      case OPT_THROW_DEPRECATION:
        veil_cfg_set_throw_deprecation(veil, true);
        break;
      case OPT_PRESERVE_SYMLINKS:
        veil_cfg_set_preserve_symlinks(veil, true);
        break;
      case OPT_PRESERVE_SYMLINKS_MAIN:
        veil_cfg_set_preserve_symlinks_main(veil, true);
        break;
      case OPT_EXPOSE_INTERNALS:
        veil_cfg_set_expose_internals(veil, true);
        break;
      case OPT_LOADER:
        veil_cfg_set_loader(veil, opt.arg);
        break;
      case OPT_REQUIRE:
        veil_cfg_add_require(veil, opt.arg);
        break;
      case OPT_IMPORT:
        veil_cfg_add_import(veil, opt.arg);
        break;
      case OPT_EVAL:
        veil_cfg_set_script(veil, opt.arg, VEIL_SCRIPT_OP_EVAL);
        break;
      case OPT_PRINT:
        veil_cfg_set_script(veil, opt.arg, VEIL_SCRIPT_OP_PRINT);
        break;
      case OPT_INPUT_TYPE:
        if (!veil_cfg_set_input_type_str(veil, opt.arg)) {
          fprintf(stderr, "veil: --input-type must be \"module\" or \"commonjs\"");
            return PARSE_RESULT_ERR(1);
        }
        break;
      case OPT_CONDITIONS:
        veil_cfg_add_condition(veil, opt.arg);
        break;
      case OPT_ESM_SPECIFIER_RESOLUTION:
        if (!veil_cfg_set_esm_specifier_resolution_str(veil, opt.arg)) {
          fprintf(stderr, "veil: --es-module-specifier-resolution must be \"node\" or \"explicit\"");
          return PARSE_RESULT_ERR(1);
        }
        break;
      case OPT_HELP:
        print_help();
        return PARSE_RESULT_EXIT();
      case OPT_VERSION:
        print_version();
        return PARSE_RESULT_EXIT();
      case OPT_STATUS_MISSING:
        printf("veil: %s requires an argument\n", opt.optstr);
        return PARSE_RESULT_ERR(1);
      default:
        printf("veil: unknown option: %s\n", opt.optstr);
        return PARSE_RESULT_ERR(1);
    }
  }

  int32_t non_option_index = opt.ind;

  // argv0
  veil_cfg_set_argv0(veil, argv[0]);

  // exec_argv
  for (size_t n = 1; n < non_option_index; n++) {
    veil_cfg_add_exec_argv(veil, argv[n]);
  }

  // capture specifier/filename arg
  if (veil_cfg_get_script_op(veil) == VEIL_SCRIPT_OP_SPECIFIER) {
    if (non_option_index < argc) {
      veil_cfg_set_script(veil, argv[non_option_index], VEIL_SCRIPT_OP_SPECIFIER);
      non_option_index++;
    } else {
      fprintf(stderr, "veil: no filename specified\n");
      return PARSE_RESULT_ERR(1);
    }
  }

  // argv
  for (size_t n = non_option_index; n < argc; n++) {
    veil_cfg_add_argv(veil, argv[n]);
  }

  return PARSE_RESULT_OK();
}

bool veil_cfg_get_no_deprecation(veil_t* veil) {
  return veil->cfg.no_deprecation;
}

void veil_cfg_set_no_deprecation(veil_t* veil, bool no_deprecation) {
  veil->cfg.no_deprecation = no_deprecation;
}

bool veil_cfg_get_throw_deprecation(veil_t* veil) {
  return veil->cfg.throw_deprecation;
}

void veil_cfg_set_throw_deprecation(veil_t* veil, bool throw_deprecation) {
  veil->cfg.throw_deprecation = throw_deprecation;
}

bool veil_cfg_get_expose_gc(veil_t* veil) {
  return veil->cfg.expose_gc;
}

void veil_cfg_set_expose_gc(veil_t* veil, bool expose_gc) {
  veil->cfg.expose_gc = expose_gc;
}

bool veil_cfg_get_preserve_symlinks(veil_t* veil) {
  return veil->cfg.preserve_symlinks;
}

void veil_cfg_set_preserve_symlinks(veil_t* veil, bool preserve_symlinks) {
  veil->cfg.preserve_symlinks = preserve_symlinks;
}

bool veil_cfg_get_preserve_symlinks_main(veil_t* veil) {
  return veil->cfg.preserve_symlinks_main;
}

void veil_cfg_set_preserve_symlinks_main(veil_t* veil, bool preserve_symlinks_main) {
  veil->cfg.preserve_symlinks_main = preserve_symlinks_main;
}

bool veil_cfg_get_expose_internals(veil_t* veil) {
  return veil->cfg.expose_internals;
}

void veil_cfg_set_expose_internals(veil_t* veil, bool expose_internals) {
  veil->cfg.expose_internals = expose_internals;
}

const char* veil_cfg_get_loader(veil_t* veil) {
  return cstr_str_safe(&veil->cfg.loader);
}

void veil_cfg_set_loader(veil_t* veil, const char* loader) {
  cstr_assign(&veil->cfg.loader, loader);
}

const char* veil_cfg_get_script(veil_t* veil) {
  return cstr_str_safe(&veil->cfg.script);
}

veil_script_op_t veil_cfg_get_script_op(veil_t* veil) {
  return veil->cfg.script_op;
}

void veil_cfg_set_script(veil_t* veil, const char* script, veil_script_op_t op) {
  cstr_assign(&veil->cfg.script, script);
  veil->cfg.script_op = op;
}

veil_input_type_t veil_cfg_get_input_type(veil_t* veil) {
  return veil->cfg.input_type;
}

bool veil_cfg_set_input_type(veil_t* veil, veil_input_type_t input_type) {
  veil->cfg.input_type = input_type;
  return true;
}

bool veil_cfg_set_input_type_str(veil_t* veil, const char* input_type) {
  if (strcmp(input_type, "module") == 0) {
    return veil_cfg_set_input_type(veil, VEIL_INPUT_TYPE_MODULE);
  } else if (strcmp(input_type, "commonjs") == 0) {
    return veil_cfg_set_input_type(veil, VEIL_INPUT_TYPE_COMMONJS);
  } else {
    return false;
  }
}

veil_esm_specifier_resolution_t veil_cfg_get_esm_specifier_resolution(veil_t* veil) {
  return veil->cfg.esm_specifier_resolution;
}

bool veil_cfg_set_esm_specifier_resolution(veil_t* veil, veil_esm_specifier_resolution_t esm_specifier_resolution) {
  veil->cfg.esm_specifier_resolution = esm_specifier_resolution;
  return true;
}

bool veil_cfg_set_esm_specifier_resolution_str(veil_t* veil, const char* esm_specifier_resolution) {
  if (strcmp("explicit", esm_specifier_resolution) == 0) {
    veil->cfg.esm_specifier_resolution = VEIL_ESM_SPECIFIER_RESOLUTION_EXPLICIT;
  } else if (strcmp("node", esm_specifier_resolution) == 0) {
    veil->cfg.esm_specifier_resolution = VEIL_ESM_SPECIFIER_RESOLUTION_NODE;
  } else {
    return false;
  }
  return true;
}

size_t veil_cfg_get_conditions_count(veil_t* veil) {
  return cvec_str_size(&veil->cfg.conditions);
}

const char* veil_cfg_get_condition(veil_t* veil, size_t index) {
  return cvec_str_get_or_empty(&veil->cfg.conditions, index);
}

void veil_cfg_add_condition(veil_t* veil, const char* condition) {
  cvec_str_emplace_back(&veil->cfg.conditions, condition);
}

size_t veil_cfg_get_require_count(veil_t* veil) {
  return cvec_str_size(&veil->cfg.require);
}

const char* veil_cfg_get_require(veil_t* veil, size_t index) {
  return cvec_str_get_or_empty(&veil->cfg.require, index);
}

void veil_cfg_add_require(veil_t* veil, const char* specifier) {
  cvec_str_emplace_back(&veil->cfg.require, specifier);
}

size_t veil_cfg_get_import_count(veil_t* veil) {
  return cvec_str_size(&veil->cfg.import);
}

const char* veil_cfg_get_import(veil_t* veil, size_t index) {
  return cvec_str_get_or_empty(&veil->cfg.import, index);
}

void veil_cfg_add_import(veil_t* veil, const char* specifier) {
  cvec_str_emplace_back(&veil->cfg.import, specifier);
}

const char* veil_cfg_get_argv0(veil_t* veil) {
  return cstr_str_safe(&veil->cfg.argv0);
}

void veil_cfg_set_argv0(veil_t* veil, const char* argv0) {
  cstr_assign(&veil->cfg.argv0, argv0);
}

size_t veil_cfg_get_argv_count(veil_t* veil) {
  return cvec_str_size(&veil->cfg.argv);
}

const char* veil_cfg_get_argv(veil_t* veil, size_t index) {
  return cvec_str_get_or_empty(&veil->cfg.argv, index);
}

void veil_cfg_add_argv(veil_t* veil, const char* argv) {
  cvec_str_emplace_back(&veil->cfg.argv, argv);
}

size_t veil_cfg_get_exec_argv_count(veil_t* veil) {
  return cvec_str_size(&veil->cfg.exec_argv);
}

const char* veil_cfg_get_exec_argv(veil_t* veil, size_t index) {
  return cvec_str_get_or_empty(&veil->cfg.exec_argv, index);
}

void veil_cfg_add_exec_argv(veil_t* veil, const char* argv) {
  cvec_str_emplace_back(&veil->cfg.exec_argv, argv);
}

static const char* cvec_str_get_or_empty(const cvec_str* vec, size_t index) {
  if (index >= cvec_str_size(vec)) {
      return "";
  }

  const cstr* value = cvec_str_at(vec, index);

  return cstr_str_safe(value);
}

static void print_help() {
  printf("Usage: veil [options] [script.js] [arguments]\n\nOptions:\n");
  // node-like options
  printf("  -h, --help                      print this help and exit                      \n");
  printf("  -v, --version                   print veil version                            \n");
  printf("  -e, --eval=...                  evaluate script                               \n");
  printf("  --expose-gc                     expose gc() function in global namespace      \n");
  printf("  --input-type=...                set module type for string input              \n");
  printf("  --no-addon                      disable loading native addons                 \n");
  printf("  --no-deprecation                silence deprecation warnings                  \n");
  printf("  --throw-deprecation             throw an exception on deprecations            \n");
  printf("  --expose-internals              enable importing of internal builtin modules  \n");
  printf("  --preserve-symlinks             preserve symbolic links when resolving        \n");
  printf("  --preserve-symlinks-main        preserve symbolic links when resolving        \n"
         "                                  the main module                               \n");
  printf("   -p, --print [...]              evaluate script and print result              \n");
  printf("  --loader                        use the specified module as a custom loader   \n");
  printf("  -C, --conditions=...            additional user conditions for conditional    \n"
         "                                  exports and imports                           \n");
  printf("  -r, --require=...               CJS module to preload (option can be repeated)\n");
  printf("  --import=...                    ES module to preload (option can be repeated) \n");
  printf("  --es-module-specifier-resolution=...                                          \n"
         "                                  select extension resolution algorithm for es  \n"
         "                                  modules; either 'explicit' (default) or 'node'\n");
  printf("\nEnvironment variables:\n\n");
  printf("UV_THREADPOOL_SIZE                sets the number of threads used in libuv's    \n"
         "                                  threadpool                                    \n");
  printf("\n");
}

static void print_version() {
  printf("%s\n", VEIL_VERSION_STR);
}
