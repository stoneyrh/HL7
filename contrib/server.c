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


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <libgen.h>
#include "common.h"
#include "proto.h"
#include "net.h"

extern char * program_invocation_short_name; /* basename(argv[0]), but global */
#define program program_invocation_short_name


void
usage(FILE *stream, int code)
{
    fprintf(stream, "usage: %s: hostname port filename\n", program);
    fprintf(stream, "\noptional arguments\n"
                    "        -o [file]   output filename\n");

    exit(code);
}

int
main(int argc, char **argv)
{
    FILE *out = NULL;
    char *buf = NULL;
    char *logname = NULL;
    char *goodack = NULL;
    bool dolog = false;
    bool doack = false;

    int len = 0;
    int next_opt;
    off_t offset = 0;
    int read_fd,
        fsize;

    const char *short_opt = "o:a:";
    const struct option long_opt[] = {
        {"ackfile", 1, NULL, 'a'},
        {"output", 1, NULL, 'o'},
        {NULL,     0, NULL, 0},
    };

    do 
    {
        next_opt = getopt_long(argc, argv, short_opt, long_opt, NULL);

        switch(next_opt)
        {
            case 'o':       /* -o or --output ... optional */
                logname = optarg;
                dolog = true;
                break;

            case 'a':       /* -o or --output ... optional */
                goodack = optarg;
                doack = true;
                break;

            case '?':       /* invalid option */
                usage(stderr, EXIT_FAILURE);
                break;

            case -1: /* End of options */
                break;

            default: /* If we get here, something went very wrong. */
                abort();
                break;

        }
    } while(next_opt != -1);

    if(dolog)
    {
        out = fopen(logname, "a");

        if(out==NULL)
            die(stderr, EXIT_FAILURE,
                    "%s: couldn't open log\n", program);
    }

    buf = tcp_recv(fileno(stdin), 5000, 4096, &len);

    if(dolog)
        fprintf(out, " got %d bytes.\nMessage:'%s'\n", len, buf);


    if(doack)
    {
        fsize=getsize(goodack);
        if(dolog)
            fprintf(out, "%s is %d bytes\n", goodack, fsize);

        if(fsize>0)
        {
            if((read_fd = open(goodack, O_RDONLY))>0)
            {
                sendfile(fileno(stdout), read_fd, &offset, fsize);
                close(read_fd);
            }

        }
    }

    free(buf);

    if(dolog)
        fclose(out);

    return EXIT_SUCCESS;
}
