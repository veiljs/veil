/*
 * Copyright (c) 2023 Light Source Software, LLC. All rights reserved.
 */

#include "defs.h"

static void notify(veil_uv_t* uv);
static void before_poll_io_cb(uv_prepare_t *handle);
static void after_poll_io_cb(uv_check_t* handle);
static void idle_cb(uv_idle_t *handle);

void veil_uv_init(veil_uv_t* uv) {
  CHECK_OK(uv_loop_init(&uv->loop));

  CHECK_OK(uv_prepare_init(&uv->loop, &uv->prepare_job));
  uv->prepare_job.data = uv;

  CHECK_OK(uv_idle_init(&uv->loop, &uv->idle_job));
  uv->idle_job.data = uv;

  CHECK_OK(uv_check_init(&uv->loop, &uv->check_job));
  uv->check_job.data = uv;

  CHECK_OK(uv_async_init(&uv->loop, &uv->stop, NULL));
  uv->stop.data = uv;

  uv->enabled = true;
}

void veil_uv_drop(veil_uv_t* uv) {
  if (!uv->enabled) {
    return;
  }

  uv_close((uv_handle_t*) &uv->prepare_job, NULL);
  uv_close((uv_handle_t*) &uv->check_job, NULL);
  uv_close((uv_handle_t*) &uv->idle_job, NULL);
  uv_close((uv_handle_t*) &uv->stop, NULL);

  uv_run(&uv->loop, UV_RUN_NOWAIT);
  CHECK_OK(uv_loop_close(&uv->loop));

  uv->enabled = false;
}

void veil_uv_run(veil_uv_t* uv) {
  CHECK_OK(uv_prepare_start(&uv->prepare_job, before_poll_io_cb));
  uv_unref((uv_handle_t*) &uv->prepare_job);

  CHECK_OK(uv_check_start(&uv->check_job, after_poll_io_cb));
  uv_unref((uv_handle_t*) &uv->check_job);

  uv_unref((uv_handle_t*) &uv->stop);

  notify(uv);
  uv_run(&uv->loop, UV_RUN_DEFAULT);
}

static void notify(veil_uv_t* uv) {
  if (uv->has_microtasks_cb(uv->microtask_context)) {
      uv_idle_start(&uv->idle_job, idle_cb);
  } else {
      uv_idle_stop(&uv->idle_job);
  }
}

static void before_poll_io_cb(uv_prepare_t* handle) {
  veil_uv_t* uv = handle->data;
  CHECK_NOT_NULL(uv);

  notify(uv);
}

static void after_poll_io_cb(uv_check_t* handle) {
  veil_uv_t* uv = handle->data;
  CHECK_NOT_NULL(uv);

  uv->run_microtasks_cb(uv->microtask_context);
  notify(uv);
}

static void idle_cb(uv_idle_t* handle) {
  (void) handle;
}
