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
mklines(int num, FILE *out)
{
    int i;
    for(i = 0; i <= num; i++)
        fprintf(out, "-");
    return;
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

char *
trim_right(char *s, int c)
{
    if(s == NULL)
        return NULL;

    register int i = strlen(s);

    while(i > 0 && s[i - 1] == c)
        s[--i] = 0;

    return s;
}

char *
trim_left(char *s, char *c)
{
    if(s == NULL)
        return NULL;

    return memmove(s, &s[strspn(s, c)], strlen(s));
}
