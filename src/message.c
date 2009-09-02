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
#include <hl7c/message.h>
#include <hl7c/segment.h>

message *
message_ctor(message *self)
{
    /* Allocate the initial message structure */
    self = calloc(1, sizeof(message));
    self->len = 0;
    self->segments = NULL; /* actual storage - indexed by zero. */

    /* set up member functions */
    self->dtor = message_dtor;   /* destructor */
    self->push = message_push;   /* add segment */
    self->begin = message_begin; /* first segment */
    self->end = message_end;     /* last segment */
    self->first = message_first; /* first position */
    self->last = message_last;   /* last position */

    self->parse = message_parse; /* parse hl7 message. */
    return self;
}


segment * 
message_begin(message *self)
{
    return *(self->segments + 0);
}

segment *
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
        m->segments = realloc(m->segments, sizeof(segment) * ++m->len);

        if(m->segments == NULL)
        {
            fprintf(stderr, "%s: %d: Out of memory!\n",
                    __func__, __LINE__);
            exit(ENOMEM);
        }
        m->segments[m->len -1] = seg;
    }
    return m;
}

/**
 * \fn message_dtor
 * \brief 
 *      Destructor for the message object. Iterates over each segment, calling the 
 *      segment's destructor. 
 *
 *  \param m - message we want to remove.
 */

void
message_dtor(message *m)
{
    segment *s = NULL;
    message_iter *mit = NULL;

    if(m != NULL)
    {
        mit = message_iter_ctor(m); 

        for(s = mit->begin(m); s != mit->end(mit); s = mit->next(mit))
            s->dtor(s);

        mit->dtor(mit);

        free(m->segments);
        free(m);
    }
    return;
}

message_iter *
message_iter_ctor(message *m)
{
    message_iter *i = calloc(1, sizeof(message_iter));
    /* setup our member functions... */
    i->klass = m;
    i->begin = m->begin;

    i->end   = message_iter_end;
    i->dtor = message_iter_dtor;
    i->next = message_iter_next;

    i->first = m->first(m);
    i->last = m->last(m);
    i->state = i->first;

    return i;
}

void  *
message_iter_next(message_iter *i)
{
    void *it;

    if(i->state != i->last)
    {
        i->state++;
        it = i->klass->segments[i->state];
    }
    else
    {
        i->state = i->first;
        it = i->klass->end(i->klass);
    }

    return it;
}

segment *
message_iter_begin(message_iter *i)
{
    return i->klass->segments[i->first];
}

segment *
message_iter_end(message_iter *i)
{
    return i->klass->end(i->klass);
}

void
message_iter_dtor(message_iter *i)
{
    free(i);
    return;
}


message *
message_parse(message *msg, FILE *fp, const char *sep, const char *delim)
{
    FILE *fake_line;   /* fake file pointer */

    int llen = 0;       /* length of line    */
    int flen = 0;       /* length of field   */
    int err  = 0;       /* storage for errno */

    size_t l = 0;       /* Needed for first getdelim */
    size_t f = 0;       /* Needed for second getdelim */

    char *line   = NULL;
    char *field  = NULL;
    segment *seg = NULL;
    char end[3];

    /* Break data into lines */
    while((llen = getdelim(&line, &l, sep[0], fp)) != -1)
    {
        /* Remove the record separator from the line. */
        snprintf(end, 2, "%c", line[llen - 1]);

        if(strcmp(end, sep) == 0)
            line[llen - 1] = 0;

        /* Create fake file pointer for our second scan. */
        if((fake_line = fmemopen(line, strlen(line)+1, "r")) == NULL)
        {
            err = errno;
            fprintf(stderr, "Internal error: aborting\n");
            exit(err);
        }

        /* Construct our segment object */
        seg = segment_ctor(seg);

        /* Now break each line into fields */
        while((flen = getdelim(&field, &f, delim[0], fake_line)) != -1)
        {
            /* Remove the delimiter from the field. */
            snprintf(end, 2, "%c", field[flen - 1]);

            if(strcmp(end, delim) == 0)
                field[flen - 1] = 0;

            /*
             * Make a copy of the field, push it into our Array.
             * Must use free_array to clean up afterward.
             */

            seg = seg->push(seg, field);
        }

        fclose(fake_line);
        msg = msg->push(msg, seg);
    }

    free(line);
    free(field);
    return msg;
}
