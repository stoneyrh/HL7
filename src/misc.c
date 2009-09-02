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

bool
has_cntrl(const char *s)
{
    register int pos;
    int len = strlen(s);
    pos = 0;

    while(--pos <= len)
        if( (isspace(s[pos]) && !isblank(s[pos])) || !isprint(s[pos]) || iscntrl(s[pos]) )
            return true;
    return false;
}



char *
convert_cntrl(const char *s)
{
    char *result, *ptr, *copy;

    register size_t pos;
    int len;
    register int count = 0;

    if(s == NULL)
        return NULL;

    /* Make a temporary copy of the string, so that we don't 
     * invalidate our const argument.
     */

    copy = strdup(s);
    len = strlen(copy);

    /* count all unprintable characters. */
    for(pos = 0, ptr = copy; pos < len; ptr++, pos++)
        if( (isspace(*copy) && !isblank(*copy)) ||   !isprint(*ptr) || iscntrl(*ptr) )
            count++;

    /* using this count, allocate enough space to hold octal 
     * representations of the unprintable characters.
     */
    result = malloc(len + count * sizeof(char*) + 1);  

    if(count == 0)
    {
        /* None found, just copy the */
        memcpy(result, copy, len);
        result[len] = '\0';
    }
    else
    {
        ptr = result;

        for(pos = 0; pos < len; copy++, pos++)
            if( (isspace(*copy) && !isblank(*copy)) || !isprint(*copy) || iscntrl(*copy))
            {
                sprintf(ptr, "\\0%02o", *copy);
                ptr += sizeof(char*);
            }
            else
                *ptr++ = *copy;

        *ptr = '\0';
    }

    /* Revert copy pointer back to its original state, so
     * we can free it.
     */

    copy -= count;
    free(copy);

    return result;
}
