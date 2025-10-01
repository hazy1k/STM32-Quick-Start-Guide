#ifndef T9PY_H
#define T9PY_H

#include <stdint.h>

uint8_t get_matched_pymb(uint8_t *str, const char **py, const char **aa);
uint8_t get_matched_py_count(uint8_t *strin);
#endif

