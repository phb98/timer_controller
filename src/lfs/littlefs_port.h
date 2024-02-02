#ifndef _LITTLE_FS_PORT_H_
#define _LITTLE_FS_PORT_H_
#include "lfs.h"
void lfs_port_init();
lfs_t * lfs_port_get_lfs_handle();
const char * lfs_port_get_err_string(const int err);
#endif