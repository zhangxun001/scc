#ifndef _DEF_H_
#define _DEF_H_

#include <stdio.h>
#define DEBUG_PRINT

#ifdef DEBUG_PRINT
    #define dev_err(...) do{fprintf(stderr, "\033[31merr[%s, %d]: \033[0m", __func__, __LINE__); \
                    fprintf(stderr, __VA_ARGS__);fprintf(stderr, "\n");} while(0)
    #define dev_dbg(...) do{fprintf(stderr, "dbg[%s,%d]: ", __func__, __LINE__); \
                    fprintf(stderr, __VA_ARGS__);fprintf(stderr, "\n");} while(0)
#else
    #define dev_err(...) do{fprintf(stderr, "[%s, %d]: ", __func__, __LINE__); \
                    fprintf(stderr, __VA_ARGS__);fprintf(stderr, "\n");} while(0)
    #define dev_dbg(...) do{}while(0)
#endif

struct scc_state {
    char *src_file_buf;
    char *out_file_buf;
};

typedef struct scc_state scc_state;

#endif
