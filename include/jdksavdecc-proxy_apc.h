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
#include "jdksavdecc-proxy_app.h"
#include "jdksavdecc-proxy_net.h"

#ifdef __cplusplus
extern "C" {
#endif

struct JDKSAvdeccProxy_APC_Addr
{
    struct sockaddr *m_addr;
    socklen_t m_addr_len;
    char m_path[4096];
    char m_host[1024];
    char m_port[128];
    char *m_additional_request_headers;
};

/**
 * @brief The JDKSAvdeccProxy_APC_State enum
 *
 * See IEEE Std 1722.1 Annex C.5.3.3, Figure C.3
 *
 */
enum JDKSAvdeccProxy_APC_State
{
    JDKSAvdeccProxy_APC_State_BEGIN = 0,
    JDKSAvdeccProxy_APC_State_INITIALIZE,
    JDKSAvdeccProxy_APC_State_WAIT_FOR_CONNECT,
    JDKSAvdeccProxy_APC_State_ACCEPT,
    JDKSAvdeccProxy_APC_State_START_TRANSFER,
    JDKSAvdeccProxy_APC_State_WAITING,
    JDKSAvdeccProxy_APC_State_CLOSED,
    JDKSAvdeccProxy_APC_State_LINK_STATUS,
    JDKSAvdeccProxy_APC_State_RECEIVE_MSG,
    JDKSAvdeccProxy_APC_State_SEND_MSG,
    JDKSAvdeccProxy_APC_State_ENTITY_ID_ASSIGNED,
    JDKSAvdeccProxy_APC_State_SEND_NOP,
    JDKSAvdeccProxy_APC_State_FINISH,
    JDKSAvdeccProxy_APC_State_END
};

struct JDKSAvdeccProxy_APC
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
    enum JDKSAvdeccProxy_APC_State m_state;

    /**
     * See IEEE 1722.1 Annex C.5.3.1.1
     */
    struct JDKSAvdeccProxy_APC_Addr m_addr;

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
    void ( *closeTcpConnection )( struct JDKSAvdeccProxy_APC *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.2
     */
    void ( *connectToProxy )( struct JDKSAvdeccProxy_APC *self, struct JDKSAvdeccProxy_APC_Addr const *addr );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.3
     */
    bool ( *getHttpResponse )( struct JDKSAvdeccProxy_APC *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.4
     */
    void ( *initialize )( struct JDKSAvdeccProxy_APC *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.5
     */
    void ( *notifyLinkStatus )( struct JDKSAvdeccProxy_APC *self, struct jdksavdecc_appdu const *linkMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.6
     */
    void ( *processMsg )( struct JDKSAvdeccProxy_APC *self, struct jdksavdecc_appdu const *apsMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.7
     */
    void ( *sendIdRequest )( struct JDKSAvdeccProxy_APC *self,
                             struct jdksavdecc_eui48 primaryMac,
                             struct jdksavdecc_eui64 entity_id );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.8
     */
    void ( *sendHttpRequest )( struct JDKSAvdeccProxy_APC *self, struct JDKSAvdeccProxy_APC_Addr const *addr );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.9
     */
    void ( *sendMsgToAps )( struct JDKSAvdeccProxy_APC *self, struct jdksavdecc_appdu const *apcMsg );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.10
     */
    void ( *sendNopToAps )( struct JDKSAvdeccProxy_APC *self );

    /**
     * See IEEE Std 1722.1 Annex C.5.3.2.X
     */
    void ( *notifyNewEntityId )( struct JDKSAvdeccProxy_APC *self, struct jdksavdecc_eui64 entity_id );
};

void JDKSAvdeccProxy_APC_init( struct JDKSAvdeccProxy_APC *self, struct JDKSAvdeccProxy_NetRequest *net );

void JDKSAvdeccProxy_APC_terminate( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_start( struct JDKSAvdeccProxy_APC *self,
                                struct JDKSAvdeccProxy_APC_Addr const *addr,
                                struct jdksavdecc_eui48 primaryMac,
                                struct jdksavdecc_eui64 preferred_entity_id );

void JDKSAvdeccProxy_APC_execute_state( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_finish( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_NetEventSocketReadable( struct JDKSAvdeccProxy_NetEvent *self, void const *data, ssize_t data_len );

void JDKSAvdeccProxy_APC_NetEventSocketConnected( struct JDKSAvdeccProxy_NetEvent *self );

void JDKSAvdeccProxy_APC_NetEventSocketWritable( struct JDKSAvdeccProxy_NetEvent *self );

void JDKSAvdeccProxy_APC_NetEventSocketClosed( struct JDKSAvdeccProxy_NetEvent *self );

void JDKSAvdeccProxy_APC_NetEventSocketTimePassed( struct JDKSAvdeccProxy_NetEvent *self,
                                                   jdksavdecc_timestamp_in_milliseconds timestamp );

void JDKSAvdeccProxy_APC_goto_state_BEGIN( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_BEGIN( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_INITIALIZE( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_INITIALIZE( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_WAIT_FOR_CONNECT( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_WAIT_FOR_CONNECT( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_ACCEPT( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_ACCEPT( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_START_TRANSFER( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_START_TRANSFER( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_WAITING( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_WAITING( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_CLOSED( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_CLOSED( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_LINK_STATUS( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_LINK_STATUS( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_RECEIVE_MSG( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_RECEIVE_MSG( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_SEND_MSG( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_SEND_MSG( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_ENTITY_ID_ASSIGNED( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_ENTITY_ID_ASSIGNED( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_SEND_NOP( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_SEND_NOP( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_FINISH( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_FINISH( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_goto_state_END( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_execute_state_END( struct JDKSAvdeccProxy_APC *self );

bool JDKSAvdeccProxy_APC_send( struct JDKSAvdeccProxy_APC *self, struct jdksavdecc_frame const *avdecc_frame );

void JDKSAvdeccProxy_APC_CloseTcpConnection( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_ConnectToProxy( struct JDKSAvdeccProxy_APC *self, struct JDKSAvdeccProxy_APC_Addr const *addr );

bool JDKSAvdeccProxy_APC_GetHttpResponse( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_Initialize( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_NotifyLinkStatus( struct JDKSAvdeccProxy_APC *self, struct jdksavdecc_appdu const *linkMsg );

void JDKSAvdeccProxy_APC_ProcessMsg( struct JDKSAvdeccProxy_APC *self, struct jdksavdecc_appdu const *apsMsg );

void JDKSAvdeccProxy_APC_SendIdRequest( struct JDKSAvdeccProxy_APC *self,
                                        struct jdksavdecc_eui48 primaryMac,
                                        struct jdksavdecc_eui64 entity_id );

void JDKSAvdeccProxy_APC_SendHttpRequest( struct JDKSAvdeccProxy_APC *self, struct JDKSAvdeccProxy_APC_Addr const *addr );

void JDKSAvdeccProxy_APC_SendMsgToAps( struct JDKSAvdeccProxy_APC *self, struct jdksavdecc_appdu const *apcMsg );

void JDKSAvdeccProxy_APC_SendNopToAps( struct JDKSAvdeccProxy_APC *self );

void JDKSAvdeccProxy_APC_NotifyNewEntityId( struct JDKSAvdeccProxy_APC *self, struct jdksavdecc_eui64 entity_id );

#ifdef __cplusplus
}
#endif
