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

#include <hl7c/segment.h>

/**
 * \file segment.c
 * \brief 
 *      Member function definitions for the segment structure.
 */


/**
 * \fn segment_ctor
 * \brief 
 *      Constructor for the segment structure. Initializes a segment.
 *
 * \param self - the segment we're initializing.
 * \returns the initialized segment.
 */

segment *
segment_ctor(segment *self)
{
    /* Allocate the initial segment structure */
    self = calloc(1, sizeof(segment));
    self->len = 0;
    self->data = NULL; /* actual storage - indexed by zero. */

    /* set up member functions */
    self->dtor = segment_dtor;
    self->push = segment_push;
    self->begin = segment_begin;
    self->end = segment_end;

    self->first = segment_first;
    self->last = segment_last;

    return self;
}


/**
 * \fn segment_begin
 * \brief 
 *      Gets the first element in an segment.
 *
 * \param self - the segment we're referencing
 * \returns the first element in the segment.
 */

void * 
segment_begin(segment *self)
{
    return *(self->data + 0);
}

/**
 * \fn segment_end
 * \brief 
 *      Gets one past the last element in an segment.
 *
 * \param self - the segment we're referencing
 * \returns the one past the last element in the segment.
 */

void *
segment_end(segment *self)
{
    return *(self->data + self->len);
}

/**
 * \fn segment_first
 * \brief 
 *      Gets the first position in an segment.
 *
 * \param self - the segment we're referencing
 * \returns the first position in the segment.
 */

int
segment_first(segment *self)
{
    return 0;
}

/**
 * \fn segment_last
 * \brief 
 *      Gets the last position in an segment.
 *
 * \param a - the segment we're referencing
 * \returns the last position in the segment.
 */

int
segment_last(segment *s)
{
    return (s->len - 1);
}

/**
 * \fn segment_push
 * \brief 
 *      Pushes item into segment.
 *
 * \param a - the segment to contain the item
 * \param item - the item to push into segment.
 * \returns the segment, after the push operation.
 */

segment *
segment_push(segment *s, const void *item)
{
    size_t len;
    void *copy;

    /*
     * Don't bother if a hasn't been initialized.
     */
    if(s != NULL && item != NULL)
    {
        s->data = realloc(s->data, sizeof(char*) * ++s->len);

        if(s->data == NULL)
        {
            fprintf(stderr, "%s: %d: Out of memory!\n",
                    __func__, __LINE__);
            exit(ENOMEM);
        }

        /*
         * Below is almost exactly how strdup is implemented by
         * the GNU devs, except that I've made "out of memory"
         * a fatal error, instead of just returning NULL.
         **/

        len = strlen(item) + 1;
        copy = malloc(len);

        if(copy == NULL)
        {
            fprintf(stderr, "%s: %d: Out of memory!\n",
                    __func__, __LINE__);
            exit(ENOMEM);
        }
        s->data[s->len -1] = (char*)memcpy(copy, item, len);
    }
    return s;
}


/**
 * \fn segment_dtor
 * \brief 
 *      The segment datatype's destructor. Iterates over and frees all elements in a.
 *
 * \param a - the segment to free.
 */


void
segment_dtor(segment *s)
{
    void *it;
    segment_iter *i;

    for(i = segment_iter_ctor(s), it = i->begin(s); it != i->end(i); it = i->next(i))
        free(it);

    i->dtor(i);
    free(s->data);
    free(s);

    return;
}

segment_iter *
segment_iter_ctor(segment *s)
{
    segment_iter *i = calloc(1, sizeof(segment_iter));
    /* setup our member functions... */
    i->klass = s;
    i->begin = s->begin;
    i->end   = segment_iter_end;
    i->dtor = segment_iter_dtor;
    i->next = segment_iter_next;

    i->first = s->first(s);
    i->last = s->last(s);
    i->state = s->first(s);
    return i;
}

void *
segment_iter_next(segment_iter *i)
{
    void *it;

    if(i->state != i->last)
        it = i->klass->data[++i->state];
    else
    {
        i->state = i->first;
        it = NULL;
    }

    return it;
}

/**
 * \fn segment_iter_begin
 * \brief 
 *      Gets the first element of the iterator.
 *
 * \param a - the iterator we're referencing
 * \returns the first element of the iterator.
 */

void *
segment_iter_begin(segment_iter *i)
{
    return i->klass->data[i->first];
}

/**
 * \fn segment_iter_end
 * \brief 
 *      Gets one past the last element of the iterator.
 *
 * \param i - the iterator to reference.
 * \returns One past the last element of the iterator.
 */

void *
segment_iter_end(segment_iter *i)
{
    return i->klass->end(i->klass);
}

void
segment_iter_dtor(segment_iter *i)
{
    free(i);
    return;
}
