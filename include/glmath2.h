#ifndef _GL_MATH_H
#define _GL_MATH_H

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <math.h>

#include "lib.h"

#define RAD(x) ((double)(x) / 57.295779513082321)
#define DEG(x) ((double)(x) * 57.295779513082321)

#define _VECTOR_OPS_SET_ARGS1(t)                            t x
#define _VECTOR_OPS_SET_ARGS2(t)  _VECTOR_OPS_SET_ARGS1(t), t y
#define _VECTOR_OPS_SET_ARGS3(t)  _VECTOR_OPS_SET_ARGS2(t), t z
#define _VECTOR_OPS_SET_ARGS4(t)  _VECTOR_OPS_SET_ARGS3(t), t w
#define _VECTOR_OPS_SET_ARGS5(t)  _VECTOR_OPS_SET_ARGS4(t), t a
#define _VECTOR_OPS_SET_ARGS6(t)  _VECTOR_OPS_SET_ARGS5(t), t b
#define _VECTOR_OPS_SET_ARGS7(t)  _VECTOR_OPS_SET_ARGS6(t), t c
#define _VECTOR_OPS_SET_ARGS8(t)  _VECTOR_OPS_SET_ARGS7(t), t d
#define _VECTOR_OPS_SET_ARGS9(t)  _VECTOR_OPS_SET_ARGS8(t), t e
#define _VECTOR_OPS_SET_ARGS10(t) _VECTOR_OPS_SET_ARGS9(t), t f

#define _VECTOR_OPS_SET_ASSIGN1(v)                              v.data[0] = x
#define _VECTOR_OPS_SET_ASSIGN2(v)  _VECTOR_OPS_SET_ASSIGN1(v); v.data[1] = y
#define _VECTOR_OPS_SET_ASSIGN3(v)  _VECTOR_OPS_SET_ASSIGN2(v); v.data[2] = z
#define _VECTOR_OPS_SET_ASSIGN4(v)  _VECTOR_OPS_SET_ASSIGN3(v); v.data[3] = w
#define _VECTOR_OPS_SET_ASSIGN5(v)  _VECTOR_OPS_SET_ASSIGN4(v); v.data[4] = a
#define _VECTOR_OPS_SET_ASSIGN6(v)  _VECTOR_OPS_SET_ASSIGN5(v); v.data[5] = b
#define _VECTOR_OPS_SET_ASSIGN7(v)  _VECTOR_OPS_SET_ASSIGN6(v); v.data[6] = c
#define _VECTOR_OPS_SET_ASSIGN8(v)  _VECTOR_OPS_SET_ASSIGN7(v); v.data[7] = d
#define _VECTOR_OPS_SET_ASSIGN9(v)  _VECTOR_OPS_SET_ASSIGN8(v); v.data[8] = e
#define _VECTOR_OPS_SET_ASSIGN10(v) _VECTOR_OPS_SET_ASSIGN9(v); v.data[9] = f

#define _VECTOR_OPS_UNION_ELEMS1(t)  union{t x; t u; t r;}
#define _VECTOR_OPS_UNION_ELEMS2(t)  _VECTOR_OPS_UNION_ELEMS1(t); union {t y; t v; t g;}
#define _VECTOR_OPS_UNION_ELEMS3(t)  _VECTOR_OPS_UNION_ELEMS2(t); union {t z; t b;}
#define _VECTOR_OPS_UNION_ELEMS4(t)  _VECTOR_OPS_UNION_ELEMS3(t); union {t w; t a;}
#define _VECTOR_OPS_UNION_ELEMS5(t)  _VECTOR_OPS_UNION_ELEMS4(t)
#define _VECTOR_OPS_UNION_ELEMS6(t)  _VECTOR_OPS_UNION_ELEMS4(t)
#define _VECTOR_OPS_UNION_ELEMS7(t)  _VECTOR_OPS_UNION_ELEMS4(t)
#define _VECTOR_OPS_UNION_ELEMS8(t)  _VECTOR_OPS_UNION_ELEMS4(t)
#define _VECTOR_OPS_UNION_ELEMS9(t)  _VECTOR_OPS_UNION_ELEMS4(t)
#define _VECTOR_OPS_UNION_ELEMS10(t) _VECTOR_OPS_UNION_ELEMS4(t)

#define VECTOR_OPS_TYPE(T, S, type)                 \
    typedef struct Vec##S##T                        \
    {                                               \
        union                                       \
        {                                           \
            type data[S];                           \
            struct                                  \
            {                                       \
                _VECTOR_OPS_UNION_ELEMS##S(type);   \
            };                                      \
        };                                          \
    } Vec##S##T;

