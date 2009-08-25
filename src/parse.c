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
#include "parse.h"

/*
 * \fn parse
 * \brief Simple example parser.
 * returns true on success, false on failure.
 * 
 **/

bool
parse(const char *message)
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
    {
        errn=errno;
        die(stderr, errn,
            "%s: Memory error! (see return code)\n", libhl7c);
    }
    printf("%s\n", ack);

    /* Run our copy through fmemopen. Now our memory is a
     * file handle. 
     */

    if((msg_handle = fmemopen(ack, strlen(ack)+1, "r")) == NULL)
    {
        errn = errno;
        die(stderr, errn,
            "%s: Memory error! (see return code)\n", libhl7c);
    }

    /* 
     * Pass our handle to our scanning routine. It allocates
     * the necessary memory, and we have to free it afterward. 
     */

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
             */

            if(strlen(hl7->members[i]->data[0]) > 1)
            {
                seg = strdup(hl7->members[i]->data[0]);

                if(seg == NULL)
                    die(stderr, ENOMEM,
                            "%s: Out of memory!\n", libhl7c);

                field2 = strdup(hl7->members[i]->data[1]);

                if(field2 == NULL)
                    die(stderr, ENOMEM,
                            "%s: Out of memory!\n", libhl7c);

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
                    return false;

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
    return true;
}
