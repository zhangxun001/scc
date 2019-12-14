#ifndef _MAIN_H_
#define _MAIN_H_

#include "stdlib.h"
#include "def.h"

int read(const char *file, char *buf, size_t buf_size);
scc_state *scc_new(void);
int scc_delete(scc_state *s);

#endif
