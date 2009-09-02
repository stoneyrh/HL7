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

#ifndef _HL7_MESSAGE_H_
#define _HL7_MESSAGE_H_

#include <stdio.h>  /* fprintf */
#include <stdlib.h> /* calloc, realloc, free */
#include <string.h> /* strlen */
#include <errno.h>  /* errno */
#include <stddef.h> /* offsetof(struct, pos) */
#include <malloc.h> /* ptrdiff_t */

#include <hl7c/segment.h>

typedef struct _message
{
    int len;
    segment **segments;

    /* member functions */
    struct _message *(*push)(struct _message *, segment *);

    segment *(*begin)(struct _message *);
    segment *(*end)(struct _message *);

    int (*first)(struct _message *);
    int (*last)(struct _message *);

/**
 * \fn message->parse(message *msg, FILE *fp, char *sep, char *delim)
 * \brief
 *      Parses over a file stream, and builds out segment objects
 *      within the message object.
 *
 *  \param msg
 *  \param fp
 *  \param sep
 *  \param delim
 */

    struct _message *(*parse)(struct _message *, FILE *, const char *, const char *);
    void (*dtor)(struct _message *);
} message;

typedef struct _message_iter
{
    int first;
    int last;
    int state;
    message * klass;

    /* member functions */
    segment *(*begin)(struct _message *);
    segment *(*end)(struct _message_iter *);
    void *(*next)(struct _message_iter *);
    void (*dtor)(struct _message_iter *);
} message_iter;


/* message member functions */

/**
 * \fn message_ctor
 * \brief 
 *      Constructor for the message structure. Initializes a message.
 *
 * \param self - the message we're initializing.
 * \returns the initialized message.
 */

message * message_ctor(message *self);

/**
 * \fn message_push
 * \brief 
 *      Pushes item into message.
 *
 * \param a - the message to contain the item
 * \param item - the item to push into message.
 * \returns the message, after the push operation.
 */

message * message_push(message *self, segment *seg);

/**
 * \fn message_begin
 * \brief 
 *      Gets the first element in an message.
 *
 * \param a - the message we're referencing
 * \returns the first element in the message.
 */

segment * message_begin(message *self);

/**
 * \fn message_end
 * \brief 
 *      Gets one past the last element in an message.
 *
 * \param a - the message we're referencing
 * \returns the one past the last element in the message.
 */

segment * message_end(message *self);

/**
 * \fn message_first
 * \brief 
 *      Gets the first position in an message.
 *
 * \param a - the message we're referencing
 * \returns the first position in the message.
 */

int message_first(message *self);

/**
 * \fn message_last
 * \brief 
 *      Gets the last position in an message.
 *
 * \param a - the message we're referencing
 * \returns the last position in the message.
 */

int message_last(message *self);

/**
 * \fn message_dtor
 * \brief 
 *      The destructor for the message datatype. Iterates over and frees all elements in a.
 *
 * \param a - the message to free.
 */

void message_dtor(message *self);

/* message iterator member functions */

message_iter * message_iter_ctor(message *self);

/**
 * \fn message_iter_begin
 * \brief 
 *      Gets the first element in an message.
 *
 * \param a - the iterator we're referencing
 * \returns the first element in the message.
 */

segment * message_iter_begin(message_iter *i);

/**
 * \fn message_iter_end
 * \brief 
 *      Gets one past the last element of the iterator.
 *
 * \param i - the iterator to reference.
 * \returns One past the last element of the iterator.
 */

segment * message_iter_end(message_iter *i);

/**
 * \fn message_iter_next
 * \brief 
 *      Gets the next element of the iterator.
 *
 * \param i - the iterator to reference.
 * \returns the next element of the iterator.
 */

void * message_iter_next(message_iter *i);

/**
 * \fn message_iter_dtor
 * \brief destructor for the message iterator.
 */

void message_iter_dtor(message_iter *i);


/**
 * \fn message_parse
 */

message * message_parse(message *msg, FILE *fp, const char *sep, const char *delim);

#endif
