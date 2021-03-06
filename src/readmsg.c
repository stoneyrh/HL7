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

#include <stdio.h>
#include <stdbool.h>

#include <hl7c/message.h>
#include <hl7c/segment.h>
#include <hl7c/common.h>
#include <hl7c/proto.h>

/*
 * \fn readmsg
 * \brief Simple example parser.
 * returns true on success, false on failure.
 * 
 **/

bool
readmsg(const char *str)
{
    int errn,
        i;

    FILE *msg_handle  = NULL;    /* "file" handle. */
    char *ack         = NULL;    /* Copy of message received. */
    char *field       = NULL;

    char *copy        = NULL;
    char *segcpy      = NULL;

    message *msg      = NULL;
    segment *seg      = NULL;

    segment_iter *si  = NULL;
    message_iter *mi  = NULL;

    errn  = 0;

    /* 
     * Make a copy of message, as fmemopen alters the buffer
     * passed to it.
     */

    ack = strdup(str);

    if(ack == NULL)
    {
        errn = errno;
        die(stderr, errn,
            "%s: Memory error! (see return code)\n", libhl7c);
    }

    /* Run our copy through fmemopen. Now our memory is a
     * file handle, opened for reading.
     */

    if((msg_handle = fmemopen(ack, strlen(ack)+1, "r")) == NULL)
    {
        errn = errno;
        die(stderr, errn,
            "%s: Memory error! (see return code)\n", libhl7c);
    }

    msg = message_ctor(msg);
    msg = msg->parse(msg, msg_handle, "\r", "|");
    fclose(msg_handle);

    for(mi = message_iter_ctor(msg), /* Construct message iterator, and */
        seg = mi->begin(msg);        /* initialize segment to the first one in the message. */
        seg != mi->end(mi);          /* While seg != one past end, */
        seg = mi->next(mi))          /* go on to next segment. */
    {
        printf("%-6.4s %-10.7s %-10.5s %-10.4s\n", "Seq.", "Segment", "Field", "Data");

        /* Same as above, but for fields in segments. */
        for(i = 0, si = segment_iter_ctor(seg), field = si->begin(seg); field != si->end(si); field = si->next(si), i++)
        {
            if(has_cntrl(field))
            {
                copy = convert_cntrl(field);
                segcpy = convert_cntrl((char*)seg->begin(seg));

                printf("[%-3d]%2s%-10.12s %-10d [%s]\n", i, " ", segcpy, i + 1, copy);

                free(segcpy);
                free(copy);
            }
            else
            {
                printf("[%-3d]%2s%-10.12s %-10d [%s]\n", i, " ", (char *)seg->begin(seg), i + 1, field);
            }
        }

        printf("\n\n");

        si->dtor(si); 
    }
    /* Clean up message iterator.*/
    mi->dtor(mi);

    /* Release message object. Calls destructors for contained 
     * segments, as well.
     */
    msg->dtor(msg);
    free(ack);
    return true;
}
