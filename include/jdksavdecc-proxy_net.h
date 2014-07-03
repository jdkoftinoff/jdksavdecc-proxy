#pragma once

/*
 Copyright (c) 2014, J.D. Koftinoff Software, Ltd. <jeffk@jdkoftinoff.com>
 http://www.jdkoftinoff.com/
 All rights reserved.

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "jdksavdecc-proxy_world.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JDKSAvdeccProxy_NetRequest;

struct JDKSAvdeccProxy_NetEvent;

struct JDKSAvdeccProxy_NetEvent
{
    /**
     * External Networking Event: The socket was readable and some data was read
     */
    void ( *netEventSocketReadable )( struct JDKSAvdeccProxy_NetEvent *self, void const *data, ssize_t data_len );

    /**
     * External Networking Event: The socket was connected
     */
    void ( *netEventSocketConnected )( struct JDKSAvdeccProxy_NetEvent *self );

    /**
     * External Networking Event: The socket is writable now
     */
    void ( *netEventSocketWritable )( struct JDKSAvdeccProxy_NetEvent *self );

    /**
     * External Networking Event: The socket was closed
     */
    void ( *netEventSocketClosed )( struct JDKSAvdeccProxy_NetEvent *self );

    /**
     * External Networking Event: Some time passed
     */
    void ( *netEventSocketTimePassed )( struct JDKSAvdeccProxy_NetEvent *self, jdksavdecc_timestamp_in_milliseconds timestamp );
};

struct JDKSAvdeccProxy_NetRequest
{
    /**
     * External Networking Request: The state machine wants to wake up when the socket is writable
     */
    void ( *netRequestWakeOnWritable )( struct JDKSAvdeccProxy_NetRequest *self,
                                        struct JDKSAvdeccProxy_NetEvent *netEventHandler,
                                        bool enable );

    /**
     * External Networking Request: The state machine wants to connect to a destination
     */
    void ( *netRequestConnect )( struct JDKSAvdeccProxy_NetRequest *self,
                                 struct JDKSAvdeccProxy_NetEvent *netEventHandler,
                                 struct sockaddr const *addr,
                                 socklen_t addr_len );

    /**
     * External Networking Request: The state machine wants to close the socket
     */
    void ( *netRequestClose )( struct JDKSAvdeccProxy_NetRequest *self, struct JDKSAvdeccProxy_NetEvent *netEventHandler );

    /**
     * External Networking Request: The state machine wants to send some data
     */
    ssize_t ( *netRequestSend )( struct JDKSAvdeccProxy_NetRequest *self,
                                 struct JDKSAvdeccProxy_NetEvent *netEventHandler,
                                 void const *data,
                                 size_t data_len );

    /**
     * External Networking Request: The state machine wants to be woken up in the future
     */
    void ( *netRequestWakeUp )( struct JDKSAvdeccProxy_NetRequest *self,
                                struct JDKSAvdeccProxy_NetEvent *netEventHandler,
                                uint32_t delta_time_in_milliseconds );
};

#ifdef __cplusplus
}
#endif
