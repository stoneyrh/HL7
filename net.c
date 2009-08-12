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


#include "net.h"

/**
 * \fn tcp_connect
 * \brief Establishes a connection with host, on the given port.
 *
 * \param host - hostname to connect to.
 * \param port - port to connect to.
 * \returns - int file descriptor of socket on success. Returns 
 * -1 on socket failure, -2 on hostname/DNS failure, -3 on failure
 *  to connect.
 */

int
tcp_connect(const char *host, int port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1)
        return -1;
    
    server = gethostbyname(host);

    if(server == NULL)
    {
        close(sockfd);
        return -2;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char *) server->h_addr,
          (char *) &serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        close(sockfd);
        return -3;
    }
    return sockfd;
}

/** 
 * \fn set_recv_wait
 * \brief - Convenience function to set socket receive timeout.
 *
 * \param int sockfd - File descriptor to set timeout on.
 *
 * \param int ms - How long to wait for data, in milliseconds.
 *
 * \returns - Same as setsockopt - zero on success, on failure
 * returns -1 and sets errno.
 */

int
set_recv_wait(int sockfd, int ms)
{
    struct timeval tv;

    tv.tv_sec = ms / 1000;
    tv.tv_usec = ( ms % 1000) * 1000;

    return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof tv);
}

/** 
 * \fn set_send_wait
 * \brief - Convenience function to set socket send timeout.
 *
 * \param int sockfd - File descriptor to set timeout on.
 *
 * \param int ms - How long to wait for send to complete, in milliseconds.
 *
 * \returns - Same as setsockopt - zero on success, on failure
 * returns -1 and sets errno.
 */

int
set_send_wait(int sockfd, int ms)
{
    struct timeval tv;

    tv.tv_sec = ms / 1000;
    tv.tv_usec = ( ms % 1000) * 1000;

    return setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof tv);
}


/** 
 * \fn tcp_send
 * \brief - Convenience function to send buffer of data, with option
 *          to time out.
 *
 * \param int sockfd - File descriptor to send on.
 *
 * \param char *buf - Pointer to buffer - what we're sending.
 *
 * \param int ms - how long to attempt to send the data, in 
 *   milliseconds. If zero or less, will block indefinitely.
 *
 * \returns - On success returns number of bytes sent. On failure
 *            returns -1.
 */

int
tcp_send(int sockfd, char *buf, int ms)
{
    int snt,
        ret;

    ret = 0;

    if(ms > 0)
        if(set_send_wait(sockfd, ms) != 0)
            ret = -1;
    
    while((snt = send(sockfd, buf, sizeof(buf), 0)) > 0)
        ret += snt;

    return ret;
}

/**
 * \fn tcp_recv
 *
 * \param int sockfd - File descriptor to receive from.
 *
 * \param int max - Maximum number of bytes to receive. If 0, will
 * consume as much as we're given. (Not recommended.)
 *
 * \param int ms - How long to attempt to send the data, in milliseconds.
 * If 0 or less, will wait indefinitely.
 *
 * \param int total - Reference to int, populates with total number 
 *  of characters read, 
 */

char *
tcp_recv(int sockfd, int ms, int max, int *total)
{
    char * buf = NULL;
    char * msg = NULL;
    int in,
        chunk,
        req,
        i;

    chunk = 64;

    if((ms > 0) && (sockfd > -1) 
            && (set_recv_wait(sockfd, ms) == 0) 
            && ((buf = (char*)calloc(1, chunk * sizeof(char*))) != NULL)
            && ((msg = (char*)calloc(1, chunk * sizeof(char*))) != NULL))
    {
        in = i = req = 0;

        if(max > 0) 
        {
            /* We've been provided a maximum,
             * so don't let the other side DoS 
             * us.
             */
            while((in = recv(sockfd, buf, chunk, 0)) > 0)
            {
                if(req >= max)
                {
                    free(buf);
                    free(msg);
                    exit(EMSGSIZE);
                }

                req =  (strlen(msg)+in) + ++i;
                msg = realloc(msg, req);
                msg = strncat(msg, buf, in);
            }

            free(buf);
            *total = strlen(msg);
        }
        else
        {
            /* Throw caution to the wind.  */
            while((in = recv(sockfd, buf, chunk, 0)) > 0)
            {
                req =  (strlen(msg)+in) + ++i;
                msg = realloc(msg, req);
                msg = strncat(msg, buf, in);
            }

            free(buf);
            *total = strlen(msg);
        }
    }
    return msg;
}
