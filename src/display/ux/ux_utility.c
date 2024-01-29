#include "ux.h"
#include "ux_utility.h"
#include "console.h"
int32_t ux_utility_adjust_var_wrapping(const ux_utility_adjust_var_wrap_t * const p_config, int32_t current_val, const int32_t change_value)
{
  if(!p_config)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return 0;
  }
  current_val += change_value;
  if(current_val > p_config->upper_bound)
  {
    if(p_config->up_wrap) current_val = p_config->lower_bound;
    else current_val = p_config->upper_bound;
  }
  else if(current_val < p_config->lower_bound)
  {
    if(p_config->low_wrap) current_val = p_config->upper_bound;
    else current_val = p_config->lower_bound;
  }
  return current_val;
}

