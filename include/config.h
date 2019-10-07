#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "lib.h"

b32 parse_int(char **str, i64 *ret);
b32 parse_i32(char **str, i32 *ret);

b32 parse_float(char **str, f64 *ret);
b32 parse_f32(char **str, f32 *ret);

b32 parse_bool(char **str, b32 *ret);
b32 parse_custom_bool(char **str, b32 *ret, char *true_str, char *false_str);
b32 parse_file_path(char **str, char **ret);
b32 parse_ident(char **str, char **ret);
void skip_space(char **str, b32 newline);

#endif
