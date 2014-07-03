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
#include "jdksavdecc-proxy_apc.h"
#include "jdksavdecc-proxy_net.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JDKSAvdeccProxy_Client_Addr
{
    struct sockaddr *m_addr;
    socklen_t m_addr_len;
    char m_path[4096];
    char m_host[1024];
    char m_port[128];
    char *m_additional_request_headers;
};

/**
 * @brief The JDKSAvdeccProxy_Client_State enum
 *
 * See IEEE Std 1722.1 Annex C.5.3.3, Figure C.3
 *
 */
enum JDKSAvdeccProxy_Client_State
{
    JDKSAvdeccProxy_Client_State_BEGIN = 0,
    JDKSAvdeccProxy_Client_State_INITIALIZE,
    JDKSAvdeccProxy_Client_State_WAIT_FOR_CONNECT,
    JDKSAvdeccProxy_Client_State_ACCEPT,
    JDKSAvdeccProxy_Client_State_START_TRANSFER,
    JDKSAvdeccProxy_Client_State_WAITING,
    JDKSAvdeccProxy_Client_State_CLOSED,
    JDKSAvdeccProxy_Client_State_LINK_STATUS,
    JDKSAvdeccProxy_Client_State_RECEIVE_MSG,
    JDKSAvdeccProxy_Client_State_SEND_MSG,
    JDKSAvdeccProxy_Client_State_ENTITY_ID_ASSIGNED,
    JDKSAvdeccProxy_Client_State_SEND_NOP,
    JDKSAvdeccProxy_Client_State_FINISH,
    JDKSAvdeccProxy_Client_State_END
};

struct JDKSAvdeccProxy_Client
{
    /**
     * @brief m_base dispatch table for network events
     *
     * The effective "Base Class" for receiving network events from a TCP socket
     */
    struct JDKSAvdeccProxy_NetEvent m_base;

    /**
     * @brief m_net TCP network socket
     *
     * The object which manages socket requests
     */
    struct JDKSAvdeccProxy_NetRequest *m_net;

