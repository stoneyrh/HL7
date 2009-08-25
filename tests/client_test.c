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

#define _GNU_SOURCE
#include "scan.h"
#include "proto.h"
#include "net.h"
#include "parse.h"

#define MAX_READ 65535
extern char * program_invocation_short_name;
#define program program_invocation_short_name

void
usage(FILE *stream, int code)
{
    fprintf(stream, "usage: %s: hostname port filename\n", program);
    fprintf(stream, "\noptional arguments\n"
                    "        -o [file]   output filename\n"
                    "        -n          don't wait for ACK from server\n");

    exit(code);
}


bool
client_test(int argc, char **argv)
{
    FILE *out = NULL;
    FILE *err = NULL;

    char *ack = NULL;

    const char *filename = NULL;
    const char *host = NULL;
    const char *port = NULL;
    off_t offset = 0;

    bool check = false;

    int sockfd,
        tmperr,
        write_count,
        fdin,
        size,
        wait_for_ack,
        req,
        next_opt;

    const char *short_opt = "no:";
    const struct option long_opt[] = {
        {"no-ack", 0, NULL, 'n'},
        {"output", 1, NULL, 'o'},
        {NULL,     0, NULL, 0},
    };

    /* Just in case we want to redirect stderr later on. */
    err = stderr;
    out = stdout;
    wait_for_ack = 1;

    do 
    {
        next_opt = getopt_long(argc, argv, short_opt, long_opt, NULL);
        switch(next_opt)
        {

            case 'o':       /* -o or --output ... optional */
                out = redirect(optarg, "a", stdout);
                break;

            case 'n':       /* -n or --no-ack ... optional */
                wait_for_ack = 0;
                break;

            case '?':       /* invalid option */
                usage(err, EXIT_FAILURE);
                break;

            /* End of options */
            case -1:
                break;

            /* If we get here, something went very wrong. */
            default:    
                abort();
                break;

        }
    } while(next_opt != -1);


    argv += optind;
    argc -= optind;

    if(argc != 3)
        usage(err, EXIT_FAILURE);

    host = argv[0];
    port = argv[1];
    filename = argv[2];
    
    if(filename == NULL)
        die(err, EINVAL,
                 "%s: HL7 filename required.\n", program);

    if(host == NULL)
        die(err, EDESTADDRREQ,
                 "%s: Destination host required.\n", program);

    if(port == NULL)
        die(err, EINVAL,
                 "%s: Destination port required.\n", program);


    fdin = sockfd = -1;
    write_count = 0;

    size = getsize(filename);

    /* Save errno, as writing to a stream can change it in the 
     * interim.
     */

    tmperr = errno;

    if(size <= 0)
    {
        switch(size)
        {
            case 0:
                die(err, EXIT_FAILURE,
                    "%s: %s: Cowardly refusing to send an empty file!\n",
                    program, filename);
                break;

            case -1:
                die(err, tmperr,
                    "%s: Could not open \"%s\": %s\n",
                    program, filename, strerror(errno));
                break;

            default:
                die(err, EXIT_FAILURE,
                    "%s: Could not open \"%s\"\n",
                    program, filename);
                break;
        }
    }

    if(!tcp_connect(host, atoi(port), &sockfd))
    {
        tmperr = errno;
        switch(sockfd)
        {
            case -1:
                die(err, EXIT_FAILURE,
                        "Error opening socket.\n");
                break;

            case -2:
                die(err, EXIT_FAILURE,
                        "%s: Hostname lookup failure: %s\n",
                        program, host);
                break;

            case -3:
                die(err, tmperr,
                        "%s: error connecting to %s, port %s.\n",
                        program, host, port);
                break;

            default:    /* If we get here, the API is busted. */
                die(err, EXIT_FAILURE,
                        "%s: Unknown error!\n", program);
                break;

        }
    }

    /* Read file and send to server */

    if((fdin = open(filename, O_RDONLY)) == -1)
    {
        die(err, errno,
            "%s: couldn't open %s for reading: %s\n",
            program, filename, strerror(errno));
    }
    else
    {
        /* Toss file to server. Linux syscall, but its fast. :D*/
        sendfile(sockfd, fdin, &offset, size);

        if(wait_for_ack)
        {
            ack = tcp_recv(sockfd, 5000, MAX_READ, &req);
            if(ack==NULL)
                return false;

            if((ack!=NULL) && 
                (strlen(ack) > 0))
            {
                fprintf(stderr, "%s\n", ack);
                check = parse(ack);
            }
            free(ack);
            ack = NULL;
            close(sockfd);

            return check;
        } /* if waiting for ack ... */
        close(sockfd);
    } /* if able to open filestream... */
    return true;
}
