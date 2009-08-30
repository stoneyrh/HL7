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

#include <hl7c/message.h>

message *
message_ctor(message *self)
{
    /* Allocate the initial message structure */
    self = calloc(1, sizeof(message));
    self->len = 0;
    self->segments = NULL; /* actual storage - indexed by zero. */

    /* set up member functions */
    self->dtor = message_dtor;
    self->push = message_push;
    self->begin = message_begin;
    self->end = message_end;

    self->first = message_first;
    self->last = message_last;
    return self;
}


void * 
message_begin(message *self)
{
    return *(self->segments + 0);
}

void *
message_end(message *self)
{
    return *(self->segments + self->len);
}

int
message_first(message *self)
{
    return 0;
}

int
message_last(message *m)
{
    return (m->len - 1);
}

message *
message_push(message *m, segment *seg)
{
    /* Don't bother if message hasn't been initialized. */

    if(m != NULL && seg != NULL)
    {
        m->segments = realloc(m->segments, sizeof(char*) * ++m->len);

        if(m->segments == NULL)
        {
            fprintf(stderr, "%s: %d: Out of memory!\n",
                    __func__, __LINE__);
            exit(ENOMEM);
        }
        m->segments[m->len] = seg;
    }
    return m;
}


void
message_dtor(message *m)
{
    int i;
    if(m != NULL)
    {
        for(i=0; i != m->len; i++)
            segment_dtor(m->segments[i]);

        free(m->segments);
        free(m);
    }
    return;
}