#define VECTOR_OPS_DEC(T, S, type)                                  \
    Vec##S##T make_vec##S##T();                                     \
    Vec##S##T init_vec##S##T(_VECTOR_OPS_SET_ARGS##S(type));        \
    Vec##S##T copy_vec##S##T(Vec##S##T vec);                        \
    Vec##S##T conv_vec##S##T(type *data, int size);                 \
    void vec##S##T##_fill(Vec##S##T *vec, type val);                \
    void vec##S##T##_zero(Vec##S##T *vec);                          \
    void vec##S##T##_set(Vec##S##T *vec, int i, type x);            \
    type vec##S##T##_get(Vec##S##T vec, int i);                     \
    double vec##S##T##_mag(Vec##S##T vec);                          \
    Vec##S##T vec##S##T##_normalize(Vec##S##T vec);                 \
    Vec##S##T vec##S##T##_add(Vec##S##T vec_a, Vec##S##T vec_b);    \
    Vec##S##T vec##S##T##_sub(Vec##S##T vec_a, Vec##S##T vec_b);    \
    Vec##S##T vec##S##T##_mul(Vec##S##T vec_a, Vec##S##T vec_b);    \
    Vec##S##T vec##S##T##_div(Vec##S##T vec_a, Vec##S##T vec_b);    \
    Vec##S##T vec##S##T##_scale(Vec##S##T vec_a, double x);         \
    Vec##S##T vec##S##T##_add_constant(Vec##S##T vec_a, type x);    \
    Vec##S##T vec##S##T##_cross(Vec##S##T vec_a, Vec##S##T vec_b);  \
    type vec##S##T##_dot(Vec##S##T vec_a, Vec##S##T vec_b);         \
    type vec##S##T##_min(Vec##S##T vec);                            \
    int vec##S##T##_min_index(Vec##S##T vec);                       \
    type vec##S##T##_max(Vec##S##T vec);                            \
    int vec##S##T##_max_index(Vec##S##T vec);                       \
    b32 vec##S##T##_equal(Vec##S##T vec_a, Vec##S##T vec_b);        \
    b32 vec##S##T##_is_pos(Vec##S##T vec);                          \
    b32 vec##S##T##_is_neg(Vec##S##T vec);                          \
    b32 vec##S##T##_is_zero(Vec##S##T vec);                         \
    Vec##S##T vec##S##T##_mul_mat(Vec##S##T vec, Mat##S##T mat);    \
    void vec##S##T##_print(Vec##S##T vec);                          \
    void vec##S##T##_pprint(Vec##S##T vec, const char *name);

