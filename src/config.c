
#include "config.h"

b32 parse_int(char **str, i64 *ret)
{
    if (!*str) return false;
    
    *ret = 0;

    char *curr = *str;
    while ('0' <= *curr && *curr <= '9')
    {
        *ret *= 10;
        *ret += *(curr++) - '0';
    }

    if (curr == *str)
        return false;
    *str = curr;
    return true;
}

b32 parse_float(char **str, f64 *ret)
{
    if (!*str) return false;

    char *curr = *str;
    
    i64 integer = 0;
    b32 ok = parse_int(&curr, &integer);
    printf("INT PARSED FOR FLOAT: %ld\n", integer);

    if (!ok) return false;

    *ret = (f64)integer;
    
    if (curr[0] == '.')
    {
        
        f64 frac = 0;
        curr++;
        f64 div = 10.0;
        while ('0' <= *curr && *curr <= '9')
        {
            frac += (*(curr++) - '0')/div;
            div *= 10;
        }
        *ret += frac;
    }

    *str = curr;
    return true;
}

b32 parse_f32(char **str, f32 *ret)
{
    f64 f;
    b32 ok = parse_float(str, &f);
    *ret = (f32)f;
    return ok;
}

b32 parse_bool(char **str, b32 *ret)
{
    return parse_custom_bool(str, ret, "true", "false");
}

b32 parse_custom_bool(char **str, b32 *ret, char *true_str, char *false_str)
{
    if (!*str) return false;

    int true_len = strlen(true_str);
    int false_len = strlen(false_str);
    if (strncmp(*str, true_str, true_len) == 0)
    {
        *str += true_len;
        *ret = true;
    }
    else if (strncmp(*str, false_str, false_len) == 0)
    {
        *str += false_len;
        *ret = false;
    }
    else
    {
        return false;
    }
    return true;
}

b32 parse_file_path(char **str, char **ret)
{
    if (!*str) return false;

    char *curr = *str;
    while (*curr != ' ' && *curr != '\n' && *curr != '\t' &&  *curr != '\r')
    {
        if (*curr == '\\')
            curr++;
        curr++;
    }
    
    if (curr == *str)
        return false;
    
    *(curr++) = 0;
    *ret = *str;
    *str = curr;
    return true;
}

b32 parse_ident(char **str, char **ret)
{
    if (!*str) return false;

    char *curr = *str;
    b32 first = true;
    while (('a' <= *curr && *curr <= 'z') ||
           ('A' <= *curr && *curr <= 'Z') ||
           ('0' <= *curr && *curr <= '9' && !first) ||
           (*curr == '_'))
    {
        curr++;
        first = false;
    }

    if (*curr != ' ' && *curr != '\n')
        return false;
    *(curr++) = 0;
    *ret = *str;
    *str = curr;
    return true;
}

void skip_space(char **str, b32 newline)
{
    while (**str == '\t' || **str == ' ' ||
           ((**str == '\n' || **str == '\r') && newline))
    {
        (*str)++;
    }
}
