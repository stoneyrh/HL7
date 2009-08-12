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

#include "scan.h"

/**
 * \file array.c
 * \brief
 *      Basic functions to facilitate the handling (creation, 
 *      population, and deletion) of dynamic arrays. This could
 *      technically be used for darn near anything, it's just
 *      handy for HL7 as well.
 */



/**
 * \fn array_init
 * \brief Initializes an array.
 * \returns Array - A newly initialized array.
 *
 * Required before any other functions are used.
 */

Array * 
array_init(void)
{
    /* Allocate the initial array structure */
    Array *array = calloc(1, sizeof(Array));

    array->data = NULL; /* actual storage - indexed by zero. */
    array->size = 0;
    array->len = 0;
    return array;
}

/**
 * \fn push
 * \brief 
 *      Adds item to Array. Allocates memory dynamically.
 *
 * \param array - the Array to add to.
 * \param item - character pointer to add to Array.
 * \returns pointer to an Array, with item added to it.
 */


Array *
push(Array *array, const char *item)
{
    size_t len;
    void *copy;

    /*
     * Don't bother if array hasn't been initialized.
     **/

    if(array != NULL && item != NULL)
    {
        array->data = realloc(array->data, sizeof(char*) * ++array->size);

        if(array->data == NULL)
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

        array->len = array->size - 1;
        array->data[array->len] = (char*)memcpy(copy, item, len);
    }

    return array;
}

/**
 * \fn free_array
 * \brief 
 *      Iterates over and frees all elements in array.
 *
 * \param array - the array to free.
 */


void
free_array(Array *array)
{
    int i;

    if(array != NULL)
    {
        for(i=0; i != array->size ;i++)
            free(array->data[i]);

        free(array->data);
        free(array);
    }
    return;
}


/** 
 * \para 
 *      Multidimensional storage container.
 */

/**
 * \fn multi_init
 * \brief 
 *      Initializes an Array of Arrays.
 *
 * \param none
 * \returns a pointer to a Multi.
 */

Multi * 
multi_init(void)
{
    /* Allocate the initial array structure */
    Multi *multi = malloc(sizeof(Multi));

    multi->members = NULL; /* actual storage - indexed by zero. */
    multi->size = 0;
    multi->len = 0;
    return multi;
}

/**
 * \fn mpush
 * \brief 
 *      Appends array to multi.
 *
 * \param array - the Array to add to multi.
 * \param multi - the Multi to add the Array to.
 * \returns pointer to multi, with Array added to it.
 */

Multi *
mpush(Multi *multi, Array *array)
{

    /*
     * Don't bother if multi hasn't been initialized.
     **/

    if(multi != NULL)
    {
        multi->members = realloc(multi->members, 
                                 sizeof(Array *) * ++multi->size);
        if(multi->members == NULL)
        {
            fprintf(stderr, "%s: %d: Out of memory!\n",
                    __func__, __LINE__);
            exit(EXIT_FAILURE);
        }

        multi->len = multi->size - 1;
        multi->members[multi->len] = array;
    }

    return multi;
}

/**
 * \fn free_multi
 * \brief 
 *      Frees *all* data in multi. Iterates over the multi, 
 *      and over each individual array.
 *
 * \param multi - the multi to free.
 */

void 
free_multi(Multi *multi)
{
    int i;
    if(multi != NULL)
    {
        for(i=0; i != multi->size ;i++)
            free_array(multi->members[i]);

        free(multi->members);
        free(multi);
    }
    return;
}
