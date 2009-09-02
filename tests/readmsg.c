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
#include <errno.h>
#include <string.h>
#include <hl7c/proto.h>

const char *filename = "../data/adt_a04_13885_20090811203018";

bool
testread(int argc, char **argv)
{
    FILE *fp;
    char *msg;
    int s = 0;
    bool ret = true;

    s = getsize(filename);

    if(!s)
        return false; // empty file;

    if((fp=fopen(filename, "r"))==NULL)
        return false; // couldn't open file.

    if((msg = calloc(1, s*sizeof(char))) == NULL)
        return false; // memory error.

    if(read(fileno(fp), msg, s) != s)
        return false; // Failed/partial read.

    fclose(fp);

    msg = trim_left(msg, "\v");
    msg = trim_right(msg, '\n');

    ret = readmsg(msg);

    free(msg);

    return ret;
}
