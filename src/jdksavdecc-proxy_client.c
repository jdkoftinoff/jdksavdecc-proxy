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
#include "jdksavdecc-proxy_client.h"

void JDKSAvdeccProxy_Client_init( struct JDKSAvdeccProxy_Client *self, struct JDKSAvdeccProxy_NetRequest *net )
{
    self->m_net = net;
    self->m_base.netEventSocketReadable = JDKSAvdeccProxy_Client_NetEventSocketReadable;
    self->m_base.netEventSocketConnected = JDKSAvdeccProxy_Client_NetEventSocketConnected;
    self->m_base.netEventSocketWritable = JDKSAvdeccProxy_Client_NetEventSocketWritable;
    self->m_base.netEventSocketClosed = JDKSAvdeccProxy_Client_NetEventSocketClosed;
    self->m_base.netEventSocketTimePassed = JDKSAvdeccProxy_Client_NetEventSocketTimePassed;
    self->closeTcpConnection = JDKSAvdeccProxy_Client_CloseTcpConnection;
    self->connectToProxy = JDKSAvdeccProxy_Client_ConnectToProxy;
    self->getHttpResponse = JDKSAvdeccProxy_Client_GetHttpResponse;
    self->initialize = JDKSAvdeccProxy_Client_Initialize;
    self->notifyLinkStatus = JDKSAvdeccProxy_Client_NotifyLinkStatus;
    self->processMsg = JDKSAvdeccProxy_Client_ProcessMsg;
    self->sendHttpRequest = JDKSAvdeccProxy_Client_SendHttpRequest;
    self->sendIdRequest = JDKSAvdeccProxy_Client_SendIdRequest;
    self->sendMsgToAps = JDKSAvdeccProxy_Client_SendMsgToAps;
    self->sendNopToAps = JDKSAvdeccProxy_Client_SendNopToAps;
    self->notifyNewEntityId = JDKSAvdeccProxy_Client_NotifyNewEntityId;
}

void JDKSAvdeccProxy_Client_terminate( struct JDKSAvdeccProxy_Client *self )
{
    /* Nothing to do here */
}

void JDKSAvdeccProxy_Client_start( struct JDKSAvdeccProxy_Client *self,
                                   struct JDKSAvdeccProxy_Client_Addr const *addr,
                                   struct jdksavdecc_eui48 primaryMac,
                                   struct jdksavdecc_eui64 preferred_entity_id )
{
    self->m_addr = *addr;
    self->m_primaryMac = primaryMac;
    self->m_entityId = preferred_entity_id;
    JDKSAvdeccProxy_Client_goto_state_BEGIN( self );
}

void JDKSAvdeccProxy_Client_finish( struct JDKSAvdeccProxy_Client *self )
{
    self->m_finished = true;
}

void JDKSAvdeccProxy_Client_NetEventSocketReadable( struct JDKSAvdeccProxy_NetEvent *self, void const *data, ssize_t data_len )
{
    /* TODO */
}

void JDKSAvdeccProxy_Client_NetEventSocketConnected( struct JDKSAvdeccProxy_NetEvent *self )
{
    /* TODO */
}

void JDKSAvdeccProxy_Client_NetEventSocketWritable( struct JDKSAvdeccProxy_NetEvent *self )
{
    /* TODO */
}

void JDKSAvdeccProxy_Client_NetEventSocketClosed( struct JDKSAvdeccProxy_NetEvent *self )
{
    /* TODO */
}

void JDKSAvdeccProxy_Client_NetEventSocketTimePassed( struct JDKSAvdeccProxy_NetEvent *self,
                                                      jdksavdecc_timestamp_in_milliseconds timestamp )
{
    /* TODO */
}

void JDKSAvdeccProxy_Client_CloseTcpConnection( struct JDKSAvdeccProxy_Client *self )
{
    /* TODO */
}

void JDKSAvdeccProxy_Client_ConnectToProxy( struct JDKSAvdeccProxy_Client *self,
                                            struct JDKSAvdeccProxy_Client_Addr const *addr )
{
    /* TODO */
}

bool JDKSAvdeccProxy_Client_GetHttpResponse( struct JDKSAvdeccProxy_Client *self )
{
    /* TODO */
    return false;
}

void JDKSAvdeccProxy_Client_Initialize( struct JDKSAvdeccProxy_Client *self )
{
    /* See IEEE Std 1722.1 Annex C.5.3.2.4 */
    self->m_apcMsgOut = false;
    self->m_apsMsgIn = false;
    self->m_finished = false;
    self->m_idAssigned = false;
    self->m_incomingTcpClosed = false;
    self->m_linkStatusMsg = false;
    self->m_responseValid = false;
    self->m_tcpConnected = false;
}

void JDKSAvdeccProxy_Client_NotifyLinkStatus( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_appdu const *linkMsg )
{
    /* TODO This is normally overridden by application */
}

void JDKSAvdeccProxy_Client_ProcessMsg( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_appdu const *apsMsg )
{
    /* TODO This is normally overridden by application */
}

