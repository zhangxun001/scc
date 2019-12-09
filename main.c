#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "def.h"

#define ERROR (-1)
#define OK 0

#define DEBUG_PRINT

#ifdef DEBUG_PRINT
    #define dev_err(...) do{fprintf(stderr, "err[%s, %d]: ", __func__, __LINE__); \
                    fprintf(stderr, __VA_ARGS__);fprintf(stderr, "\n");} while(0)
    #define dev_dbg(...) do{fprintf(stderr, "dbg[%s,%d]: ", __func__, __LINE__); \
                    fprintf(stderr, __VA_ARGS__);fprintf(stderr, "\n");} while(0)
#else
    #define dev_err(...) do{fprintf(stderr, "[%s, %d]: ", __func__, __LINE__); \
                    fprintf(stderr, __VA_ARGS__);fprintf(stderr, "\n");} while(0)
    #define dev_dbg(...) do{}while(0)
#endif

#define FILE_BUF_SIZE (2 * 1024 * 1024)

#define FREAD_NMEMB 1

int read(const char *file, char *buf, size_t buf_size)
{
    FILE *fp = NULL;
    fp = fopen(file, "rb");
    if(fp == NULL) {
        dev_err("Could not open source file");
        return ERROR;
    }
    fseek(fp, 0L, SEEK_END);
    size_t file_size = ftell(fp);
    dev_dbg("%ld", file_size);
    if (file_size >= buf_size) {
        dev_err("input source file too large!"); 
        file_size = buf_size-1;  
    }
    rewind(fp);
    size_t ret;
    if((ret = fread(buf, 1, file_size, fp)) != file_size)
        dev_err("file read incomplete %ld", ret); 
    buf[file_size] = '\0';
    (void)fclose(fp);
    return OK;
}

int compile(scc_state *s)
{
    dev_dbg("%s", s->src_file_buf);
    return OK;
}

scc_state *scc_new(void){
    scc_state *s = malloc(sizeof(scc_state));

    if(s == NULL) {
        dev_err("malloc scc NULL");
        return NULL;
    }
    s->src_file_buf = malloc(FILE_BUF_SIZE);
    memset(s->src_file_buf , 0, FILE_BUF_SIZE);

    s->out_file_buf = malloc(FILE_BUF_SIZE);
    memset(s->out_file_buf , 0, FILE_BUF_SIZE);
    return s;
}

int scc_delete(scc_state *s){

    if(s == NULL) {
        dev_err("scc is NULL");
        return ERROR;
    }

    free(s->src_file_buf);
    free(s->out_file_buf);

    free(s);
    return OK;
}


static void show_help()
{
    dev_err("Usage:");
    dev_err("   ./scc [source_file] [output_file]");
    dev_err("   ./scc test.sc test ");
}

static void check(int c, char* v[])
{
    if (c != 3) {
        show_help();
        exit(ERROR);
    }
}

static int read_single_srcfile(scc_state *s, const char* src_file)
{
    char* buf = s->src_file_buf;
    read(src_file, buf, FILE_BUF_SIZE);
}

static scc_state *s1;

int main(int argc, char* argv[])
{
    check(argc, argv);
    char *src_file = argv[1];
    char *out_file = argv[2];
    dev_dbg("%s %s", src_file, out_file);
    s1 = scc_new();
    read_single_srcfile(s1, src_file);
    compile(s1);
    scc_delete(s1);
    return OK;

}
