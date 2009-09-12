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

#ifndef _HL7_SEGMENT_H_
#define _HL7_SEGMENT_H_

#include <stdio.h>  /* fprintf, getdelim */
#include <stdlib.h> /* calloc, realloc, free */
#include <string.h> /* strlen */
#include <errno.h>  /* errno and various error definitions - ENOMEM, etc. */
#include <stddef.h> /* offsetof(struct, pos)  - mainly for debugging. */

typedef struct _segment
{
    int len;
    void **data;

    /* member functions */
    struct _segment *(*push)(struct _segment *, const void *);

/**
 * \fn segment->parse(segment *self, char *line, const char *delim)
 * \brief
 *      Handles breaking a segment into its own individual fields.
 */

    struct _segment *(*parse)(struct _segment *, char *, const char *);

    void *(*begin)(struct _segment *);
    void *(*end)(struct _segment *);

    int (*first)(struct _segment *);
    int (*last)(struct _segment *);

    void (*dtor)(struct _segment *);
} segment;

typedef struct _segment_iter
{
    int first;
    int last;
    int state;
    segment * klass;

    /* member functions */
    void *(*begin)(struct _segment *);
    void *(*end)(struct _segment_iter *);
    void (*dtor)(struct _segment_iter *);
    void *(*next)(struct _segment_iter *);
} segment_iter;


/* segment member functions */

/**
 * \fn segment_ctor
 * \brief 
 *      Constructor for the segment structure. Initializes a segment.
 *
 * \param self - the segment we're initializing.
 * \returns the initialized segment.
 */

segment * segment_ctor(segment *self);

/**
 * \fn segment_push
 * \brief 
 *      Pushes item into segment.
 *
 * \param a - the segment to contain the item
 * \param item - the item to push into segment.
 * \returns the segment, after the push operation.
 */

segment * segment_push(segment *self, const void *item);

/**
 * \fn segment_begin
 * \brief 
 *      Gets the first element in an segment.
 *
 * \param a - the segment we're referencing
 * \returns the first element in the segment.
 */

void * segment_begin(segment *self);

/**
 * \fn segment_end
 * \brief 
 *      Gets one past the last element in an segment.
 *
 * \param a - the segment we're referencing
 * \returns the one past the last element in the segment.
 */

void * segment_end(segment *self);

/**
 * \fn segment_first
 * \brief 
 *      Gets the first position in an segment.
 *
 * \param a - the segment we're referencing
 * \returns the first position in the segment.
 */

int segment_first(segment *self);

/**
 * \fn segment_last
 * \brief 
 *      Gets the last position in an segment.
 *
 * \param a - the segment we're referencing
 * \returns the last position in the segment.
 */

int segment_last(segment *self);

/**
 * \fn segment_dtor
 * \brief 
 *      The destructor for the segment datatype. Iterates over and frees all elements in a.
 *
 * \param a - the segment to free.
 */

void segment_dtor(segment *self);

/* segment iterator member functions */

segment_iter * segment_iter_ctor(segment *self);

/**
 * \fn segment_iter_begin
 * \brief 
 *      Gets the first element in an segment.
 *
 * \param a - the iterator we're referencing
 * \returns the first element in the segment.
 */

void * segment_iter_begin(segment_iter *i);

/**
 * \fn segment_iter_end
 * \brief 
 *      Gets one past the last element of the iterator.
 *
 * \param i - the iterator to reference.
 * \returns One past the last element of the iterator.
 */

void * segment_iter_end(segment_iter *i);

/**
 * \fn segment_iter_next
 * \brief 
 *      Gets the next element of the iterator.
 *
 * \param i - the iterator to reference.
 * \returns the next element of the iterator.
 */

void * segment_iter_next(segment_iter *i);

void segment_iter_dtor(segment_iter *i);


/**
 * \fn segment_parse()
 */
segment * segment_parse(segment *self, char *line, const char *delim);

#endif