    /**
     * The Current State
     */
    enum JDKSAvdeccProxy_Client_State m_state;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.1
     */
    struct JDKSAvdeccProxy_Client_Addr m_addr;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.2
     */
    struct jdksavdecc_appdu m_apcMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.3
     */
    bool m_apcMsgOut;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.4
     */
    struct jdksavdecc_appdu m_apsMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.5
     */
    bool m_apsMsgIn;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.6
     */
    jdksavdecc_timestamp_in_milliseconds m_currentTime;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.7
     */
    bool m_finished;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.8
     */
    struct jdksavdecc_eui64 m_entityId;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.9
     */
    bool m_idAssigned;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.10
     */
    bool m_incomingTcpClosed;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.11
     */
    struct jdksavdecc_appdu m_linkMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.12
     */
    bool m_linkStatusMsg;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.13
     */
    struct jdksavdecc_eui64 m_newId;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.14
     */
    bool m_nopTimeout;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.15
     */
    struct jdksavdecc_eui48 m_primaryMac;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.16
     */
    bool m_responseValid;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.17
     */
    bool m_tcpConnected;

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.1
     */
    void ( *closeTcpConnection )( struct JDKSAvdeccProxy_Client *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.2
     */
    void ( *connectToProxy )( struct JDKSAvdeccProxy_Client *self, struct JDKSAvdeccProxy_Client_Addr const *addr );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.3
     */
    bool ( *getHttpResponse )( struct JDKSAvdeccProxy_Client *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.4
     */
    void ( *initialize )( struct JDKSAvdeccProxy_Client *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.5
     */
    void ( *notifyLinkStatus )( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_appdu const *linkMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.6
     */
    void ( *processMsg )( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_appdu const *apsMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.7
     */
    void ( *sendIdRequest )( struct JDKSAvdeccProxy_Client *self,
                             struct jdksavdecc_eui48 primaryMac,
                             struct jdksavdecc_eui64 entity_id );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.8
     */
    void ( *sendHttpRequest )( struct JDKSAvdeccProxy_Client *self, struct JDKSAvdeccProxy_Client_Addr const *addr );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.9
     */
    void ( *sendMsgToAps )( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_appdu const *apcMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.10
     */
    void ( *sendNopToAps )( struct JDKSAvdeccProxy_Client *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.X
     */
    void ( *notifyNewEntityId )( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_eui64 entity_id );
};

void JDKSAvdeccProxy_Client_init( struct JDKSAvdeccProxy_Client *self, struct JDKSAvdeccProxy_NetRequest *net );

void JDKSAvdeccProxy_Client_terminate( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_start( struct JDKSAvdeccProxy_Client *self,
                                   struct JDKSAvdeccProxy_Client_Addr const *addr,
                                   struct jdksavdecc_eui48 primaryMac,
                                   struct jdksavdecc_eui64 preferred_entity_id );

void JDKSAvdeccProxy_Client_execute_state( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_finish( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_NetEventSocketReadable( struct JDKSAvdeccProxy_NetEvent *self, void const *data, ssize_t data_len );

void JDKSAvdeccProxy_Client_NetEventSocketConnected( struct JDKSAvdeccProxy_NetEvent *self );

void JDKSAvdeccProxy_Client_NetEventSocketWritable( struct JDKSAvdeccProxy_NetEvent *self );

void JDKSAvdeccProxy_Client_NetEventSocketClosed( struct JDKSAvdeccProxy_NetEvent *self );

void JDKSAvdeccProxy_Client_NetEventSocketTimePassed( struct JDKSAvdeccProxy_NetEvent *self,
                                                      jdksavdecc_timestamp_in_milliseconds timestamp );

void JDKSAvdeccProxy_Client_goto_state_BEGIN( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_BEGIN( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_INITIALIZE( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_INITIALIZE( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_WAIT_FOR_CONNECT( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_WAIT_FOR_CONNECT( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_ACCEPT( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_ACCEPT( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_START_TRANSFER( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_START_TRANSFER( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_WAITING( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_WAITING( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_CLOSED( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_CLOSED( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_LINK_STATUS( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_LINK_STATUS( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_RECEIVE_MSG( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_RECEIVE_MSG( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_SEND_MSG( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_SEND_MSG( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_ENTITY_ID_ASSIGNED( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_ENTITY_ID_ASSIGNED( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_SEND_NOP( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_SEND_NOP( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_FINISH( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_FINISH( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_goto_state_END( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_execute_state_END( struct JDKSAvdeccProxy_Client *self );

bool JDKSAvdeccProxy_Client_send( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_frame const *avdecc_frame );

void JDKSAvdeccProxy_Client_CloseTcpConnection( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_ConnectToProxy( struct JDKSAvdeccProxy_Client *self,
                                            struct JDKSAvdeccProxy_Client_Addr const *addr );

bool JDKSAvdeccProxy_Client_GetHttpResponse( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_Initialize( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_NotifyLinkStatus( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_appdu const *linkMsg );

void JDKSAvdeccProxy_Client_ProcessMsg( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_appdu const *apsMsg );

void JDKSAvdeccProxy_Client_SendIdRequest( struct JDKSAvdeccProxy_Client *self,
                                           struct jdksavdecc_eui48 primaryMac,
                                           struct jdksavdecc_eui64 entity_id );

void JDKSAvdeccProxy_Client_SendHttpRequest( struct JDKSAvdeccProxy_Client *self,
                                             struct JDKSAvdeccProxy_Client_Addr const *addr );

void JDKSAvdeccProxy_Client_SendMsgToAps( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_appdu const *apcMsg );

void JDKSAvdeccProxy_Client_SendNopToAps( struct JDKSAvdeccProxy_Client *self );

void JDKSAvdeccProxy_Client_NotifyNewEntityId( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_eui64 entity_id );

#ifdef __cplusplus
}
#endif
