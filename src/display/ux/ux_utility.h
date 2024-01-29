#ifndef _UX_UTILITY_H_
#define _UX_UTILITY_H_
#include <stdint.h>
#include <stdbool.h>
typedef struct {
  int32_t upper_bound;
  int32_t lower_bound; 
  bool    up_wrap;  // When value overflow upper_bound, do you want to it be lower_bound
  bool    low_wrap; // Same as above
} ux_utility_adjust_var_wrap_t;
int32_t ux_utility_adjust_var_wrapping(const ux_utility_adjust_var_wrap_t * const p_config, int32_t current_val, const int32_t change_value);
#endif