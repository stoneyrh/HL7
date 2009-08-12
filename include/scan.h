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

#ifndef _HL7_SCAN_H_
#define _HL7_SCAN_H_ 1

#define _GNU_SOURCE
#include "common.h"

/**
 * \file scan.h
 *
 * \brief Structures describing arrays and multidimensional
 * arrays, as well as function prototypes for handling them.
 *
 * Yes, yes, I know that char ** arrays are already
 * multidimensional, but I've been brainwashed by Python lists, 
 * PHP arrays, and C++ vectors. And I like it. (:
 *
 */

/*
 * Array. 
 * TODO: Make this more generic?
 */

typedef struct _Array
{
    int len;
    int size;
    char **data;
} Array;

Array * array_init(void);
Array * push(Array * array, const char * item);
Array * array_scan(FILE * fp, char * sep, char * delim);
void free_array(Array * array);

/*
 * Multidimensional Array.
 *
 * These should be analogous with their Array counterparts, only
 * returning and functioning with Multi's. 
 */

typedef struct _Multi
{
    int len;
    int size;
    Array **members;
} Multi;

Multi * multi_init(void);
Multi * mpush(Multi * multi, Array * array);
Multi * multi_scan(FILE * fp, char * sep, char * delim);
void free_multi(Multi * multi);


/* For debugging/lazy typers. Prints to stderr if DEBUG 
 * is defined. Does nothing if it isn't.
 */

void d(const char * fmt, ...);

#endif
