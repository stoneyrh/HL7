#include "hl7c/proto.h"

int
getsize(const char *path)
{
    struct stat st;
    int ret;

    /* Couldn't stat, no such file, permission denied, or
     * similar.
     **/
    if((ret = stat(path, &st)) != 0)
        return -1;

    return st.st_size;
}


void
die(FILE *err, int code, const char *fmt, ...)
{
    va_list ap;
    int fd;

    va_start(ap, fmt);
    vfprintf(err, fmt, ap);
    va_end(ap);

    /* Attempt to clean up, just in case err 
     * is redirected to a file. 
     **/

    if((fd = fileno(err)) != -1)
        fdatasync(fd);

    fclose(err);
    exit(code);
}


FILE *
redirect(const char *path, const char *mode, FILE *stream)
{
    FILE *fp;

    if((fp = freopen(path, "a", stream)) == NULL)
        return NULL;
    
    return fp;
}