void JDKSAvdeccProxy_Client_NotifyNewEntityId( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_eui64 entity_id )
{
    /* TODO This is normally overriden by application */
}

void JDKSAvdeccProxy_Client_SendIdRequest( struct JDKSAvdeccProxy_Client *self,
                                           struct jdksavdecc_eui48 primaryMac,
                                           struct jdksavdecc_eui64 entity_id )
{
    /* TODO Form and Send ID Request */
}

void JDKSAvdeccProxy_Client_SendHttpRequest( struct JDKSAvdeccProxy_Client *self,
                                             struct JDKSAvdeccProxy_Client_Addr const *addr )
{
    /* TODO Form and send HTTP request */
}

void JDKSAvdeccProxy_Client_SendMsgToAps( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_appdu const *apcMsg )
{
    /* TODO form and send apcMsg */
}

void JDKSAvdeccProxy_Client_SendNopToAps( struct JDKSAvdeccProxy_Client *self )
{
    /* TODO form and semd a Nop msg */
}

void JDKSAvdeccProxy_Client_execute_state( struct JDKSAvdeccProxy_Client *self )
{
    switch ( self->m_state )
    {
    case JDKSAvdeccProxy_Client_State_BEGIN:
        JDKSAvdeccProxy_Client_execute_state_BEGIN( self );
        break;
    case JDKSAvdeccProxy_Client_State_INITIALIZE:
        JDKSAvdeccProxy_Client_execute_state_INITIALIZE( self );
        break;
    case JDKSAvdeccProxy_Client_State_WAIT_FOR_CONNECT:
        JDKSAvdeccProxy_Client_execute_state_WAIT_FOR_CONNECT( self );
        break;
    case JDKSAvdeccProxy_Client_State_ACCEPT:
        JDKSAvdeccProxy_Client_execute_state_ACCEPT( self );
        break;
    case JDKSAvdeccProxy_Client_State_START_TRANSFER:
        JDKSAvdeccProxy_Client_execute_state_START_TRANSFER( self );
        break;
    case JDKSAvdeccProxy_Client_State_WAITING:
        JDKSAvdeccProxy_Client_execute_state_WAITING( self );
        break;
    case JDKSAvdeccProxy_Client_State_CLOSED:
        JDKSAvdeccProxy_Client_execute_state_CLOSED( self );
        break;
    case JDKSAvdeccProxy_Client_State_LINK_STATUS:
        JDKSAvdeccProxy_Client_execute_state_LINK_STATUS( self );
        break;
    case JDKSAvdeccProxy_Client_State_RECEIVE_MSG:
        JDKSAvdeccProxy_Client_execute_state_RECEIVE_MSG( self );
        break;
    case JDKSAvdeccProxy_Client_State_SEND_MSG:
        JDKSAvdeccProxy_Client_execute_state_SEND_MSG( self );
        break;
    case JDKSAvdeccProxy_Client_State_ENTITY_ID_ASSIGNED:
        JDKSAvdeccProxy_Client_execute_state_ENTITY_ID_ASSIGNED( self );
        break;
    case JDKSAvdeccProxy_Client_State_SEND_NOP:
        JDKSAvdeccProxy_Client_execute_state_SEND_NOP( self );
        break;
    case JDKSAvdeccProxy_Client_State_FINISH:
        JDKSAvdeccProxy_Client_execute_state_FINISH( self );
        break;
    case JDKSAvdeccProxy_Client_State_END:
        JDKSAvdeccProxy_Client_execute_state_END( self );
        break;
    default:
        self->m_state = JDKSAvdeccProxy_Client_State_END;
        break;
    }
}

void JDKSAvdeccProxy_Client_goto_state_BEGIN( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_BEGIN;
}

void JDKSAvdeccProxy_Client_execute_state_BEGIN( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_INITIALIZE( self );
}

void JDKSAvdeccProxy_Client_goto_state_INITIALIZE( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_INITIALIZE;
    self->initialize( self );
    self->connectToProxy( self, &self->m_addr );
}

void JDKSAvdeccProxy_Client_execute_state_INITIALIZE( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_WAIT_FOR_CONNECT( self );
}

void JDKSAvdeccProxy_Client_goto_state_WAIT_FOR_CONNECT( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_WAIT_FOR_CONNECT;
}

void JDKSAvdeccProxy_Client_execute_state_WAIT_FOR_CONNECT( struct JDKSAvdeccProxy_Client *self )
{
    if ( self->m_tcpConnected )
    {
        JDKSAvdeccProxy_Client_goto_state_ACCEPT( self );
    }
    else if ( self->m_finished )
    {
        JDKSAvdeccProxy_Client_goto_state_FINISH( self );
    }
}

void JDKSAvdeccProxy_Client_goto_state_ACCEPT( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_ACCEPT;
    self->sendHttpRequest( self, &self->m_addr );
}

