#ifndef PRIV_H
#define PRIV_H

#include <stddef.h>
#include "fdt.h"

void set_priv_m(size_t level, void* entry_func, struct fdt_header* dtb);

#endif
