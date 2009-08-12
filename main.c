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

#define MAX_READ 65535

int 
main(int argc, char **argv)
{
    FILE *out = NULL;
    FILE *err = NULL;

    char *ack = NULL;
    char *msg = NULL;

    const char *filename = NULL;
    const char *host = NULL;
    const char *port = NULL;

    int sockfd,
        tmperr,
        write_count,
        fdin,
        ret,
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
    program = basename(argv[0]);

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


            /* End of options - can be triggered
             * by the user by using the '--' option.
             **/

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
     **/

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

    sockfd = tcp_connect(host, atoi(port));
    tmperr = errno;

    if (sockfd < 0)
    {
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
        /* 
         * Allocate memory for the size of the message, plus one
         * for the terminating NUL character.
         **/

        msg = (char*)malloc(size*sizeof(char)+1);

        if(msg == NULL)
        {
            close(fdin);
            close(sockfd);
            die(err, ENOMEM, "%s: Out of memory!\n", program);
        }


        /* NOTE: If these two read/write calls turn out to be
         * a bottleneck, on Linux one can use sendfile(2), which 
         * is more efficient as it doesn't have to transfer the
         * data in and out of userspace - that is, it handles it
         * all in the kernel. 
         **/
       

        /* Pull file into msg */

        if((ret = read(fdin, msg, size)) != size) /* read error */
        {
            free(msg);
            close(fdin);
            close(sockfd);
            die(err, EIO,
                "%s: Partial or failed read of file %s "
                "(%d bytes total, %d read)\n",
                program, filename, size, ret);
        }

        close(fdin);

        /* Write msg to socket */
        if((write_count = write(sockfd, msg, size)) != size)
        {
            /* 
             * If write_count != size, that's a good 
             * indication that Bad Things have happened to our
             * process, so let the user know about it.
             **/

            free(msg);
            close(sockfd);
            die(err, EIO,
                "%s: Partial or failed write of file %s to sockfd %d,"
                " %d bytes read, %d bytes written\n",
                program, filename, sockfd, size, write_count);
        }

        free(msg);
        msg = NULL;

        if(wait_for_ack)
        {
            ack = tcp_recv(sockfd, 10000, MAX_READ, &req);
            ret = 0;

            if(strlen(ack) > 0)
                ret = handle_ack(ack);

            free(ack);
            ack = NULL;

            if(ret != 1)
            {
                /* The received ack reported an error */
                close(sockfd);
                return ret;
            }
            else
                printf("Success!\n");

        } /* if waiting for ack ... */
        close(sockfd);
        
    } /* if able to open filestream... */
    return EXIT_SUCCESS;
}

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

void
usage(FILE *stream, int code)
{
    fprintf(stream, "usage: %s: hostname port filename\n", program);
    fprintf(stream, "\noptional arguments\n"
                    "        -o [file]   output filename\n"
                    "        -n          don't wait for ACK from server\n");

    exit(code);
}

FILE *
redirect(const char *path, const char *mode, FILE *stream)
{
    FILE *fp;

    if((fp = freopen(path, "a", stream)) == NULL)
        die(stderr, errno,
            "%s: Error redirecting fileno %d  to %s: %s\n",
            program, fileno(stream), path, strerror(errno));
    
    return fp;
}

/*
 * @function handle_ack
 * returns 1 on success, something else on failure.
 **/

int
handle_ack(const char *message)
{
    int i,
        eflag,
        errn,
        seg_len;

    FILE *msg_handle;    /* "file" handle. */
    char *ack = NULL;    /* Copy of message received. */
    char *seg = NULL;    /* Segment header. */
    char *field2 = NULL; /* Second field in segment */

    Multi *hl7 = NULL;

    eflag = 0;
    errn  = 0;

    /* 
     * Make a copy of message, as fmemopen alters the buffer
     * passed to it.
     **/

    ack = strdup(message);

    if(ack == NULL)
        die(stderr, ENOMEM,
            "%s: Out of memory!\n", program);

    /* Run our copy through fmemopen. Now our memory is a
     * file handle. 
     **/

    if((msg_handle = fmemopen(ack, strlen(ack)+1, "r")) == NULL)
    {
        errn = errno;
        die(stderr, errn,
            "%s: Memory error! (see return code)\n", program);
    }

    /* 
     * Pass our handle to our scanning routine. It allocates
     * the necessary memory, and we have to free it afterward. 
     **/

    hl7 = multi_scan(msg_handle, "\r", "|");
    fclose(msg_handle);

    /* Scan through segments */
    for(i = 0; i != hl7->size; i++)
    {
        seg_len = hl7->members[i]->size;

        if(seg_len >= 2)
        {
            /* Skip if it's not a real segment,
             * i.e., "XXX|..."
             **/

            if(strlen(hl7->members[i]->data[0]) > 1)
            {
                seg = strdup(hl7->members[i]->data[0]);

                if(seg == NULL)
                    die(stderr, ENOMEM,
                            "%s: Out of memory!\n", program);

                field2 = strdup(hl7->members[i]->data[1]);

                if(field2 == NULL)
                    die(stderr, ENOMEM,
                            "%s: Out of memory!\n", program);

                //d("[%s|%d] %d (f2 == %s)\n", seg, i, seg_len, field2); 
                
                /* Check our ack field. */
                if(strcmp(seg, "MSA") == 0)
                {
                    if(strcmp(field2, "AE") == 0)
                    {
                        /* They've reported an error. */
                        errn = EPROTO;
                        eflag = 1;
                    }
                    else
                    {
                        printf("Response: %s\n", ack); 
                    }
                } /* MSA Segment */


                if((eflag == 1) && 
                   (strcmp(seg, "ERR") == 0))
                {
                    fprintf(stderr, "ERROR: %s\n", field2);

                } /* ERR Segment */


                /* To iterate further through the segments, do 
                 * something like this:
                 *
                 * for(x = 0; x != hl7->members[i]->size; x++)
                 * {
                 *      ... process hl7->members[i]->data[x] ...
                 * }
                 **/

                /* Clean up after strdup */
                free(seg);
                free(field2);

            } /* if strlen ... */

        } /* if seg_len ... */

    } /* for ... */

    /* Release our memory. free_multi iterates
     * through each segment.
     **/
    free_multi(hl7);
    free(ack);
    return (errn == 0) ? 1 : errn;
}