#define VECTOR_OPS_DEF(T, S, type, format_spec)                         \
    force_inline Vec##S##T make_vec##S##T()                             \
    {                                                                   \
        Vec##S##T vec = {0};                                            \
        return vec;                                                     \
    }                                                                   \
                                                                        \
    force_inline Vec##S##T init_vec##S##T(_VECTOR_OPS_SET_ARGS##S(type)) \
    {                                                                   \
        Vec##S##T vec = {0};                                            \
        _VECTOR_OPS_SET_ASSIGN##S(vec);                                 \
        return vec;                                                     \
    }                                                                   \
                                                                        \
    force_inline Vec##S##T make_vec##S##T##_fill(type val)              \
    {                                                                   \
        Vec##S##T res = make_vec##S##T();                               \
        vec##S##T##_fill(&res, val);                                    \
        return res;                                                     \
    }                                                                   \
                                                                        \
    force_inline Vec##S##T copy_vec##S##T(Vec##S##T vec)                \
    {                                                                   \
        Vec##S##T copy = make_vec##S##T();                              \
        memcpy(copy.data, vec.data, sizeof(type)*S);                    \
        return copy;                                                    \
    }                                                                   \
                                                                        \
    force_inline Vec##S##T conv_vec##S##T(type *data, int size)         \
    {                                                                   \
        Vec##S##T conv = make_vec##S##T();                              \
        memcpy(conv.data, data, sizeof(type)*(S>size?size:S));          \
        return conv;                                                    \
    }                                                                   \
                                                                        \
    force_inline void vec##S##T##_fill(Vec##S##T *vec, type val)        \
    {                                                                   \
        for (int i = 0; i < S; i++)                                     \
            vec->data[i] = val;                                         \
    }                                                                   \
                                                                        \
    force_inline void vec##S##T##_zero(Vec##S##T *vec)                  \
    {                                                                   \
        vec##S##T##_fill(vec, 0);                                       \
    }                                                                   \
                                                                        \
    force_inline void vec##S##T##_set(Vec##S##T *vec, int i, type val)  \
    {                                                                   \
        vec->data[i] = val;                                             \
    }                                                                   \
                                                                        \
    force_inline type vec##S##T##_get(Vec##S##T vec, int i)             \
    {                                                                   \
        return vec.data[i];                                             \
    }                                                                   \
                                                                        \
    double vec##S##T##_mag(Vec##S##T vec)                               \
    {                                                                   \
        double mag = 0;                                                 \
        for (int i = 0; i < S; i++)                                     \
            mag += vec.data[i]*vec.data[i];                             \
        return sqrt(mag);                                               \
    }                                                                   \
                                                                        \
    Vec##S##T vec##S##T##_normalize(Vec##S##T vec)                      \
    {                                                                   \
        Vec##S##T normal = make_vec##S##T();                            \
                                                                        \
        double mag = vec##S##T##_mag(vec);                              \
        if (!mag) return normal;                                        \
        return vec##S##T##_scale(vec, 1.0/mag);                         \
    }                                                                   \
                                                                        \
    Vec##S##T vec##S##T##_add(Vec##S##T vec_a, Vec##S##T vec_b)         \
    {                                                                   \
        Vec##S##T res = make_vec##S##T();                               \
        for (int i = 0; i < S; i++)                                     \
            res.data[i] = vec_a.data[i] + vec_b.data[i];                \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Vec##S##T vec##S##T##_sub(Vec##S##T vec_a, Vec##S##T vec_b)         \
    {                                                                   \
        Vec##S##T res = make_vec##S##T();                               \
        for (int i = 0; i < S; i++)                                     \
            res.data[i] = vec_a.data[i] - vec_b.data[i];                \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Vec##S##T vec##S##T##_mul(Vec##S##T vec_a, Vec##S##T vec_b)         \
    {                                                                   \
        Vec##S##T res = make_vec##S##T();                               \
        for (int i = 0; i < S; i++)                                     \
            res.data[i] = vec_a.data[i] * vec_b.data[i];                \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Vec##S##T vec##S##T##_div(Vec##S##T vec_a, Vec##S##T vec_b)         \
    {                                                                   \
        Vec##S##T res = make_vec##S##T();                               \
        for (int i = 0; i < S; i++)                                     \
            res.data[i] = vec_a.data[i] / vec_b.data[i];                \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Vec##S##T vec##S##T##_scale(Vec##S##T vec_a, double x)              \
    {                                                                   \
        Vec##S##T res = make_vec##S##T();                               \
        for (int i = 0; i < S; i++)                                     \
            res.data[i] = vec_a.data[i] * x;                            \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Vec##S##T vec##S##T##_add_constant(Vec##S##T vec_a, type x)         \
    {                                                                   \
        Vec##S##T res = make_vec##S##T();                               \
        for (int i = 0; i < S; i++)                                     \
            res.data[i] = vec_a.data[i] + x;                            \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Vec##S##T vec##S##T##_cross(Vec##S##T vec_a, Vec##S##T vec_b)       \
    {                                                                   \
        assert(S == 3);                                                 \
        Vec##S##T res = make_vec##S##T();                               \
                                                                        \
        res.data[0] = vec_a.data[1]*vec_b.data[2] - vec_b.data[1]*vec_a.data[2]; \
        res.data[1] = vec_a.data[2]*vec_b.data[0] - vec_b.data[2]*vec_a.data[0]; \
        res.data[2] = vec_a.data[0]*vec_b.data[1] - vec_b.data[0]*vec_a.data[1]; \
                                                                        \
        return res;                                                     \
    }                                                                   \
                                                                        \
    type vec##S##T##_dot(Vec##S##T vec_a, Vec##S##T vec_b)              \
    {                                                                   \
        type ret = 0;                                                   \
        for (int i = 0; i < S; i++)                                     \
            ret += vec_a.data[i]*vec_b.data[i];                         \
        return ret;                                                     \
    }                                                                   \
                                                                        \
    type vec##S##T##_min(Vec##S##T vec)                                 \
    {                                                                   \
        type ret = vec.data[0];                                         \
        for (int i = 1; i < S; i++)                                     \
            ret = ret < vec.data[i] ? ret : vec.data[i];                \
        return ret;                                                     \
    }                                                                   \
                                                                        \
    int vec##S##T##_min_index(Vec##S##T vec)                            \
    {                                                                   \
        int ret = 0;                                                    \
        for (int i = 1; i < S; i++)                                     \
            ret = vec.data[ret] < vec.data[i] ? ret : i;                \
        return ret;                                                     \
    }                                                                   \
                                                                        \
    type vec##S##T##_max(Vec##S##T vec)                                 \
    {                                                                   \
        type ret = vec.data[0];                                         \
        for (int i = 1; i < S; i++)                                     \
            ret = ret > vec.data[i] ? ret : vec.data[i];                \
        return ret;                                                     \
    }                                                                   \
                                                                        \
    int vec##S##T##_max_index(Vec##S##T vec)                            \
    {                                                                   \
        int ret = 0;                                                    \
        for (int i = 1; i < S; i++)                                     \
            ret = vec.data[ret] > vec.data[i] ? ret : i;                \
        return ret;                                                     \
    }                                                                   \
                                                                        \
    b32 vec##S##T##_equal(Vec##S##T vec_a, Vec##S##T vec_b)             \
    {                                                                   \
        for (int i = 0; i < S; i++)                                     \
            if (vec_a.data[i] != vec_b.data[i]) return 0;               \
        return 1;                                                       \
    }                                                                   \
                                                                        \
    b32 vec##S##T##_is_pos(Vec##S##T vec)                               \
    {                                                                   \
        for (int i = 0; i < S; i++)                                     \
            if (vec.data[i] < 0) return 0;                              \
        return 1;                                                       \
    }                                                                   \
                                                                        \
    b32 vec##S##T##_is_neg(Vec##S##T vec)                               \
    {                                                                   \
        for (int i = 0; i < S; i++)                                     \
            if (vec.data[i] >= 0) return 0;                             \
        return 1;                                                       \
    }                                                                   \
                                                                        \
    b32 vec##S##T##_is_zero(Vec##S##T vec)                              \
    {                                                                   \
        for (int i = 0; i < S; i++)                                     \
            if (vec.data[i] != 0) return 0;                             \
        return 1;                                                       \
    }                                                                   \
                                                                        \
    Vec##S##T vec##S##T##_mul_mat(Vec##S##T vec, Mat##S##T mat)         \
    {                                                                   \
        Vec##S##T res = make_vec##S##T();                               \
        for (int i = 0; i < S; i++)                                     \
            for (int vi = 0; vi < S; vi++)                              \
                res.data[i] += vec.data[vi] * mat.data[i*S+vi];         \
        return res;                                                     \
    }                                                                   \
                                                                        \
    void vec##S##T##_print(Vec##S##T vec)                               \
    {                                                                   \
        for (int i = 0; i < S; i++)                                     \
            printf("%"format_spec"%s",                                  \
                   vec.data[i],                                         \
                   i < S-1 ? ", " : "");                                \
    }                                                                   \
                                                                        \
    void vec##S##T##_pprint(Vec##S##T vec, const char *name)            \
    {                                                                   \
        printf("%s:\n  [", name);                                       \
        vec##S##T##_print(vec);                                         \
        printf("]\n");                                                  \
    }


                                                             
#define VECTOR_OPS(T, S, type, format_spec)     \
    VECTOR_OPS_DEC(T, S, type);                 \
    VECTOR_OPS_DEF(T, S, type, format_spec);


#define MATRIX_OPS_TYPE(T, S, type)             \
    typedef struct Mat##S##T                    \
    {                                           \
        type data[S*S];                         \
    } Mat##S##T;

#define MATRIX_OPS_DEC(T, S, type)                                      \
    Mat##S##T make_mat##S##T();                                         \
    Mat##S##T init_mat##S##T(type val);                                 \
    Mat##S##T make_mat##S##T##_fill(type val);                          \
    Mat##S##T copy_mat##S##T(Mat##S##T mat);                            \
    type mat##S##T##_get(const Mat##S##T mat, int i, int j);            \
    void mat##S##T##_set(Mat##S##T *mat, int i, int j, type x);         \
    void mat##S##T##_set_row(Mat##S##T *mat, int i, Vec##S##T vec);     \
    void mat##S##T##_set_col(Mat##S##T *mat, int j, Vec##S##T vec);     \
    void mat##S##T##_set_diag(Mat##S##T *mat, type val);                \
    void mat##S##T##_fill(Mat##S##T *mat, type val);                    \
    void mat##S##T##_zero(Mat##S##T *mat);                              \
    void mat##S##T##_identity(Mat##S##T *mat);                          \
    Mat##S##T mat##S##T##_transpose(Mat##S##T mat);                     \
    Mat##S##T mat##S##T##_add(Mat##S##T mat_a, Mat##S##T mat_b);        \
    Mat##S##T mat##S##T##_sub(Mat##S##T mat_a, Mat##S##T mat_b);        \
    Mat##S##T mat##S##T##_mul(Mat##S##T mat_a, Mat##S##T mat_b);        \
    Mat##S##T mat##S##T##_mul_elems(Mat##S##T mat_a, Mat##S##T mat_b);  \
    Mat##S##T mat##S##T##_div_elems(Mat##S##T mat_a, Mat##S##T mat_b);  \
    Mat##S##T mat##S##T##_add_constant(Mat##S##T mat, type x);          \
    type mat##S##T##_min(Mat##S##T mat);                                \
    type mat##S##T##_max(Mat##S##T mat);                                \
    int mat##S##T##_is_zero(Mat##S##T mat);                             \
    int mat##S##T##_is_pos(Mat##S##T mat);                              \
    int mat##S##T##_is_neg(Mat##S##T mat);                              \
    int mat##S##T##_equal(Mat##S##T mat_a, Mat##S##T mat_b);            \
    void mat##S##T##_print(Mat##S##T mat);                              \
    void mat##S##T##_pprint(Mat##S##T mat, const char *name);

#define MATRIX_OPS_DEF(T, S, type, format_spec)                         \
    force_inline Mat##S##T make_mat##S##T()                             \
    {                                                                   \
        Mat##S##T res = {0};                                            \
        return res;                                                     \
    }                                                                   \
                                                                        \
    force_inline Mat##S##T init_mat##S##T(type val)                     \
    {                                                                   \
        Mat##S##T res = make_mat##S##T();                               \
        mat##S##T##_set_diag(&res, val);                                \
        return res;                                                     \
    }                                                                   \
                                                                        \
    force_inline Mat##S##T make_mat##S##T##_fill(type val)              \
    {                                                                   \
        Mat##S##T res = make_mat##S##T();                               \
        mat##S##T##_fill(&res, val);                                    \
        return res;                                                     \
    }                                                                   \
                                                                        \
    force_inline Mat##S##T copy_mat##S##T(Mat##S##T mat)                \
    {                                                                   \
        Mat##S##T copy = make_mat##S##T();                              \
        memcpy(copy.data, mat.data, sizeof(type)*S*S);                  \
        return copy;                                                    \
    }                                                                   \
                                                                        \
    force_inline type mat##S##T##_get(const Mat##S##T mat, int i, int j) \
    {                                                                   \
        return mat.data[j*S+i];                                         \
    }                                                                   \
                                                                        \
    force_inline void mat##S##T##_set(Mat##S##T *mat, int i, int j, type x) \
    {                                                                   \
        mat->data[j*S+i] = x;                                           \
    }                                                                   \
                                                                        \
    force_inline void mat##S##T##_set_row(Mat##S##T *mat, int i, Vec##S##T vec) \
    {                                                                   \
        for (int j = 0; j < S; j++)                                     \
        {                                                               \
            if (j < S)                                                  \
                mat##S##T##_set(mat, i, j, vec.data[j]);                \
            else                                                        \
                mat##S##T##_set(mat, i, j, 0);                          \
        }                                                               \
    }                                                                   \
                                                                        \
    force_inline void mat##S##T##_set_col(Mat##S##T *mat, int j, Vec##S##T vec) \
    {                                                                   \
        for (int i = 0; i < S; i++)                                     \
        {                                                               \
            if (j < S)                                                  \
                mat##S##T##_set(mat, i, j, vec.data[i]);                \
            else                                                        \
                mat##S##T##_set(mat, i, j, 0);                          \
        }                                                               \
    }                                                                   \
                                                                        \
    void mat##S##T##_set_diag(Mat##S##T *mat, type val)                 \
    {                                                                   \
        for (int i = 0, j = 0; i < S && j < S; i++, j++)                \
            mat##S##T##_set(mat, i, j, val);                            \
    }                                                                   \
                                                                        \
    force_inline void mat##S##T##_fill(Mat##S##T *mat, type val)        \
    {                                                                   \
        for (int i = 0; i < S*S; i++)                                   \
            mat->data[i] = val;                                         \
    }                                                                   \
                                                                        \
    force_inline void mat##S##T##_zero(Mat##S##T *mat)                  \
    {                                                                   \
        mat##S##T##_fill(mat, 0);                                       \
    }                                                                   \
                                                                        \
    force_inline void mat##S##T##_identity(Mat##S##T *mat)              \
    {                                                                   \
        mat##S##T##_zero(mat);                                          \
        mat##S##T##_set_diag(mat, 1);                                   \
    }                                                                   \
                                                                        \
    Mat##S##T mat##S##T##_transpose(Mat##S##T mat)                      \
    {                                                                   \
        Mat##S##T res = make_mat##S##T();                               \
        for (int i = 0; i < S; i++)                                     \
        {                                                               \
            for (int j = 0; j < S; j++)                                 \
            {                                                           \
                mat##S##T##_set(&res, i, j, mat##S##T##_get(mat, j, i)); \
                mat##S##T##_set(&res, j, i, mat##S##T##_get(mat, i, j)); \
            }                                                           \
        }                                                               \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat##S##T mat##S##T##_add(Mat##S##T mat_a, Mat##S##T mat_b)         \
    {                                                                   \
        Mat##S##T res = make_mat##S##T();                               \
        for (int i = 0; i < S*S; i++)                                   \
            res.data[i] = mat_a.data[i] + mat_b.data[i];                \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat##S##T mat##S##T##_sub(Mat##S##T mat_a, Mat##S##T mat_b)         \
    {                                                                   \
        Mat##S##T res = make_mat##S##T();                               \
        for (int i = 0; i < S*S; i++)                                   \
            res.data[i] = mat_a.data[i] - mat_b.data[i];                \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat##S##T mat##S##T##_mul(Mat##S##T mat_a, Mat##S##T mat_b)         \
    {                                                                   \
        Mat##S##T res = make_mat##S##T();                               \
        for (int i = 0; i < S; i++)                                     \
            for (int j = 0; j < S; j++)                                 \
                for (int k = 0; k < S; k++)                             \
                    mat##S##T##_set(&res, i, j,                         \
                                    mat##S##T##_get(res, i, j) +        \
                                    mat##S##T##_get(mat_a, i, k)*mat##S##T##_get(mat_b, k, j)); \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat##S##T mat##S##T##_mul_elems(Mat##S##T mat_a, Mat##S##T mat_b)   \
    {                                                                   \
        Mat##S##T res = make_mat##S##T();                               \
        for (int i = 0; i < S*S; i++)                                   \
            res.data[i] = mat_a.data[i] * mat_b.data[i];                \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat##S##T mat##S##T##_div_elems(Mat##S##T mat_a, Mat##S##T mat_b)   \
    {                                                                   \
        Mat##S##T res = make_mat##S##T();                               \
        for (int i = 0; i < S*S; i++)                                   \
            res.data[i] = mat_a.data[i] / mat_b.data[i];                \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat##S##T mat##S##T##_add_constant(Mat##S##T mat, type x)           \
    {                                                                   \
        Mat##S##T res = make_mat##S##T();                               \
        for (int i = 0; i < S*S; i++)                                   \
            res.data[i] = mat.data[i] + x;                              \
        return res;                                                     \
    }                                                                   \
                                                                        \
    type mat##S##T##_min(Mat##S##T mat)                                 \
    {                                                                   \
        type ret = mat.data[0];                                         \
        for (int i = 1; i < S*S; i++)                                   \
            ret = ret < mat.data[i] ? ret : mat.data[i];                \
        return ret;                                                     \
    }                                                                   \
                                                                        \
    type mat##S##T##_max(Mat##S##T mat)                                 \
    {                                                                   \
        type ret = mat.data[0];                                         \
        for (int i = 1; i < S*S; i++)                                   \
            ret = ret > mat.data[i] ? ret : mat.data[i];                \
        return ret;                                                     \
    }                                                                   \
                                                                        \
    int mat##S##T##_is_zero(Mat##S##T mat)                              \
    {                                                                   \
        for (int i = 0; i < S*S; i++)                                   \
            if (mat.data[i] != 0) return 0;                             \
        return 1;                                                       \
    }                                                                   \
                                                                        \
    int mat##S##T##_is_pos(Mat##S##T mat)                               \
    {                                                                   \
        for (int i = 0; i < S*S; i++)                                   \
            if (mat.data[i] < 0) return 0;                              \
        return 1;                                                       \
    }                                                                   \
                                                                        \
    int mat##S##T##_is_neg(Mat##S##T mat)                               \
    {                                                                   \
        for (int i = 0; i < S*S; i++)                                   \
            if (mat.data[i] >= 0) return 0;                             \
        return 1;                                                       \
    }                                                                   \
                                                                        \
    int mat##S##T##_equal(Mat##S##T mat_a, Mat##S##T mat_b)             \
    {                                                                   \
        for (int i = 0; i < S*S; i++)                                   \
            if (mat_a.data[i] != mat_b.data[i]) return 0;               \
        return 1;                                                       \
    }                                                                   \
                                                                        \
    void mat##S##T##_print(Mat##S##T mat)                               \
    {                                                                   \
        for (int i = 0; i < S; i++)                                     \
        {                                                               \
            for (int j = 0; j < S; j++)                                 \
                printf("%"format_spec"\t", mat##S##T##_get(mat, i, j)); \
            printf("\n");                                               \
        }                                                               \
    }                                                                   \
                                                                        \
    void mat##S##T##_pprint(Mat##S##T mat, const char *name)            \
    {                                                                   \
        printf("%s:\n  [\n", name);                                     \
        for (int i = 0; i < S; i++)                                     \
        {                                                               \
            for (int j = 0; j < S; j++)                                 \
                printf("    %"format_spec"\t", mat##S##T##_get(mat, i, j)); \
            printf("\n");                                               \
        }                                                               \
        printf("  ]\n");                                                \
    }

#define MATRIX4_SPEC_OPS_DEC(T, type)                                   \
    Mat4##T mat4##T##_translate(Mat4##T mat, Vec3##T dir);              \
    Mat4##T mat4##T##_rotate(Mat4##T mat, double angle, Vec3##T axis);  \
    Mat4##T mat4##T##_scale(Vec3##T dims);                              \
    Mat4##T mat4##T##_rotate_to(Vec3##T eye, Vec3##T focus, Vec3##T up); \
    Mat4##T mat4##T##_look_at(Vec3##T eye, Vec3##T focus, Vec3##T up);  \
    Mat4##T mat4##T##_perspective(double vert_fov, double aspect, double z_near, double z_far); \
    Mat4##T mat4##T##_ortho(double left, double right, double bottom, double top, double z_near, double z_far);

#define MATRIX4_SPEC_OPS_DEF(T, type)                                   \
    force_inline Mat4##T mat4##T##_translate(Mat4##T mat, Vec3##T dir)  \
    {                                                                   \
        Mat4##T res = copy_mat4##T(mat);                                \
                                                                        \
        mat4##T##_set(&res, 0, 3, dir.x);                               \
        mat4##T##_set(&res, 1, 3, dir.y);                               \
        mat4##T##_set(&res, 2, 3, dir.z);                               \
                                                                        \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat4##T mat4##T##_rotate(Mat4##T mat, double angle, Vec3##T axis)   \
    {                                                                   \
        Mat4##T mod = make_mat4##T();                                   \
                                                                        \
        double cos_a = cos(angle);                                      \
        double sin_a = sin(angle);                                      \
                                                                        \
        Vec3##T normal = vec3##T##_normalize(axis);                     \
        Vec3##T temp = vec3##T##_scale(normal, 1.0-cos_a);              \
                                                                        \
        mat4##T##_set(&mod, 0, 0, temp.x*axis.x+cos_a);                 \
        mat4##T##_set(&mod, 0, 1, temp.x*axis.y-axis.z*sin_a);          \
        mat4##T##_set(&mod, 0, 2, temp.x*axis.z+axis.y*sin_a);          \
                                                                        \
        mat4##T##_set(&mod, 1, 0, temp.y*axis.x+axis.z*sin_a);          \
        mat4##T##_set(&mod, 1, 1, temp.y*axis.y+cos_a);                 \
        mat4##T##_set(&mod, 1, 2, temp.y*axis.z-axis.x*sin_a);          \
                                                                        \
        mat4##T##_set(&mod, 2, 0, temp.z*axis.x-axis.y*sin_a);          \
        mat4##T##_set(&mod, 2, 1, temp.z*axis.y+axis.x*sin_a);          \
        mat4##T##_set(&mod, 2, 2, temp.z*axis.z+cos_a);                 \
                                                                        \
        mat4##T##_set(&mod, 3, 3, 1);                                   \
                                                                        \
        Mat4##T rot = mat4##T##_mul(mod, mat);                          \
                                                                        \
        return rot;                                                     \
    }                                                                   \
                                                                        \
    Mat4##T mat4##T##_scale(Vec3##T dims)                               \
    {                                                                   \
        Mat4##T res = make_mat4##T();                                   \
        mat4##T##_set(&res, 0, 0, dims.x);                              \
        mat4##T##_set(&res, 1, 1, dims.y);                              \
        mat4##T##_set(&res, 2, 2, dims.z);                              \
        mat4##T##_set(&res, 3, 3, 1);                                   \
                                                                        \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat4##T mat4##T##_rotate_to(Vec3##T eye, Vec3##T focus, Vec3##T up) \
    {                                                                   \
        Vec3##T f = vec3##T##_normalize(vec3##T##_sub(focus, eye));     \
        Vec3##T s = vec3##T##_normalize(vec3##T##_cross(f, vec3##T##_normalize(up))); \
        Vec3##T u = vec3##T##_cross(s, f);                              \
                                                                        \
        Mat4##T res = init_mat4##T(1);                                  \
                                                                        \
        mat4##T##_set(&res, 0, 0, s.x);                                 \
        mat4##T##_set(&res, 0, 1, s.y);                                 \
        mat4##T##_set(&res, 0, 2, s.z);                                 \
                                                                        \
        mat4##T##_set(&res, 1, 0, u.x);                                 \
        mat4##T##_set(&res, 1, 1, u.y);                                 \
        mat4##T##_set(&res, 1, 2, u.z);                                 \
                                                                        \
        mat4##T##_set(&res, 2, 0, -f.x);                                \
        mat4##T##_set(&res, 2, 1, -f.y);                                \
        mat4##T##_set(&res, 2, 2, -f.z);                                \
                                                                        \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat4##T mat4##T##_look_at(Vec3##T eye, Vec3##T focus, Vec3##T up)   \
    {                                                                   \
        Vec3##T f = vec3##T##_normalize(vec3##T##_sub(focus, eye));     \
        Vec3##T s = vec3##T##_normalize(vec3##T##_cross(f, vec3##T##_normalize(up))); \
        Vec3##T u = vec3##T##_cross(s, f);                              \
                                                                        \
        Mat4##T res = mat4##T##_rotate_to(eye, focus, up);              \
                                                                        \
        mat4##T##_set(&res, 0, 3, -vec3##T##_dot(s, eye));              \
        mat4##T##_set(&res, 1, 3, -vec3##T##_dot(u, eye));              \
        mat4##T##_set(&res, 2, 3,  vec3##T##_dot(f, eye));              \
                                                                        \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat4##T mat4##T##_perspective(double vert_fov, double aspect, double z_near, double z_far) \
    {                                                                   \
        double f = 1/tan(vert_fov/2);                                   \
                                                                        \
        Mat4##T res = make_mat4##T();                                   \
                                                                        \
        mat4##T##_set(&res, 0, 0, f/aspect);                            \
        mat4##T##_set(&res, 1, 1, f);                                   \
        mat4##T##_set(&res, 2, 2, (z_far+z_near)/(z_near-z_far));       \
        mat4##T##_set(&res, 2, 3, (2*z_far*z_near)/(z_near-z_far));     \
        mat4##T##_set(&res, 3, 2, -1);                                  \
                                                                        \
        return res;                                                     \
    }                                                                   \
                                                                        \
    Mat4##T mat4##T##_ortho(double left, double right, double bottom, double top, double z_near, double z_far) \
    {                                                                   \
        Vec3##T t = init_vec3##T(-(right+left)/(right-left),            \
                                 -(top+bottom)/(top-bottom),            \
                                 -(z_far+z_near)/(z_far-z_near));       \
                                                                        \
        Mat4##T res = make_mat4##T();                                   \
                                                                        \
        mat4##T##_set(&res, 0, 0,  2/(right-left));                     \
        mat4##T##_set(&res, 1, 1,  2/(top-bottom));                     \
        mat4##T##_set(&res, 2, 2, -2/(z_far-z_near));                   \
        mat4##T##_set(&res, 3, 3,  1);                                  \
                                                                        \
        mat4##T##_set(&res, 0, 3, t.x);                                 \
        mat4##T##_set(&res, 1, 3, t.y);                                 \
        mat4##T##_set(&res, 2, 3, t.z);                                 \
                                                                        \
        return res;                                                     \
    }

#define MATRIX4_SPEC_OPS(T, type)               \
    MATRIX4_SPEC_OPS_DEC(T, type);              \
    MATRIX4_SPEC_OPS_DEF(T, type);

#define MATRIX_OPS(T, S, type, format_spec)     \
    MATRIX_OPS_DEC(T, S, type);                 \
    MATRIX_OPS_DEF(T, S, type, format_spec);

VECTOR_OPS_TYPE(f, 2, float);
VECTOR_OPS_TYPE(f, 3, float);
VECTOR_OPS_TYPE(f, 4, float);

MATRIX_OPS_TYPE(f, 2, float);
MATRIX_OPS_TYPE(f, 3, float);
MATRIX_OPS_TYPE(f, 4, float);

VECTOR_OPS_DEC(f, 2, float);
VECTOR_OPS_DEC(f, 3, float);
VECTOR_OPS_DEC(f, 4, float);

MATRIX_OPS_DEC(f, 3, float); 
MATRIX_OPS_DEC(f, 4, float);
MATRIX4_SPEC_OPS_DEC(f, float);

#endif // _GL_MATH_H