void JDKSAvdeccProxy_Client_execute_state_ACCEPT( struct JDKSAvdeccProxy_Client *self )
{
    if ( self->getHttpResponse( self ) )
    {
        JDKSAvdeccProxy_Client_goto_state_START_TRANSFER( self );
    }
    else
    {
        JDKSAvdeccProxy_Client_goto_state_CLOSED( self );
    }
}

void JDKSAvdeccProxy_Client_goto_state_START_TRANSFER( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_START_TRANSFER;
    self->sendIdRequest( self, self->m_primaryMac, self->m_entityId );
    self->m_nopTimeout = self->m_currentTime + 10000;
}

void JDKSAvdeccProxy_Client_execute_state_START_TRANSFER( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_WAITING( self );
}

void JDKSAvdeccProxy_Client_goto_state_WAITING( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_WAITING;
}

void JDKSAvdeccProxy_Client_execute_state_WAITING( struct JDKSAvdeccProxy_Client *self )
{
    if ( self->m_incomingTcpClosed )
    {
        JDKSAvdeccProxy_Client_goto_state_CLOSED( self );
    }
    else if ( self->m_finished )
    {
        JDKSAvdeccProxy_Client_goto_state_CLOSED( self );
    }
    else if ( self->m_linkStatusMsg )
    {
        JDKSAvdeccProxy_Client_goto_state_LINK_STATUS( self );
    }
    else if ( self->m_apsMsgIn )
    {
        JDKSAvdeccProxy_Client_goto_state_RECEIVE_MSG( self );
    }
    else if ( self->m_apcMsgOut )
    {
        JDKSAvdeccProxy_Client_goto_state_SEND_MSG( self );
    }
    else if ( self->m_idAssigned )
    {
        JDKSAvdeccProxy_Client_goto_state_ENTITY_ID_ASSIGNED( self );
    }
    else if ( self->m_currentTime > self->m_nopTimeout )
    {
        JDKSAvdeccProxy_Client_goto_state_SEND_NOP( self );
    }
}

void JDKSAvdeccProxy_Client_goto_state_CLOSED( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_CLOSED;
    self->closeTcpConnection( self );
}

void JDKSAvdeccProxy_Client_execute_state_CLOSED( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_FINISH( self );
}

void JDKSAvdeccProxy_Client_goto_state_LINK_STATUS( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_LINK_STATUS;
    self->notifyLinkStatus( self, &self->m_linkMsg );
    self->m_linkStatusMsg = false;
}

void JDKSAvdeccProxy_Client_execute_state_LINK_STATUS( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_WAITING( self );
}

void JDKSAvdeccProxy_Client_goto_state_RECEIVE_MSG( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_RECEIVE_MSG;
    self->processMsg( self, &self->m_apsMsg );
    self->m_apsMsgIn = false;
}

void JDKSAvdeccProxy_Client_execute_state_RECEIVE_MSG( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_WAITING( self );
}

void JDKSAvdeccProxy_Client_goto_state_SEND_MSG( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_SEND_MSG;
    self->sendMsgToAps( self, &self->m_apcMsg );
    self->m_apcMsgOut = false;
    self->m_nopTimeout = self->m_currentTime + 10000;
}

void JDKSAvdeccProxy_Client_execute_state_SEND_MSG( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_WAITING( self );
}

void JDKSAvdeccProxy_Client_goto_state_ENTITY_ID_ASSIGNED( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_ENTITY_ID_ASSIGNED;
    self->m_entityId = self->m_newId;
    self->notifyNewEntityId( self, self->m_entityId );
    self->m_nopTimeout = self->m_currentTime + 10000;
}

void JDKSAvdeccProxy_Client_execute_state_ENTITY_ID_ASSIGNED( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_WAITING( self );
}

void JDKSAvdeccProxy_Client_goto_state_SEND_NOP( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_SEND_NOP;
    self->sendNopToAps( self );
    self->m_nopTimeout = self->m_currentTime + 10000;
}

void JDKSAvdeccProxy_Client_execute_state_SEND_NOP( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_WAITING( self );
}

void JDKSAvdeccProxy_Client_goto_state_FINISH( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_FINISH;
}

void JDKSAvdeccProxy_Client_execute_state_FINISH( struct JDKSAvdeccProxy_Client *self )
{
    JDKSAvdeccProxy_Client_goto_state_END( self );
}

void JDKSAvdeccProxy_Client_goto_state_END( struct JDKSAvdeccProxy_Client *self )
{
    self->m_state = JDKSAvdeccProxy_Client_State_END;
}

void JDKSAvdeccProxy_Client_execute_state_END( struct JDKSAvdeccProxy_Client *self )
{
    /* Do Nothing */
}

bool JDKSAvdeccProxy_Client_send( struct JDKSAvdeccProxy_Client *self, struct jdksavdecc_frame const *avdecc_frame )
{
    bool r = false;

    if ( !self->m_apcMsgOut )
    {
        jdksavdecc_appdu_set_avdecc_from_apc(
            &self->m_apcMsg, avdecc_frame->dest_address, avdecc_frame->length, avdecc_frame->payload );
        self->m_apcMsgOut = true;
        r = true;
    }
    return r;
}
