#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>

#include "glmath2.h"
#include "lib.h"

char *load_file(const char *file_path, i64 *size);
void get_direction_angles(const Vec3f dir, float *h_angle, float *v_angle);

#endif // _UTIL_H_
