/*
 * Copyright (c) 2008-2009, Jeremy Sandell <jlsandell@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */



/*
 * Whew, now that that's out of the way... Don't be fooled 
 * by the name; this 'server' is designed to simply read from 
 * stdin, and write to both stdout (default), and an optional 
 * filename given on the command line.
 *
 * Intended to be plugged into xinetd.
 **/


/* 
 * Defining _GNU_SOURCE gives us a better basename() 
 * ala string.h than libgen.h
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#define VERSION 0.2.1
#define MAXREAD 65535

static char * program;

int
main(int argc, char **argv)
{
    FILE * out;
    char buf[MAXREAD-1];
    int len = 0;
    int output = -1;

    program = basename(argv[0]);

    if(argc > 1)
    {
        if((out = fopen(argv[1], "a")) == NULL)
        {
            fprintf(stderr, "%s: couldn't open log\n", program);
            exit(EXIT_FAILURE);
        }
        output = fileno(out);
    }


    len = read(fileno(stdin), buf, MAXREAD);
    write(fileno(stdout), buf, len);

    if(output>0)
        write(output, buf, len);

    return EXIT_SUCCESS;
}
