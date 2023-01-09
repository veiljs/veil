/*
 * Copyright (c) 2023 Light Source Software, LLC. All rights reserved.
 */

#pragma once

#include <stc/forward.h>

const char* cstr_str_safe(const cstr* str);

void veil_abort(const char* filename, size_t line, const char* msg);
