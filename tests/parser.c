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
#include <string.h>
#include <errno.h>

#include <hl7c/segment.h>
#include <hl7c/message.h>
#include "tests.h"

bool parser_test(int argc, char **argv)
{
    int err;
    const char *filename = "../data/adt_a04_13885_20090811203018";
    FILE *fp = NULL;
    void *field = NULL;

    segment *s;
    segment_iter *sit;

    message *m = NULL;
    message_iter *mit = NULL;

    if((fp=fopen(filename, "r"))==NULL)
    {
        err=errno;
        fprintf(stderr, "Couldn't open %s: %s\n", filename, strerror(err));
        exit(err);
    }

    m = message_ctor(m); /* Construct the message object. The segments 
                          * are constructed by the parser.
                          */

    m = m->parse(m, fp, "\r", "|"); /* Parse the message. */

    fclose(fp);

    for(mit = message_iter_ctor(m), /* Construct the message iterator */
        s = mit->begin(m);          /* Initialize our segment object to the first segment. */
        s != mit->end(mit);         /* Stop if we reach the end of the message. */
        s = mit->next(mit))         /* Otherwise, go on to the next segment. */
    {

        for(sit = segment_iter_ctor(s), /* Construct the segment iterator. */
            field = sit->begin(s);      /* Initialize our field to the first field */
            field != sit->end(sit);     /* Stop if we reach the end of the segment. */
            field = sit->next(sit))     /* Otherwise, go on to the next field. */
        {
            fprintf(stderr, "'%s'\n", (char*)field); /* Process the field. In this case, just print it out. */
        }

        sit->dtor(sit); /* Clean up the segment iterator. */
    }

    mit->dtor(mit); /* Clean up the message iterator. */
    m->dtor(m); /* Clean up the message object. */

    return true;
}
