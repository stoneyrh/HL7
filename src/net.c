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
#include "proto.h"

#if 0
    bool Socket::create()
    {
        int tw; /* TIME_WAIT */
        sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

        if (!valid())
            return false;


        tw = 1;

        /* TODO: As noted in bind and listen, need to peek
         * at return value and act accordingly.
         */

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(const char *)&tw, sizeof(tw)) == -1)
            return false;

        return true;
    }
#endif

bool
sock_create(int *sockfd)
{
    int tw = 1;
    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(*sockfd == -1)
        return false;

    if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR,(const char *)&tw, sizeof(tw)) == -1)
        return false;


    return true;
}



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

bool
tcp_connect(const char *host, int port, int *sockfd)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if(!sock_create(sockfd))
        return false;
    
    server = gethostbyname(host);
    if(server == NULL)
    {
        close(*sockfd);
        *sockfd = -2;
        return false;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr,(char *)server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        close(*sockfd);
        *sockfd = -3;
        return false;
    }
    return true;
}

/** 
 * \fn set_recv_wait
 * \brief - Convenience function to set socket receive timeout.
 *
 * \param int sockfd - File descriptor to set timeout on.
 *
 * \param int ms - How long to wait for data, in milliseconds.
 *
 * \returns - true  on success, false on failure (sets errno).
 */

bool
set_recv_wait(int sockfd, int ms)
{
    struct timeval tv;

    tv.tv_sec = ms / 1000;
    tv.tv_usec = ( ms % 1000) * 1000;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof tv) == -1)
        return false;

    return true;
}

/** 
 * \fn set_send_wait
 * \brief - Convenience function to set socket send timeout.
 *
 * \param int sockfd - File descriptor to set timeout on.
 *
 * \param int ms - How long to wait for send to complete, in milliseconds.
 *
 * \returns - true on success, false on failure (sets errno).
 */

bool
set_send_wait(int sockfd, int ms)
{
    struct timeval tv;

    tv.tv_sec = ms / 1000;
    tv.tv_usec = ( ms % 1000) * 1000;

    if(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof tv) == -1)
        return false;

    return true;
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
 * \param int *sent - pass by reference to get number of bytes sent.
 *
 * \returns - true on success, false on failure.
 */

bool
tcp_send(int sockfd, char *buf, int ms, int *sent)
{
    int snt;
    *sent = 0;
#if 0
        while(*sent < strlen(buf) + 1 )
        {
            snt = send(sockfd, buf, strlen(buf)+1, 0);
            *sent += snt;
        }
#endif

    if(ms > 0)
    {
        if(!set_send_wait(sockfd, ms))
            return false;

        while((snt = send(sockfd, buf, strlen(buf)+1, 0)) > 0)
            *sent += snt;
    }
    return (*sent > 0);
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
    *total = 0;

    if(!set_recv_wait(sockfd, ms) || !(ms > 0))
        return buf;


    if((buf = (char*)calloc(1, chunk * sizeof(char))) == NULL)
        return buf;

    
    if((msg = (char*)calloc(1, chunk * sizeof(char))) == NULL)
    {
        free(buf);
        buf = NULL;
        return buf;
    }


    if((ms > 0) && (sockfd > -1))
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
                    /* We've hit our limit. Return what we're allowed.*/
                    free(buf);
                    *total = strlen(msg);
                    return msg;
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

#ifdef USE_OLD_RESOLVE
bool 
lookup(const char *s, const char *resolved)
{
    struct hostent *hp;

    if(inet_aton(s, NULL))
    {
        /* Is an IP address */
        hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);

        if(hp != NULL)
        {
            resolved = hp->h_name;
            return true;
        }
    }
    else
    {
        /* Not an IP address */
        hp = gethostbyname(s);

        if(hp!=NULL)
        {
            resolved = inet_ntoa(*(struct in_addr*)hp->h_addr_list[0]);
            return true;
        }
    }
    return false;
}
#else
bool
lookup(const char *s, const char *resolved)
{
    char hostbuf[NI_MAXHOST];
    struct addrinfo *res;
    struct addrinfo hints;
    const char *addr;
    int     err;

    memset((char *) &hints, 0, sizeof(hints));

    hints.ai_family = AF_INET; /* AF_INET - ipv4.
                                * AF_INET6 - ipv6.
                                * PF_UNSPEC - both / either.
                                */

    hints.ai_flags = AI_CANONNAME; /* Flag to populate ai_canonname 
                                    * with the 'official' name for the host.
                                    */

    hints.ai_socktype = SOCK_STREAM; /* NOTE: use 0 for any socket type. */

    if ((err = getaddrinfo(s, NULL, &hints, &res)) != 0)
    {
        /*host/address lookup failure.*/
        freeaddrinfo(res);
        return false;
    }

    if(inet_aton(s, NULL))
    {
        /* We've been given an IP, retreive the name. */
        err = getnameinfo(res->ai_addr, res->ai_addrlen, hostbuf, 
                sizeof(hostbuf), NULL, 0, NI_NAMEREQD);

        if(err)
        {
            /* getnameinfo failed. Free our resources and bail.
             * Note - could use gai_strerror(err) to inspect
             * the error.
             */
            freeaddrinfo(res);
            return false;
        }
    }
    else
    {
        /* We've been given a host, get the IP address. */
        addr = (char *) &((struct sockaddr_in *) res->ai_addr)->sin_addr;

        if (inet_ntop(res->ai_family, addr, hostbuf, sizeof(hostbuf)) == 0)
        {
            freeaddrinfo(res);
            return false;
        }
    }
    resolved = hostbuf;
    freeaddrinfo(res);
    return true;
}
#endif
