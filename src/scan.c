/*
 *  Copyright (c) 2008-2009, Jeremy Sandell <jlsandell@gmail.com>
 *  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "scan.h"

/**
 * \file scan.c
 *
 * \brief Functions for man
 */

Array *
array_scan(FILE *fp,  char *sep, char *delim)
{
    FILE *fake_line;   /* fake file pointer */

    ssize_t llen;       /* length of line read */
    ssize_t flen;       /* length of field read */

    size_t l = 0;       /* Needed for first getdelim */
    size_t f = 0;       /* Needed for second getdelim */

    int err = 0;        /* storage for errno */

    char *line = NULL;  
    char *field = NULL;
    char end[3];

    Array * array;
    array = array_init();

    /* Break data into lines */
    while((getdelim(&line, &l, sep[0], fp)) != -1)
    {
        /* get the line length, not counting the nul */
        llen = strlen(line);

        (void)snprintf(end, 2, "%c", line[llen-1]);
        
        if(strcmp(end, sep) == 0)
            line[llen-1] = 0;

        /* Create fake file pointer for our second scan. */
        if((fake_line = fmemopen(line, strlen(line)+1, "r")) == NULL)
        {
            err = errno;
            (void)fprintf(stderr, "Internal error: aborting\n");
            exit(err);
        }

        /* Now break each line into fields */
        while((getdelim(&field, &f, delim[0], fake_line)) != -1)
        {
            /* Get the field length, not counting the nul */
            flen = strlen(field);

            /*
             * Make a copy of the field, push it into our Array.
             * Must use free_array to clean up afterward.
             **/

            array = push(array, field);

            (void)snprintf(end, 2, "%c", array->data[array->len][flen-1]);

            if(strcmp(end, delim) == 0)
                array->data[array->len][flen-1] = 0;

        }
        (void)fclose(fake_line);
    }

    free(line);
    free(field);

    return array;
}

/**
 * Multi dimensional version of array_scan.
 */

Multi *
multi_scan(FILE *fp,  char *sep, char *delim)
{
    FILE *fake_line;   /* fake file pointer */

    //ssize_t llen;       /* length of line read */
    //ssize_t flen;       /* length of field read */
    int llen = 0;
    int flen = 0;

    size_t l = 0;       /* Needed for first getdelim */
    size_t f = 0;       /* Needed for second getdelim */

    int err = 0;        /* storage for errno */

    char *line = NULL;
    char *field = NULL;
    char end[3];

    Multi *multi;
    Array *array;

    multi = multi_init();

    /* Break data into lines */
    while((llen=getdelim(&line, &l, sep[0], fp)) != -1)
    {
        (void)snprintf(end, 2, "%c", line[llen-1]);

        if(strcmp(end, sep) == 0)
            line[llen-1] = 0;

        /* Create fake file pointer for our second scan. */
        if((fake_line = fmemopen(line, strlen(line)+1, "r")) == NULL)
        {
            err = errno;
            (void)fprintf(stderr, "Internal error: aborting\n");
            exit(err);
        }

        array = array_init();

        /* Now break each line into fields */
        while((flen=getdelim(&field, &f, delim[0], fake_line)) != -1)
        {
            /*
             * Make a copy of the field, push it into our Array.
             * Must use free_array to clean up afterward.
             **/

            array = push(array, field);
            (void)snprintf(end, 2, "%c", array->data[array->len][flen-1]);

            if(strcmp(end, delim) == 0)
                array->data[array->len][flen-1] = 0;

        }
        (void)fclose(fake_line);
        multi = mpush(multi, array);
    }

    free(line);
    free(field);

    return multi;
}

void
d(const char *fmt, ...)
{
#ifdef DEBUG
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
#else
    return;
#endif
}

